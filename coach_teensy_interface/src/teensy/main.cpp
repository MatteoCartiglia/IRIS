#include "aer.h"
#include "lib/mcp/Adafruit_MCP23017.h"
#include "chips.h"
#include "usb_serial.h"
#include "version.h"
#include "Wire.h"
#include "../usb_packets.h"

// Hard reset timing
// pre_hard_reset_delay_ms gives the (generous) time between sending HardResetImminent and starting the hardware reset.
// hard_reset_delay_ms is the time between pulling the on/off pin high and giving up and setting it low again.
// The Teensy resets 5s after the on/off pin gets pulled high, so waiting 6s should be plenty of time.

constexpr unsigned int pre_hard_reset_delay_ms = 2000;
constexpr unsigned int hard_reset_delay_ms = 6000;

// PINS
constexpr int ii_num_data_pins = 11;
int iid[ii_num_data_pins] = {15, 14, 13, 12, 11, 7, 6, 5, 4, 3, 2}; //expander
constexpr int ii_req = 1;
constexpr int ii_ack = 10;
constexpr int prst = 9; //expander
constexpr int srst = 8; //expander
int iidly[2] = {0, 1}; //expander
constexpr int c2f_ack = 2;
constexpr int c2f_req = 3;
constexpr int c2f_num_data_pins = 4;
int c2f_data[c2f_num_data_pins] = {20, 21, 22, 23};
constexpr int aero_ack = 4;
constexpr int aero_req = 5;
constexpr int aero_num_data_pins = 3;
int aero_data[aero_num_data_pins] = {9, 8, 7};
int adc_pins[4] = {27, 26, 24, 25};
int mux_pins[3] = {29, 31, 33};
int i2c_0[2] = {18, 19};
constexpr int expander_reset = 0;
constexpr int hard_reset = 6;
constexpr int photoceptor_LED = 28;
int current_range[2] = {11, 12};
int led_pins[3] = {30, 32};
constexpr int detect_version_pin = 17;

// buffers
constexpr int num_c2fs = 16;
constexpr int rx_buf_size = 1024;
constexpr int tx_buf_size = 1024;
constexpr int ac_buf_size = 511;
uint16_t c2f_events[num_c2fs] = {0};
uint8_t tx_buf[tx_buf_size];
uint8_t rx_buf[rx_buf_size];
uint8_t aero_buf[tx_buf_size - 2];
uint16_t dac_buf[ac_buf_size];
uint16_t adc_buf[ac_buf_size];

bool aero_flag = false;
uint32_t aero_duration = 0;
constexpr int aer_delays = 0; // Delay in microseconds after toggling aer handshake output pins

// AC
uint16_t num_waveform_values = ac_buf_size;
static constexpr byte dac_channel_map[8] = {3, 2, 1, 0, 7, 6, 5, 4};
static constexpr byte adc_channel_map[8] = {5, 7, 6, 4, 1, 2, 0, 3};

elapsedMicros since_blank_micro;
elapsedMillis since_blank_milli;

Adafruit_MCP23017 mcp;
DAC53608 dac[4];

AER_in aero(aero_req,
            aero_ack,
            aero_data,
            aero_num_data_pins,
            aero_buf,
            aer_delays,
            10,
            false);
AER_in c2f(c2f_req,
           c2f_ack,
           c2f_data,
           c2f_num_data_pins,
           NULL,
           aer_delays,
           0,
           false);
AER_out ii(ii_req,
           ii_ack,
           iid,
           ii_num_data_pins,
           aer_delays,
           true);


// TEENSY 2 PC USB
void setHeader(int type, int len) {
  tx_buf[0] = (type << 2) | (len >> 8);
  tx_buf[1] = len & 0xFF;
}

void sendStatus(plane::TeensyStatus msg) {
  setHeader(1, 1);
  tx_buf[2] = (uint8_t) msg;
  usb_serial_write(tx_buf, 3);
}

void sendOneReading(int msg) {
  setHeader(3, 2);
  tx_buf[2] = msg >> 8;
  tx_buf[3] = msg;
  usb_serial_write(tx_buf, 4);
}

void sendReadings() {
  setHeader(4, num_waveform_values*2);
  for (int i = 0; i < num_waveform_values; i++)
  {
    tx_buf[2+2*i] = adc_buf[i] >> 8;
    tx_buf[3+2*i] = adc_buf[i];
  }
  usb_serial_write(tx_buf, 2*num_waveform_values + 2);
}

