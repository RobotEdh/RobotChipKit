#ifndef SERIAL_H_
#define SERIAL_H_

void serialCom();
void SerialOpen(uint32_t baud);
void SerialWrite(uint8_t port,uint8_t c);
uint8_t SerialAvailable(uint8_t port);
void debugmsg_append_str(const char *str);
void SerialEnd(uint8_t port);
uint8_t SerialPeek(uint8_t port);


#endif /* SERIAL_H_ */
