#include <Serial2IOT.h>

//Create an instance of the IOT
IOTClass IOT; 


void setup()
{
  int ret = 0;
  
  Serial.begin(9600);
 
  IOT.begin();
  Serial.println ("IOT begin");
  
  ret = IOT.ping();
  Serial.print ("IOT ping, ret : "); Serial.println (ret);
  
}

  

void loop()
{

  

}




