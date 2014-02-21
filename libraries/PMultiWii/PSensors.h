#ifndef SENSORS_H_
#define SENSORS_H_

void ACC_getADC ();

void Gyro_getADC ();

bool initSensors();

void ComputeEulerAngles();

#endif /* SENSORS_H_ */
