//---------------------------------------------------------------------------------------------------------------------------------------
//
//
// Author: Ciara Giles-Doran <gciara@student.ethz.ch>
// Last updated: 
//---------------------------------------------------------------------------------------------------------------------------------------


#define MAX_BG_CHANNELS     37
#define MAX_DAC_CHANNEL     12
#define DAC_COMMAND         3
#define DAC_COMMAND_SHIFT   4
#define NUMBER_CHANNELS     8

#define bias_type           0x0001
#define bias_fine           0x01FE
#define bias_course         0x0E00
#define FINE_BIAS_SHIFT     1
#define COURSE_BIAS_SHIFT   9

#define BIASGEN_POR_FILE    "data/biasgen_por.csv"
#define PORT_NAME           "/dev/ttyACM1"
                            //"/dev/cu.usbmodem105688601";  
                            //"cu.usbmodem105688601";       ALIVE non soldered board
                            //"/cu.usbmodem105661701";      ALIVE BOARD