// AER
void aeroISR() {
  if (!aero.reqRead()) {
    if (aero.get_index() >= tx_buf_size - 2) { // buffer full
      setHeader(5, aero.get_index());
      for (int i = 0; i < aero.get_index(); i++)
      {
        tx_buf[2+i] = aero_buf[i];
      }
      usb_serial_write(tx_buf, 2 + aero.get_index());
      aero.set_index(0);
    }
    aero.record_event();
    aero.ackWrite(0);
  }
  if (aero.reqRead()) {
    aero.ackWrite(1);
  }
}

void c2fISR() {
  if (!c2f.reqRead()) {
    c2f_events[c2f.dataRead()]++;
    c2f.ackWrite(0);
  }
  if (c2f.reqRead()) {
    c2f.ackWrite(1);
  }
}

bool sendII(uint32_t x) {
  bool success = true;
  uint32_t msb = (~(x>>10) & 0x3FF);
  uint32_t lsb = (~x & 0x3FF) | (1<<10);

  digitalWriteFast(led_pins[1], 0);
  success &= ii.dataWrite_handshake(msb, mcp);
  success &= ii.dataWrite_handshake(lsb, mcp);
  digitalWriteFast(led_pins[1], 1);

  return success;
}

// ADC AND DAC
void setSelect(int value, int num_pins=3) {
  for (int i=0; i<num_pins; i++) {
    bool x = value & ( 1 << i );
    digitalWriteFast(mux_pins[i], x);
  }
}

uint16_t adcRead(int channel, int delay_val) {
  setSelect(adc_channel_map[channel%8]);
  delayMicroseconds(delay_val); // wait for ADC to read and average result
  return analogRead(adc_pins[channel/8]);
}

// dac_func is used to pass the ability to set dac as an argument to other functions
int dac_channel_selector = 0;
void dacFunc (uint16_t dac_value)
{
  dac[dac_channel_selector/8].setDAC(dac_channel_map[dac_channel_selector%8], dac_value);
}

// adc_func is used to pass the ability to read adc as an argument to other functions
int adc_channel_selector = 0;
uint16_t adcFunc ()
{
  return adcRead(adc_channel_selector, 0);
}

int currentRange(int adc_channel) {
  if ((adc_channel == 30) | (adc_channel == 31)) //GO21 and GO20 pins
  {
    return digitalReadFast(current_range[0]);
  }
  else if ((adc_channel == 20) | (adc_channel == 21)) //GO23 and GO22 pins
  {
    return digitalReadFast(current_range[1]);
  }
  else return -1;
}

// RESET
void reset() {
  ii.reqWrite(LOW);
  c2f.ackWrite(HIGH);
  aero.ackWrite(HIGH);

  digitalWriteFast(led_pins[0], LOW);
  digitalWriteFast(led_pins[1], LOW);
  mcp.digitalWrite(srst, LOW);
  delay(1);
  mcp.digitalWrite(prst, LOW);
  delay(100);

  digitalWriteFast(led_pins[0], HIGH);
  digitalWriteFast(led_pins[1], HIGH);
  mcp.digitalWrite(prst, HIGH);
  delay(1);
  mcp.digitalWrite(srst, HIGH);
  delay(100);

  for (int i = 0; i < 32; i++)
  {
    dac[i/8].setDAC(i%8, 0);
  }

  analogReadResolution(12);
  analogReadAveraging(4);
}

// SETUP
void setup() {
  // LED pins
  pinMode(led_pins[0], OUTPUT);
  pinMode(led_pins[1], OUTPUT);

  // Current range detecting pins
  pinMode(current_range[0], INPUT_PULLUP);
  pinMode(current_range[1], INPUT_PULLUP);

  // ADC
  for (int i=0; i<3; i++) {
    pinMode(mux_pins[i], OUTPUT);
    digitalWriteFast(mux_pins[i], LOW);
  }

  // DAC
  Wire.begin();
  dac[0].begin(0b1001000, Wire);
  dac[1].begin(0b1001001, Wire);
  dac[2].begin(0b1001010, Wire);
  dac[3].begin(0b1001011, Wire);

  // I2C Expander
  pinMode(expander_reset, OUTPUT);
  digitalWrite(expander_reset, LOW);
  delay(100);
  digitalWrite(expander_reset, HIGH);
  mcp.begin();
  for (int i=0; i<11; i++) {
    mcp.pinMode(iid[i], OUTPUT);
  }
  mcp.pinMode(prst, OUTPUT);
  mcp.pinMode(srst, OUTPUT);
  mcp.pinMode(iidly[0], OUTPUT);
  mcp.pinMode(iidly[1], OUTPUT);
  mcp.digitalWrite(iidly[0], LOW);
  mcp.digitalWrite(iidly[1], LOW);

  // MISC PINS
  pinMode(photoceptor_LED, OUTPUT);
  pinMode(hard_reset, OUTPUT);
  digitalWrite(hard_reset, LOW);
  pinMode(hard_reset, OUTPUT);
  digitalWrite(hard_reset, LOW);
  pinMode(detect_version_pin, INPUT_PULLDOWN);

  reset();
  digitalWrite(13, HIGH); // LED on TEENSY
}

