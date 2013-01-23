#include <GP2Y0A21YK.h>
#include <wiring.h>


 
#include <XBee.h>

#define SUCESS 0
#define UNEXPECTED_APIID -1
#define STATUS_ERROR -2
#define RESPONSE_ERROR -3
#define NO_RESPONSE -4

#define PAYLOAD_SIZE 100

/* Analogic interface pin A0 is provided on pin V0 */
/* Power +3V is set on pin VCC        */
/* Ground    is set on pin GND        */



#define GP2Y0A21YK_Pin 14   // IR sensor GP2Y0A21YK analogic pin J7-01 A0 (C2IN-/AN2/SS1/CN4/RB2)   Use ADC module channel 2
/* Analogic interface is provided on pin V0 */
/* Power +3V is set on pin VCC              */
/* Ground    is set on pin GND              */







int xbee_send_data(char* msg) {

int ret = 0;
int count_max = 0;
int start = -1;
bool Endofdata = false;

char payload[PAYLOAD_SIZE];
Serial.print("xbee_send_data -");
while(!Endofdata) {
    if (strlen(msg) > PAYLOAD_SIZE - 1) 
    {
        count_max = PAYLOAD_SIZE - 1;
        payload[0] = 0;
    }
    else
    {
        count_max = strlen(msg)+1;
        payload[0] = 1;
        Endofdata= true;
    }                     
    for (int count=1;count<count_max;count++)
    {
        payload [count] = msg[count+start];
    }
    start = count_max;
    Serial.print(" - count_max:");
    Serial.print(count_max);
    Serial.print(" - payload:");
    Serial.print(payload);
    delay(5000);
    ret = xbee_send(payload, count_max);
    if (ret != SUCCESS) return ret;
  }
  
  return SUCCESS;
}

int xbee_send(char* msg, int msg_len) {


  uint8_t payload[PAYLOAD_SIZE];
  
  for (int count=0;count<msg_len;count++)
  {
          payload [count] = msg[count];
  }
  
  XBee xbee = XBee();
  xbee.begin(9600);
  
 

  // 64-bit addressing: This is the SH + SL address of remote XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x407be775);
  
  Tx64Request tx = Tx64Request(addr64, ACK_OPTION, payload, sizeof(payload),0x12); 
     

  TxStatusResponse txStatus = TxStatusResponse();
    
  xbee.send(tx);
  Serial.print("\nsendxbee2");
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!
        Serial.print("\ngot a response!");

        // should be a znet tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
    	   
           if (txStatus.getStatus() == SUCCESS) {
                // success.  time to celebrate
                Serial.print("\nsuccess");
                return SUCCESS;          	
           }
           else {
                // the remote XBee did not receive our packet. is it powered on?
                Serial.print("\nthe remote XBee did not receive our packet");
                return STATUS_ERROR;
           }         
      }
      else if (xbee.getResponse().isError()) {
            Serial.print("\nError reading packet.  Error code: "); 
            Serial.print(xbee.getResponse().getErrorCode());
            return RESPONSE_ERROR; 
      }
    }
    else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      Serial.print("\nNO_RESPONSE");
      return NO_RESPONSE; 
    }
}


void setup()
{
  
  Serial.begin(9600); // initialize serial port
  GP2Y0A21YK_init(GP2Y0A21YK_Pin); // initialize the pin connected to the sensor
 
}


void loop()
{
  int  distance;
  long startTime, stopTime, elapsedTime;
  int ret;
  
  Serial.print(" --> read distance from GP2Y0A21K"); 
  
  startTime = micros();
  distance = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
  stopTime = micros();
  elapsedTime = stopTime - startTime; // take 26 us
  
  if (distance == -1) {
    Serial.print("\nDistance outside range: < 15 cm or > 70 cm"); 
    Serial.print(" - elapsedTime: ");
    Serial.print(elapsedTime);   
  }
  else
  {  
    Serial.print("\nDistance in cm: ");
    Serial.print(distance); 
    Serial.print(" - startTime: ");
    Serial.print(startTime); 
    Serial.print(" - stopTime: ");
    Serial.print(stopTime); 
    Serial.print(" - elapsedTime: ");
    Serial.print(elapsedTime); 
    ret = xbee_send_data("Hello");
  }
  delay(500); //make it readable
  
  return;
}