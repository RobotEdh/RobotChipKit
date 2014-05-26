#ifndef SENSORS_H_
#define SENSORS_H_

extern double c_angle[2];
double Patan2(double x, double y);

void ACC_Common();
void ACC_getADC ();
bool ACC_init ();
void GYRO_Common();
void Gyro_getADC ();
bool Gyro_init();

void i2c_getSixRawADC(uint8_t add, uint8_t reg);
void i2c_getTemperature(uint8_t add, uint8_t reg);
void i2c_init();
size_t i2c_read_reg_to_buf(uint8_t devAddr, uint8_t regAddr, void *buf, size_t size);
uint8_t i2c_writeReg(uint8_t devAddr, uint8_t regAddr, uint8_t val);

bool initSensors();
bool MPU_init();

#endif /* SENSORS_H_ */
