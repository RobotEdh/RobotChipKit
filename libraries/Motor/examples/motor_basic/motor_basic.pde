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
  
  delay(1000); //wait end command 
  reset_TickRight();
  reset_TickLeft();
}

  
void setup()
{ 
  Serial.begin(9600); // initialize serial port
  motor_begin(); 
}


void loop()
{
  int ret;
  
  Serial.println(" --> stop");  
  stop();
  resetTicks();
  delay(5000); //make it readable 
  printTicks();   

  Serial.println(" --> start_forward motor 1"); 
  start_forward_test(1);
  resetTicks();
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> start_forward motor 2"); 
  start_forward_test(2);
  resetTicks();
  delay(5000); //make it readable
  printTicks();
   
  Serial.println(" --> start_forward motor 3"); 
  start_forward_test(3);
  resetTicks();
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> start_forward motor 4"); 
  start_forward_test(4);
  resetTicks();
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> stop");  
  stop();
  resetTicks();
  delay(5000); //make it readable 
  printTicks(); 
  
  Serial.println(" --> start_forward"); 
  start_forward();
  resetTicks();
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate +80 RIGHT_MOTOR, ret:"); 
  ret =  accelerate_n(RIGHT_MOTOR, 80);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate +80 LEFT_MOTOR, ret:"); 
  ret =  accelerate_n(LEFT_MOTOR, 80);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate +20 BOTH_MOTOR, ret:"); 
  ret =  accelerate_n(BOTH_MOTOR, 20);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate -30 RIGHT_MOTOR, ret:"); 
  ret =  deccelerate_n(RIGHT_MOTOR, 30);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate -50 LEFT_MOTOR, ret:"); 
  ret =  deccelerate_n(LEFT_MOTOR, 50);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate -40 BOTH_MOTOR, ret:"); 
  ret =  deccelerate_n(BOTH_MOTOR, 40);
  resetTicks();
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> stop");
  stop();
  resetTicks();
  delay(5000); //make it readable 
  printTicks();
  
  Serial.println(" --> start_backward"); 
  start_backward();
  resetTicks();
  delay(5000); //make it readable   
  printTicks();
  
  Serial.println(" --> stop");  
  stop();
  resetTicks();
  delay(5000); //make it readable 
  printTicks();
}

