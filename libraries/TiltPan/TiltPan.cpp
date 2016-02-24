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
  delay(1500);         // waits 1500ms for the servo to reach the position 

  // initialize the PWM pin connected to the servo used for the Vertical Tilt&Pan and initialize the associate Timer interrupt
  VServo.attach(Vpin);   
  // reset the servo position
  VServo.write(90);    // reset servo position
  delay(1500);         // waits 1500ms for the servo to reach the position 

  TiltPan_move (40,160);
  TiltPan_move (40,70);
  TiltPan_move (140,70);
  TiltPan_move (140,160);
  TiltPan_move (90,90);


  return;      
}

void TiltPan_move(uint8_t HPos, uint8_t VPos)
{
    if (HPos > 180) HPos = 180; 
    if (HPos < 0) HPos = 0;
            
    if (VPos > 180) VPos = 180; 
    if (VPos < 0) VPos = 0; 
        
    // Vertical limits due to the Tilt Pan
    if ((HPos < 60)  && (VPos < 70)) VPos = 70;
    if ((HPos > 120) && (VPos < 70)) VPos = 70;     
           
    HServo.write(HPos);  // moves Horizontal servo to position HPos
    delay(1500);         // waits 1500ms for the servo to reach the position 
    
    VServo.write(VPos);  // moves Vertical servo to position VPos
    delay(1500);         // waits 1500ms for the servo to reach the position 
        
    return;      
}