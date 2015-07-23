#include <Servo.h> // Servo
#include <Wire.h> // I2C protocol for Compass

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
  
void setup()
{
  Serial.begin(9600); // initialize serial port
  motor_begin();
  resetTicks();
}


void loop()
{
  
  printTicks();
  delay(2000);
}  