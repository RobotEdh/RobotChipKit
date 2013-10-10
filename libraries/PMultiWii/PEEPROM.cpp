#include "WProgram.h"


#include "config.h"
#include "def.h"
#include "types.h"
#include "PEEPROM.h"
#include "PMultiWii.h"

void LoadDefaults() {
    uint8_t i;

    // PID
    conf.pid[ROLL].P8     = 33;  conf.pid[ROLL].I8    = 30; conf.pid[ROLL].D8     = 23;
    conf.pid[PITCH].P8    = 33;  conf.pid[PITCH].I8   = 30; conf.pid[PITCH].D8    = 23;
    conf.pid[YAW].P8      = 68;  conf.pid[YAW].I8     = 45;  conf.pid[YAW].D8     = 0;
 
    // Rate Roll/Pitch & Yaw
    conf.rollPitchRate = 0;
    conf.yawRate = 0;
    
    // Throttle attenuation
    conf.dynThrPID = 0;
 
    // RC Rate & expo
    conf.rcRate8 = 100; conf.rcExpo8 = 0; //rate 100%, no expo
    for(i=0;i<5;i++) {    // 500/128 = 3.90625    3.9062 * 3.9062 = 15.259   1526*100/128 = 1192
       lookupPitchRollRC[i] = (1526+conf.rcExpo8*(i*i-15))*i*(int32_t)conf.rcRate8/1192; // ={0,128,256,384,512} if rate 100%, no expo
    }
    
    // Throttle Mid-expo
    conf.minthrottle = MINTHROTTLE;
    conf.thrMid8 = 50; conf.thrExpo8 = 0;  //linear value  (MID 50 EXPO 0)
    for(i=0;i<11;i++) {
       int16_t tmp = 10*i-conf.thrMid8;
       uint8_t y = 1;
       if (tmp>0) y = 100-conf.thrMid8;
       if (tmp<0) y = conf.thrMid8;
       lookupThrottleRC[i] = 10*conf.thrMid8 + tmp*( 100-conf.thrExpo8+(int32_t)conf.thrExpo8*(tmp*tmp)/(y*y) )/10; // [0;1000]
       lookupThrottleRC[i] = conf.minthrottle + (int32_t)(MAXTHROTTLE-conf.minthrottle)* lookupThrottleRC[i]/1000;  // [0;1000] -> [conf.minthrottle;MAXTHROTTLE]
     }
}


