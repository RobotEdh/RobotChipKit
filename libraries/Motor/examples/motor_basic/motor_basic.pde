#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <TiltPan.h>    // Tilt&Pan



void printTicks()
{
  int tickR=-1;
  int tickL=-1;
  
  tickR = get_TickRight();
  tickL = get_TickLeft();
  Serial.print("TickRight=");
  Serial.print(tickR);
  Serial.print("\tTickLeft=");
  Serial.println(tickL);
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
  
  Serial.println(" --> start_forward motor 1"); 
  start_forward_test(1);
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> start_forward motor 2"); 
  start_forward_test(2);
  delay(5000); //make it readable
  printTicks();
   
  Serial.println(" --> start_forward motor 3"); 
  start_forward_test(3);
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> start_forward motor 4"); 
  start_forward_test(4);
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> start_forward"); 
  start_forward();
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate x80 RIGHT_MOTOR, ret:"); 
  ret =  accelerate_n(RIGHT_MOTOR, 80);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate x80 LEFT_MOTOR, ret:"); 
  ret =  accelerate_n(LEFT_MOTOR, 80);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> accelerate x20 BOTH_MOTOR, ret:"); 
  ret =  accelerate_n(BOTH_MOTOR, 20);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate x30 RIGHT_MOTOR, ret:"); 
  ret =  deccelerate_n(RIGHT_MOTOR, 30);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate x30 LEFT_MOTOR, ret:"); 
  ret =  deccelerate_n(LEFT_MOTOR, 30);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.print(" --> deccelerate x40 BOTH_MOTOR, ret:"); 
  ret =  deccelerate_n(BOTH_MOTOR, 40);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> stop");  
  stop();
  delay(5000); //make it readable 
  printTicks();
  
  Serial.println(" --> start_backward"); 
  start_backward();
  delay(5000); //make it readable   
  printTicks();
  
  Serial.println(" --> stop");  
  stop();
  delay(5000); //make it readable 
  printTicks();
  
  Serial.println(" --> start_forward"); 
  start_forward();
  delay(5000); //make it readable
  printTicks();
  
  Serial.println(" --> turn +45°, 10s max");
  printDirection(); 
  ret =  turn(45, 10*1000);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks(); 
  
  Serial.println(" --> turn -45°, 10s max");
  printDirection(); 
  ret =  turn(-45, 10*1000);
  Serial.println(ret); 
  delay(5000); //make it readable
  printTicks();  

  Serial.println(" --> stop");  
  stop();
  delay(5000); //make it readable 
  printTicks();  
   
  return;
}

