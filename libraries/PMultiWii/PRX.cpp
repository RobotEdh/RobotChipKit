#include "WProgram.h"
#include <sys/attribs.h> //used for __ISR

#include "config.h"
#include "def.h"
#include "types.h"
#include "PSerial.h"
#include "PMultiWii.h"

/**************************************************************************************/
/***************             Global RX related variables           ********************/
/**************************************************************************************/
//RAW RC values will be store here
volatile uint16_t rcValue[RC_CHANS] = {1502, 1502, 1502, 1502, 1502}; // interval [1000;2000]


/**************************************************************************************/
/***************                   RX Pin Setup                    ********************/
/**************************************************************************************/
void configureReceiver() {
  Serial.println(">configureReceiver");    
  asm volatile("di");     //disable CPU interrupts
  
  AD1PCFGSET = 0x0000003e; //set analog pins RB1-RB5 to digital 0x3e=111110
  TRISBSET =   0x0000003e; //set RB1-RB5 as inputs
  CNCON = 0x00008000;      //enable the CN module ON
  CNENSET = 0x000000f8;    //enable pins RB1-RB5 (CN3-CN7) for interrupt 0xf8=11111000
  CNPUESET = 0x000000f8;   //enable weak pull up for pins RB1-RB5 (CN3-CN7)
  PORTB;                   //read PortB to clear any mismatch
  IPC6SET = 0x001f0000;    //set priority to 7 sub 4 0x00060000 for priority 1 sub 0 
  IFS1CLR = 0x0001;        //clear the CN interrupt flag bit
  IEC1SET = 0x0001;        //enable the CN interrupt enable bit
  
  asm volatile("ei");      //enable CPU interrupts
}


#ifdef __cplusplus
extern "C" {
#endif
void __ISR(_CHANGE_NOTICE_VECTOR, ipl7) ChangeNotice_Handler(void) {  //priority 7 

    static uint32_t edgeTime[5];
    static uint16_t lastb; // previous PORTB
    uint16_t thisb; // current PORTB
    uint32_t currTime, dTime; // current time, delta time

    
    thisb = PORTB;          // read PORT B   
    currTime = micros();    

    if ((thisb ^ lastb) & 0x02) {  // RB1 changed (0x02= 10)  ROLLPIN    
      if (!(thisb & 0x02)) {       // RB1 is low                        
        dTime = currTime-edgeTime[0];                            
        if (900<dTime && dTime<2200) {  // filter erroneous values                             
          rcValue[0] = (uint16_t)dTime;                             
        }                                                            
      }
      else
        edgeTime[0] = currTime; // RB1 is high                            
    }        
        
    if ((thisb ^ lastb) & 0x04) {  // RB2 changed (0x04= 100)  PITCHPIN     
      if (!(thisb & 0x04)) {       // RB2 is low                        
        dTime = currTime-edgeTime[1];                             
        if (900<dTime && dTime<2200) {  // filter erroneous values                              
          rcValue[1] = (uint16_t)dTime;                             
        }                                                            
      }
      else
        edgeTime[1] = currTime; // RB2 is high                            
    }        

    if ((thisb ^ lastb) & 0x08) {  // RB3 changed (0x08= 1000)   YAWPIN    
      if (!(thisb & 0x08)) {       // RB3 is low                        
        dTime = currTime-edgeTime[2];                             
        if (900<dTime && dTime<2200) {  // filter erroneous values                              
          rcValue[2] = (uint16_t)dTime;                             
        }                                                            
      }
      else
        edgeTime[2] = currTime; // RB3 is high                            
    }        

   if ((thisb ^ lastb) & 0x10) {  // RB4 changed (0x10= 10000)   THROTTLEPIN  
      if (!(thisb & 0x10)) {       // RB4 is low                        
        dTime = currTime-edgeTime[3];                             
        if (900<dTime && dTime<2200) {  // filter erroneous values                              
          rcValue[3] = (uint16_t)dTime;                             
        }                                                            
      }
      else
        edgeTime[3] = currTime; // RB4 is high                            
    }        

   if ((thisb ^ lastb) & 0x20) {  // RB5 changed (0x20= 100000)   AUX1PIN    
      if (!(thisb & 0x20)) {       // RB5 is low                        
        dTime = currTime-edgeTime[4];                             
        if (900<dTime && dTime<2200) {   // filter erroneous values                             
          rcValue[4] = (uint16_t)dTime;                             
        }                                                            
      }
      else
        edgeTime[4] = currTime; // RB5 is high                            
    }        
   
    lastb = thisb;    // Memorize the current state of PORT B
    IFS1CLR = 0x0001; // Be sure to clear the CN interrupt status flag before exiting the service routine.
   
#ifdef __cplusplus
}
#endif                  
               
}

/**************************************************************************************
***************   read the RX data                    
**************************************************************************************/

