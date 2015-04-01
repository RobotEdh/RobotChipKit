
#include <VirtualWire.h>

void setup()
{
    Serial.begin(9600);	// Debugging only
   
    // Initialise the IO and ISR
    Serial.println("Setup VW");
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec

    Serial.println("Start RX");
    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    uint8_t value[VW_MAX_VALUE_LEN];
    uint8_t len = VW_MAX_VALUE_LEN;
    uint8_t source;
    uint8_t type;
   
    Serial.println("Loop");
    
    // Wait at most 200ms for a reply
    if (vw_wait_rx_max(200))
    {
        if (vw_get_float(value, &len, &source, &type) && type == VW_LIGHT_DATA_TYPE) // Non-blocking
        {
 	      // Message with a good checksum received, dump it.
	      Serial.print("Source: "); Serial.println((int)source);
	      Serial.print("value: ");
              int i;
	      for (i = 0; i < len; i++)
	      {
	          Serial.print(value[i]);
	      }
              Serial.println(" ");

        }
    }
}