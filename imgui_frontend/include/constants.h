//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------


// Serial port constants
#define PORT_NAME                   "/dev/ttyACM0"
                                    //"/dev/cu.usbmodem105688601";  
                                    //"cu.usbmodem105688601";           ALIVE non soldered board
                                    //"/cu.usbmodem105661701";          ALIVE BOARD

// AER-related constants
#define AER_NO_CORES                2
#define AER_NO_SYNAPSE_TYPES        4
#define AER_NO_NEURONS              4
#define AER_NO_NMDA_SYNAPSES        40
#define AER_NO_GABAa_SYNAPSES       2
#define AER_NO_GABAb_SYNAPSES       16
#define AER_NO_AMPA_SYNAPSES_CC     6
#define AER_NO_AMPA_SYNAPSES_NN     3       // 3 AMPA+ and 3 AMPA-

#define AER_DECODER_BIT_0_PIN       18
#define AER_DECODER_BIT_1_PIN       19
#define AER_DECODER_BIT_2_PIN       20
#define AER_DECODER_BIT_3_PIN       21
#define AER_DECODER_BIT_4_PIN       22
#define AER_DECODER_BIT_5_PIN       23
#define AER_DECODER_BIT_6_PIN       24
#define AER_DECODER_BIT_7_PIN       25
#define AER_DECODER_BIT_8_PIN       26

// DAC-related constants
#define DAC_BIASFILE                "data/BiasValues_DAC.csv"
#define DAC_CHANNELS_USED           11
#define DAC_COMMAND_WRITE_UPDATE    48      // 0011 0000
#define DAC_MAX_VOLTAGE             1800    // mV

// BIASGEN-related constants
#define BIASGEN_BIASFILE            "data/BiasValues_BIASGEN.csv"
#define BIASGEN_CHANNELS            54
#define BIASGEN_CATEGORIES          9
#define BIASGEN_MAX_VOLTAGE         1    // mV

// GUI-related constants
#define BUTTON_HEIGHT               20
#define BUTTON_UPDATE_WIDTH         100
#define BUTTON_AER_WIDTH            285
#define WINDOW_HEIGHT               800
#define WINDOW_WIDTH                1000
#define CLEAR_COLOUR_X              0.45f
#define CLEAR_COLOUR_Y              0.55f
#define CLEAR_COLOUR_Z              0.60f
#define CLEAR_COLOUR_W              1.00f



#define NUMBER_CHANNELS     8

#define bias_type           0x0001
#define bias_fine           0x01FE
#define bias_course         0x0E00
#define FINE_BIAS_SHIFT     1
#define COURSE_BIAS_SHIFT   9