// MAIN LOOP
void loop()
{
  if (usb_serial_available()) {
    usb_serial_read(rx_buf, rx_buf_size);

    uint16_t header = (rx_buf[0] << 8) | rx_buf[1];
    uint8_t pkt_type = header >> plane::PKT_HDR_PKT_TYPE_SHIFT;
    uint16_t pkt_len = header & (plane::MAX_PKT_LEN-1);
    uint8_t *pkt_data = &rx_buf[2];

    aero.record_event_manual(7);

    switch (pkt_type) {
      case 1: { // reset
        if (pkt_data[0] == 0) // soft
        {
          reset();
          sendStatus(plane::TeensyStatus::Success);
        }
        else //hard
        {
          // detect_version_pin is high on v0.3 boards
          if (digitalReadFast(detect_version_pin))
          {
            sendStatus(plane::TeensyStatus::HardResetNotSupported);
          }
          else
          {
            // Send HardResetImminent immediately to notify the host that it can disconnect, and wait to allow it to do so.
            sendStatus(plane::TeensyStatus::HardResetImminent);
            delay(pre_hard_reset_delay_ms);

            // To start the reset, the hard reset line needs to be held high for at least 5s, after which the reset should occur.
            digitalWrite(hard_reset, HIGH);
            delay(hard_reset_delay_ms);

            // This point should never be reached, but if it is, the reset has failed to take effect,
            // so set the hard_reset back to normal and send a failure message
            // (though this will not be received if the host has disconnected).
            digitalWrite(hard_reset, LOW);
            sendStatus(plane::TeensyStatus::HardResetFailed);
          }
        }
      }
      break;

      case 2: { // get firmware version
        setHeader(2, 3);
        tx_buf[2] = PLANE_VERSION_MAJOR;
        tx_buf[3] = PLANE_VERSION_MINOR;
        tx_buf[4] = PLANE_VERSION_PATCH;
        usb_serial_write(tx_buf, 5);
      }
      break;

      case 3: { // set dc voltage
        int value = (pkt_data[0] << 8) | pkt_data[1];
        int channel = pkt_data[2];
        dac[channel/8].setDAC(dac_channel_map[channel%8], value);
        sendStatus(plane::TeensyStatus::Success);
      }
      break;

      case 4: { // set voltages in waveform
        for (int i = 0; i < pkt_len/2; i++)
        {
          dac_buf[i] = (pkt_data[2*i] << 8) | pkt_data[2*i + 1];
        }
        num_waveform_values = pkt_len/2;
        sendStatus(plane::TeensyStatus::Success);
      }
      break;

      case 5: { // set current
        int ranged_value = (pkt_data[0] << 8) | pkt_data[1];
        int dac_channel = pkt_data[2];
        int adc_channel = pkt_data[3];

        int value = ranged_value & plane::RANGED_VALUE_VALUE_MASK;
        bool desired_current_range = ranged_value & plane::RANGED_VALUE_RANGE_MASK;

        dac_channel_selector = dac_channel;
        adc_channel_selector = adc_channel;

        digitalWriteFast(led_pins[0], LOW);
        int range = currentRange(adc_channel);
        if (range != -1 && range != desired_current_range)
        {
          sendStatus(plane::TeensyStatus::IncorrectCurrentSwitchRange);
        }
        else
        {
          int16_t current = setCurrent(dacFunc,
                                        adcFunc,
                                        value,                  // Desired value of current
                                        0,                      // For now, assume min dac value for all  pins are 0
                                        1023);                  // For now, assume max dac value for all pins are 1.8V
          if (current == -1)
          {
            sendStatus(plane::TeensyStatus::CurrentCannotBeSet);
          }
          else if (current == -2)
          {
            sendStatus(plane::TeensyStatus::CurrentOutsideSearchRange);
          }
          else
          {
            if (range == 1)
            {
                current |= int16_t(plane::CurrentRange::Low);
            }
            sendOneReading(current);
          }
        }
        digitalWriteFast(led_pins[0], HIGH);
      }
      break;

      case 6: { // Set LED
        analogWrite(photoceptor_LED, pkt_data[0]);
        sendStatus(plane::TeensyStatus::Success);
      }
      break;

      case 7: { // Set adc bit depth
        if (pkt_data[0] == 12)
        {
          analogReadAveraging(4);
        }
        else
        {
          analogReadAveraging(1);
        }
        analogReadResolution(12);
        sendStatus(plane::TeensyStatus::Success);
      }
      break;

      case 8: { // request one reading
        int adc_channel = pkt_data[0];

        uint16_t reading = adcRead(adc_channel, 0) |
                           ((currentRange(adc_channel) & 0x1) << plane::RANGED_VALUE_VALUE_BITS);

        sendOneReading(reading);
      }
      break;

      case 9: { // request readings
        uint32_t delay_val = (pkt_data[0] << 8) | pkt_data[1];
        int dac_channel = pkt_data[2];
        int adc_channel = pkt_data[3];

        Wire.setClock(1000000); // 1 MHz (Fast+ mode I2C sck frequency)
        int selectDac = dac_channel/8;
        int channelInDac = dac_channel_map[dac_channel%8];

        since_blank_micro = 0;
        for(int i = 0; i < num_waveform_values; i++)
        {
          dac[selectDac].setDAC(channelInDac, dac_buf[i]);
          while(since_blank_micro <= delay_val) {};
          since_blank_micro = 0;
          adc_buf[i] = adcRead(adc_channel, 0);
        }

        Wire.setClock(100000); // 100 kHz (standard I2C sck frequency)

        sendReadings();
      }
      break;

      case 10: { // request transient response
        int step_value = (pkt_data[0] << 8) | pkt_data[1];
        uint32_t delay_val = (pkt_data[2] << 8) | pkt_data[3];
        int dac_channel = pkt_data[4];
        int adc_channel = pkt_data[5];

        adc_buf[0] = adcRead(adc_channel, 0);
        dac[dac_channel/8].setDAC(dac_channel_map[dac_channel%8], step_value);
        for(int i = 1; i < num_waveform_values; i++)
        {
          while(since_blank_micro <= delay_val) {};
          since_blank_micro = 0;
          adc_buf[i] = adcRead(adc_channel, 0);
        }
        sendReadings();
      }
      break;

      case 11: { // request AERO packet
        aero_duration = (pkt_data[0] << 8) | pkt_data[1];
        aero_flag = true;

        aero.set_index(0);
        aero.set_t0(micros());
        since_blank_milli = 0;
        attachInterrupt(digitalPinToInterrupt(aero_req), aeroISR, CHANGE);
        aero.ackWrite(aero.reqRead());
      }
      break;

      case 12: { // request C2F packet
        int duration = (pkt_data[0] << 8) | pkt_data[1];

        for(int i = 0; i<16; i++){
          c2f_events[i] = 0;
        }

        attachInterrupt(digitalPinToInterrupt(c2f_req), c2fISR, CHANGE);
        c2f.ackWrite(c2f.reqRead());
        delay(duration);
        detachInterrupt(digitalPinToInterrupt(c2f_req));

        setHeader(7, 32);
        for (int i = 0; i < num_c2fs; i++)
        {
          tx_buf[2+2*i] = c2f_events[i] >> 8;
          tx_buf[3+2*i] = c2f_events[i];
        }
        usb_serial_write(tx_buf, 2*num_c2fs + 2);
      }
      break;

      case 14: { //CoACH Events
        bool success = true;
        for (int i = 0; i < pkt_len/4; i++)
        {
          int event = (pkt_data[4*i] << 24) |
                      (pkt_data[4*i + 1] << 16) |
                      (pkt_data[4*i + 2] << 8) |
                      (pkt_data[4*i + 3]);

          success &= sendII(event);
          success &= sendII(event);
          success &= sendII(0x41000);
        }

        if (success)
        {
          sendStatus(plane::TeensyStatus::Success);
        }
        else
        {
          sendStatus(plane::TeensyStatus::AerHandshakeFailed);
        }
      }
      break;

      default: {
        sendStatus(plane::TeensyStatus::UnknownCommand);
      }
      break;
    }
  }

  if(aero_flag & (since_blank_milli >= aero_duration))
  {
    detachInterrupt(digitalPinToInterrupt(aero_req));
    aero_flag = false;

    setHeader(6, aero.get_index());
    for (int i = 0; i < aero.get_index(); i++)
    {
      tx_buf[2+i] = aero_buf[i];
    }
    usb_serial_write(tx_buf, 2 + aero.get_index());
  }
}
