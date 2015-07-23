#include <Servo.h>      // Servo
#include <Wire.h>       // I2C protocol for Compass

#include <motor.h>
#include <GP2Y0A21YK.h>        // IR sensor
#include <CMPS03.h>            // Compass
#include <LiquidCrystal_I2C.h> // LCD

void printTicks()
{
  int tickR = -1;
  int tickL = -1;
  
  tickR = get_TickRight();
  tickL = get_TickLeft();
  Serial.print("TickRight=");
  Serial.print(tickR);   
  Serial.print("\tTickLeft=");
  Serial.println(tickL);
}

void resetTicks()
{
  reset_TickRight();
  reset_TickLeft();
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
  if      (ret ==  DIRECTION_LEFT)  Serial.println("Left");  
  else if (ret ==  DIRECTION_RIGHT) Serial.println("Right");  
  else if (ret ==  OBSTACLE_LEFT)   Serial.println("Obstacle left"); 
  else if (ret ==  OBSTACLE_RIGHT)  Serial.println("Obstacle right");
  else if (ret ==  OBSTACLE)        Serial.println("Obstacle"); 
  else                              Serial.println("Error"); 
  
  delay(5000); //make it readable
  
  
  Serial.println(" --> start_forward"); 
  start_forward();
  delay(5000); //make it readable
  
  
  Serial.println(" --> turn +45 10s max");
  printDirection();
  ret =  turn(45, 10*1000);
  Serial.println(ret); 
  printDirection(); 
 
  
  Serial.println(" --> turn -45 10s max");
  printDirection(); 
  ret =  turn(-45, 10*1000);
  Serial.println(ret); 
  printDirection(); 

  Serial.println(" --> turn +45 30s max");
  printDirection(); 
  ret =  turn(45, 30*1000);
  Serial.println(ret); 
  printDirection(); 
  
  Serial.println(" --> turn -45 30s max");
  printDirection(); 
  ret =  turn(-45, 30*1000);
  Serial.println(ret); 
  printDirection(); 
}
