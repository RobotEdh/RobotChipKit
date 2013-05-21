#include <Servo.h>      // Servo
#include <Wire.h>       // I2C protocol for Compass

#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compass

int tickRprev = 0;  // used to print delta tick
int tickLprev = 0;

void printTicks()
{
  int tickR = -1;
  int tickL = -1;
  
  tickR = get_TickRight();
  tickL = get_TickLeft();
  Serial.print("TickRight=");
  Serial.print(tickR-tickRprev);   // print delta tick right
  Serial.print("\tTickLeft=");
  Serial.println(tickL-tickLprev); // print delta tick left
  
  tickRprev = tickR;
  tickLprev = tickL;
}

void printDirection()
{
  CMPS03Class CMPS03;          // The Compass class
  int direction = -1;
  
  direction = CMPS03.CMPS03_read();
  Serial.print("Direction=");
  Serial.println(direction);
}
  
void setup()
{ 
  Serial.begin(9600); // initialize serial port
  motor_begin(); 
}


void loop()
{
  int ret;
  
  Serial.println(" --> check around"); 
  ret = check_around(); 
  Serial.print("ret: "); 
  Serial.println(ret);  
  delay(5000); //make it readable 
}