uint16_t readRawRC(uint8_t chan) {
  uint16_t data;
   
  asm volatile("di");    // turn intrupts off
  data = rcValue[chan];  // Let's copy the data Atomically
  asm volatile("ei");    // turn intrupts on
 
  return data; // We return the value correctly copied when the IRQ's where disabled
}


/**********************************************************************/
/*                    computeRC                                       */
/*  - call readRawRC to get:                                          */
/*         rcData4Values[chan][rc4ValuesIndex]                        */
/*    and make average on 4 values to get:                            */
/*         rcData[chan] interval [1000;2000]                          */
/*  - compute rcCommand[axis]                                         */
/*            interval [-500;+500] for ROLL & PITCH & YAW             */
/*            interval [conf.minthrottle;MAXTHROTTLE] for THROTTLE    */
/**********************************************************************/

void computeRC() {
  static uint16_t rcData4Values[RC_CHANS][4], rcDataMean[RC_CHANS];
  static uint8_t rc4ValuesIndex = 0;
  uint16_t tmp,tmp2;
  uint8_t axis,prop1,prop2;
  uint8_t chan,a;
  
#if defined(TRACE)
    Serial.println(">computeRC");
#endif 

    rc4ValuesIndex++;
    if (rc4ValuesIndex == 4) rc4ValuesIndex = 0;
    
    for (chan = 0; chan < RC_CHANS; chan++) { // read data from all channels
        rcData4Values[chan][rc4ValuesIndex] = readRawRC(chan);
#if defined(TRACE2)  
        Serial.print("rcData4Values[");Serial.print((int)chan);Serial.print("][");
        Serial.print((int)rc4ValuesIndex);Serial.print("]:");Serial.println(rcData4Values[chan][rc4ValuesIndex]);
#endif
        rcDataMean[chan] = 0;
        for (a=0;a<4;a++) rcDataMean[chan] += rcData4Values[chan][a];  // make average on 4 values
        
        rcDataMean[chan]= (rcDataMean[chan]+2)>>2;
        if ( rcDataMean[chan] < (uint16_t)rcData[chan] -3)  rcData[chan] = rcDataMean[chan]+2; // filter in current value outside +/- 3 previous value
        if ( rcDataMean[chan] > (uint16_t)rcData[chan] +3)  rcData[chan] = rcDataMean[chan]-2;
#if defined(TRACE)  
        Serial.print("rcData[");Serial.print((int)chan);Serial.print("]:");Serial.println(rcData[chan]);
#endif       
    } // end read data from all channels
 
    
    // PITCH & ROLL only dynamic PID adjustemnt, depending on throttle value
    prop2 = 128; // prop2 was 100, is 128 now
    if (rcData[THROTTLE]>1500) { // breakpoint is fix: 1500
        if (rcData[THROTTLE]<2000) {
            prop2 -=  ((uint16_t)conf.dynThrPID*(rcData[THROTTLE]-1500)>>9); //  /512 instead of /500
        } else {
            prop2 -=  conf.dynThrPID;
        }
    }

    for(axis=0;axis<3;axis++) {
        tmp = min(abs(rcData[axis]-MIDRC),500);
        if(axis!=2) { //ROLL & PITCH
            tmp2 = tmp>>7; // 500/128 = 3.9  => range [0;3]
            rcCommand[axis] = lookupPitchRollRC[tmp2] + ((tmp-(tmp2<<7)) * (lookupPitchRollRC[tmp2+1]-lookupPitchRollRC[tmp2])>>7);
            prop1 = 128-((uint16_t)conf.rollPitchRate*tmp>>9); // prop1 was 100, is 128 now -- and /512 instead of /500
            prop1 = (uint16_t)prop1*prop2>>7; // prop1: max is 128   prop2: max is 128   result prop1: max is 128
            dynP8[axis] = (uint16_t)conf.pid[axis].P8*prop1>>7; // was /100, is /128 now
            dynD8[axis] = (uint16_t)conf.pid[axis].D8*prop1>>7; // was /100, is /128 now
        }
        else  // YAW
        {      
            rcCommand[axis] = tmp;
        }
        if (rcData[axis]<MIDRC) rcCommand[axis] = -rcCommand[axis];
#if defined(TRACE) 
        Serial.print("rcCommand[");Serial.print((int)axis);Serial.print("]:");Serial.println(rcCommand[axis]);
#endif         
    }
    tmp = constrain(rcData[THROTTLE],MINCHECK,2000);
    tmp = (uint32_t)(tmp-MINCHECK)*1000/(2000-MINCHECK); // [MINCHECK;2000] -> [0;1000]
    tmp2 = tmp/100;
    rcCommand[THROTTLE] = lookupThrottleRC[tmp2] + (tmp-tmp2*100) * (lookupThrottleRC[tmp2+1]-lookupThrottleRC[tmp2]) / 100; // [0;1000] -> expo -> [conf.minthrottle;MAXTHROTTLE]
#if defined(TRACE) 
    Serial.print("rcCommand[");Serial.print((int)THROTTLE);Serial.print("]:");Serial.println(rcCommand[THROTTLE]);
#endif
}




