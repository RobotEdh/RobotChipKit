
#include "WProgram.h"
#include <sys/attribs.h> //EDH used for __ISR
#define cli()  asm volatile("di") //turn intrupts off
#define sei()  asm volatile("ei") //turn intrupts on

#include "config.h"
#include "def.h"
#include "types.h"
#include "PSerial.h"
#include "PMultiWii.h"

/**************************************************************************************/
/***************             Global RX related variables           ********************/
/**************************************************************************************/


//RAW RC values will be store here
volatile uint16_t rcValue[RC_CHANS] = {1502, 1502, 1502, 1502, 1502, 1502, 1502, 1502}; // interval [1000;2000]


//Channel order
static uint8_t rcChannel[RC_CHANS]  = {ROLLPIN, PITCHPIN, YAWPIN, THROTTLEPIN, AUX1PIN,AUX2PIN,AUX3PIN,AUX4PIN};
static uint8_t PCInt_RX_Pins[PCINT_PIN_COUNT] = {PCINT_RX_BITS}; // if this slowes the PCINT readings we can switch to a define for each pcint bit


#define FAILSAFE_DETECT_TRESHOLD  985


/**************************************************************************************/
/***************                   RX Pin Setup                    ********************/
/**************************************************************************************/
void configureReceiver() {
  /******************    Configure each rc pin for PCINT    ***************************/
    // PCINT activation
    CNEN = 0x000090fc;   //enable cn2-7,12,15
}

/**************************************************************************************/
/***************               Standard RX Pins reading            ********************/
/**************************************************************************************/

   // predefined PC pin block (thanks to lianj)  - Version without failsafe
  #define RX_PIN_CHECK(pin_pos, rc_value_pos)                        \
    if (mask & PCInt_RX_Pins[pin_pos]) {                             \
      if (!(pin & PCInt_RX_Pins[pin_pos])) {                         \
        dTime = cTime-edgeTime[pin_pos];                             \
        if (900<dTime && dTime<2200) {                               \
          rcValue[rc_value_pos] = dTime;                             \
        }                                                            \
      } else edgeTime[pin_pos] = cTime;                              \
    }


#ifdef __cplusplus
extern "C" {
#endif
void __ISR(_CHANGE_NOTICE_VECTOR, ipl7) ChangeNotice_Handler(void) { //EDH
    uint8_t mask;
    uint8_t pin;
    uint16_t cTime,dTime;
    static uint16_t edgeTime[8];
    static uint8_t PCintLast;
  
    pin = RX_PCINT_PIN_PORT; // RX_PCINT_PIN_PORT indicates the state of each PIN for the arduino port dealing with Ports digital pins
   
    mask = pin ^ PCintLast;   // doing a ^ between the current interruption and the last one indicates wich pin changed
    cTime = micros();         // micros() return a uint32_t, but it is not usefull to keep the whole bits => we keep only 16 bits
    sei();                    // re enable other interrupts at this point, the rest of this interrupt is not so time critical and can be interrupted safely
    PCintLast = pin;          // we memorize the current state of all PINs [D0-D7]
  
    #if (PCINT_PIN_COUNT > 0)
      RX_PIN_CHECK(0,2);
    #endif
    #if (PCINT_PIN_COUNT > 1)
      RX_PIN_CHECK(1,4);
    #endif
    #if (PCINT_PIN_COUNT > 2)
      RX_PIN_CHECK(2,5);
    #endif
    #if (PCINT_PIN_COUNT > 3)
      RX_PIN_CHECK(3,6);
    #endif
    #if (PCINT_PIN_COUNT > 4)
      RX_PIN_CHECK(4,7);
    #endif
    #if (PCINT_PIN_COUNT > 5)
      RX_PIN_CHECK(5,0);
    #endif
    #if (PCINT_PIN_COUNT > 6)
      RX_PIN_CHECK(6,1);
    #endif
    #if (PCINT_PIN_COUNT > 7)
      RX_PIN_CHECK(7,3);
    #endif
    
      PCintLast = RX_PCINT_PIN_PORT; // Read PORT to clear mismatch condition    
      IFS1CLR = 0x0001; // Be sure to clear the CN interrupt status
                  // flag before exiting the service routine.
#ifdef __cplusplus
}
#endif                  
               
}

uint16_t readRawRC(uint8_t chan) {
  uint16_t data;
 
  cli(); // Let's disable interrupts
  data = rcValue[rcChannel[chan]]; // Let's copy the data Atomically
  sei(); // Let's restore interrupt state

  return data; // We return the value correctly copied when the IRQ's where disabled
}

/**************************************************************************************/
/***************          compute and Filter the RX data           ********************/
/**************************************************************************************/
void computeRC() {
  static uint16_t rcData4Values[RC_CHANS][4], rcDataMean[RC_CHANS];
  static uint8_t rc4ValuesIndex = 0;
  uint8_t chan,a;
//  Serial.println(" Start computeRC");

    rc4ValuesIndex++;
    if (rc4ValuesIndex == 4) rc4ValuesIndex = 0;
    for (chan = 0; chan < RC_CHANS; chan++) {
        rcData4Values[chan][rc4ValuesIndex] = readRawRC(chan);
        rcDataMean[chan] = 0;
        for (a=0;a<4;a++) rcDataMean[chan] += rcData4Values[chan][a];
        rcDataMean[chan]= (rcDataMean[chan]+2)>>2;
        if ( rcDataMean[chan] < (uint16_t)rcData[chan] -3)  rcData[chan] = rcDataMean[chan]+2;
        if ( rcDataMean[chan] > (uint16_t)rcData[chan] +3)  rcData[chan] = rcDataMean[chan]-2;
        if (chan<8 && rcSerialCount > 0) { // rcData comes from MSP and overrides RX Data until rcSerialCount reaches 0
            rcSerialCount --;
            if (rcSerial[chan] >900) {rcData[chan] = rcSerial[chan];} // only relevant channels are overridden
        }
    }
}





