#ifndef EEPROM_H_
#define EEPROM_H_

#include "WProgram.h"

void eeprom_read_block (void *__dst, const void *__src, size_t size);
void eeprom_write_block (const void *__src, void *__dst, size_t size);
void readGlobalSet();
bool readEEPROM();
void update_constants();
void writeGlobalSet(uint8_t b);
void writeParams(uint8_t b);
void LoadDefaults();
void readPLog(void);
void writePLog(void);

#endif /* EEPROM_H_ */
