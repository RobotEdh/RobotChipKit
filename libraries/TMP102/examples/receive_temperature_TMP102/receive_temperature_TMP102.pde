
#include <VirtualWire.h>

void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_setup(2000);	 // Bits per sec

    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    uint8_t value[VW_MAX_VALUE_LEN];
    uint8_t len = VW_MAX_VALUE_LEN;
    uint8_t source;
    uint8_t type;

    if (vw_get_float(value, &len, source, type) && type == VW_TEMPERATURE_DATA_TYPE) // Non-blocking
    {
 	  // Message with a good checksum received, dump it.
	  Serial.print("Source: "); Serial.println(source);
	  int i;
	  for (i = 0; i < len; i++)
	  {
	      Serial.print(value[i]);
	  }
    }
}
