#include <VirtualWire.h> // RF transmission library

#include <wiring.h>

#include <TEMT6000.h>
 
void setup()
{ 
    Serial.begin(9600);	// Debugging only
   
    // Initialise the IO and ISR
    Serial.println("Setup VW");
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec

    Serial.println("Init TEMT6000");
    TEMT6000_init(TEMT6000_Pin); // initialize the pin connected to the sensor
}


void loop()
{
  int  value;
  int source = 1; //source = 1
  
  Serial.println("--> read light value of TEMT6000: "); 
 
  value = TEMT6000_getLight(TEMT6000_Pin);
 
  Serial.print("Source: "); Serial.println(source);
  Serial.print("value: ");
  Serial.println(value); 
 
  vw_send_float(value, 0, VW_LIGHT_DATA_TYPE, source);
   
  vw_wait_tx(); // Wait until the whole message is gone
 
  delay(5000); //make it readable

  return;
}

