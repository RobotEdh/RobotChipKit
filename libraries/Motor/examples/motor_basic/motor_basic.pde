#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <TiltPan.h>    // Tilt&Pan


void setup()
{ 
  Serial.begin(9600); // initialize serial port
  motor_begin(); 
}


void loop()
{
  int ret;
  
  Serial.print(" --> start_forward motor 1\n"); 
  start_forward_test(1);
  delay(5000); //make it readable
  
  Serial.print(" --> start_forward motor 2\n"); 
  start_forward_test(2);
  delay(5000); //make it readable
  
  Serial.print(" --> start_forward motor 3\n"); 
  start_forward_test(3);
  delay(5000); //make it readable
  
  Serial.print(" --> start_forward motor 4\n"); 
  start_forward_test(4);
  delay(5000); //make it readable
  
  Serial.print(" --> start_forward\n"); 
  start_forward();
  delay(5000); //make it readable
  
  Serial.print(" \n--> accelerate x80 RIGHT_MOTOR, ret:"); 
  ret =  accelerate_n(RIGHT_MOTOR, 80);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" \n--> accelerate x80 LEFT_MOTOR, ret:"); 
  ret =  accelerate_n(LEFT_MOTOR, 80);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" -\n-> accelerate x20 BOTH_MOTOR, ret:"); 
  ret =  accelerate_n(BOTH_MOTOR, 20);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" \n--> deccelerate x30 RIGHT_MOTOR, ret:"); 
  ret =  deccelerate_n(RIGHT_MOTOR, 30);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" \n--> deccelerate x30 LEFT_MOTOR, ret:"); 
  ret =  deccelerate_n(LEFT_MOTOR, 30);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" \n--> deccelerate x40 BOTH_MOTOR, ret:"); 
  ret =  deccelerate_n(BOTH_MOTOR, 40);
  Serial.print(ret); 
  delay(5000); //make it readable
  
  Serial.print(" \n--> stop\n");  
  stop();
  delay(5000); //make it readable 
  
  Serial.print(" --> start_backward\n"); 
  start_backward();
  delay(5000); //make it readable   
  
  Serial.print(" --> stop\n");  
  stop();
  delay(5000); //make it readable 
  
  return;
}

