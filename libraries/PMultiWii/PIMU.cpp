#include "WProgram.h"

#include "config.h"
#include "def.h"
#include "types.h"
#include "PMultiWii.h"
#include "PIMU.h"
#include "PSensors.h"


/****************************************************************/
/*                    computeIMU                                */
/*  - call ACC_getADC  to get:                                  */
/*                            imu.accADC[axis]                  */
/*  - call Gyro_getADC to get:                                  */
/*                            imu.gyroADC[axis]                 */
/****************************************************************/

void computeIMU () {
  
#if defined(TRACE)
  Serial.println(">>Start computeIMU");
#endif  

  ACC_getADC();
  
  Gyro_getADC();
    
#if defined(TRACE)  
  Serial.println("<<End computeIMU");
#endif  
}
