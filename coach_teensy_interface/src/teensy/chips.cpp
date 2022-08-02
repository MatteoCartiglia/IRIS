#include "chips.h"

void DAC53608::begin(const int addr, TwoWire& bus) {
    _addr = addr;
    _bus = &bus;
    write16(1, 0); // Turn on all outputs
}

void DAC53608::setRegisterPointer(uint8_t reg) {
    write16(reg, 0);
}

void DAC53608::setDAC(uint8_t channel, uint16_t value) {  
  write16(channel | (1<<3),
          value << 2);
}

int16_t DAC53608::read() {
  int16_t ret;
  
  _bus->requestFrom((int)_addr, 2);
  ret = _bus->read();
  ret <<= 8;
  ret |= _bus->read();

  return ret;
}

void DAC53608::write16(uint8_t reg, uint16_t d) {
  uint8_t temp = (uint8_t)d;
  d >>= 8;
  _bus->beginTransmission(_addr);

  _bus->write(reg);
  _bus->write((uint8_t)d);
  _bus->write(temp);

  _bus->endTransmission();
}

bool isInBetween(int val, int p1, int p2) {
  if ((val > p1) & (val < p2)) return true;
  else if ((val < p1) & (val > p2)) return true;
  else return false;
}

int16_t setCurrent(void (*dac_function)(uint16_t dac_value), uint16_t (*adc_function)(), int16_t value, int dacMin, int dacMax) {
  // Currents cannot be set directly, so instead we vary the output
  // of a dac until a desired current (value) is achieved. We assume
  // that the current as a function of voltage is monotonic. This allows
  // the use of an algorithm similar to binary search. It is likely that
  // the desired current cannot be set exactly, so this function tries
  // to set it as close as possible and returns this value. It returns
  // -1 if the desired current cannot be set using the allowable range
  // for the dac values or if some other error occured.

  int min = dacMin; // min dac value to search within
  int max = dacMax; // max dac value to search within
  int mid;          // dac value in the middle of the search space
  int16_t minI;     // current corresponding to min dac value
  int16_t maxI;     // current corresponding to max dac value
  int16_t midI;     // current corresponding to mid dac value
  int maxD;         // distance from maxI to desired current
  int minD;         // distance from minI to desired current

  dac_function(min);
  minI = adc_function();

  dac_function(max);
  maxI = adc_function();

  if (!isInBetween(value, minI, maxI)) {
    // error: Current cannot be set with allowable dac values
    return -1;
  }

  while (max>min+1) {
    mid = (max + min)/2;
    dac_function(mid);
    midI = adc_function();

    if (isInBetween(value, minI, midI)){
      max = mid;
      maxI = midI;
    }
    else if (isInBetween(value, midI, maxI)){
      min = mid;
      minI = midI;
    }
    else if (midI == value) {
      return midI;
    }
    else {
      // error: Current outside of search range
      return -2;
    }
  }

  maxD = abs(value - maxI);
  minD = abs(value - minI);
  if (maxD < minD) {
    dac_function(max);
  }
  else {
    dac_function(min);
  }
  
  return adc_function();

}