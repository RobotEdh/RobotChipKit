#include <TiltPan.h>  
#include <Servo.h>      // Servo

Servo HServo;                // The Servo class used for Horizontal Tilt & Pan
Servo VServo;                // The Servo class used for Vertical Tilt & Pan

void TiltPan_begin(int Hpin, int Vpin)
{
  // initialize the PWM pin connected to the servo used for the Horizontal Tilt&Pan and initialize the associate Timer interrupt
  HServo.attach(Hpin);   
  // reset the servo position
  HServo.write(90);    // reset servo position
  delay(15);           // waits 15ms for the servo to reach the position 

  // initialize the PWM pin connected to the servo used for the Vertical Tilt&Pan and initialize the associate Timer interrupt
  VServo.attach(Vpin);   
  // reset the servo position
  VServo.write(90);    // reset servo position
  delay(15);           // waits 15ms for the servo to reach the position 


  return;      
}

void TiltPan_move(uint8_t HPos, uint8_t VPos)
{
    if (HPos > 180) HPos = 180; 
    if (HPos < 0) HPos = 0;
            
    if (VPos > 180) VPos = 180; 
    if (VPos < 45) VPos = 45; //45° minimum due to the TiltPan
           
    HServo.write(HPos);  // moves Horizontal servo to position HPos
    delay(15);           // waits 15ms for the servo to reach the position 
    
    VServo.write(VPos);  // moves Vertical servo to position VPos
    delay(15);           // waits 15ms for the servo to reach the position 
        
    return;      
}