#ifndef OUTPUT_H_
#define OUTPUT_H_

void initOutput();
void RunMotors();
void writeServos();
void writeAllMotors(int16_t value);
void writeOneMotor(uint8_t no, int16_t value);
void writeMotors();

#endif /* OUTPUT_H_ */
