/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

/**
 *	Fonction mathématiques supplémentaires
 */

#ifndef Imu_h
#define Imu_h

#include "MPU_types.h"
#include "MPU6050.h"

#define ACCEL_LOWPASSFILTER_VAL  0.20
#define COMPLEMENTARY_FILTER_VAL 0.98

/************************/
/****** FUNCTIONS *******/
/************************/

void IMUInit();
void IMUExecution();

#endif