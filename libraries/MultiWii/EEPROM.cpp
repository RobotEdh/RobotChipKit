//#include "Arduino.h"
#include "WProgram.h"


uint8_t calculate_sum(uint8_t *cb , uint8_t siz) {

}

void readGlobalSet() {
}
 
bool readEEPROM() {
  }
 

void writeGlobalSet(uint8_t b) {

}
 
void writeParams(uint8_t b) {

}

void update_constants() { 

}

void LoadDefaults() {

}

#ifdef LOG_PERMANENT
void readPLog(void) {

}
void writePLog(void) {

}
#endif

#if defined(GPS_NAV)
//Stores the WP data in the wp struct in the EEPROM
void storeWP() {

}

// Read the given number of WP from the eeprom, supposedly we can use this during flight.
// Returns true when reading is successfull and returns false if there were some error (for example checksum)
bool recallWP(uint8_t wp_number) {

}

// Returns the maximum WP number that can be stored in the EEPROM, calculated from conf and plog sizes, and the eeprom size
uint8_t getMaxWPNumber() {

}

#endif
