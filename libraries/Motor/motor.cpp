#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo

int SpeedMotorRight = 0;      // Duty cycle PWM motor right between 0 and SPEEDMAX( 255)
int SpeedMotorLeft = 0;       // Duty cycle PWM motor left between 0 and SPEEDMAX (255)

// data updated during interrupts
volatile int TickRight = 0;   
volatile int DirRight = 0;
volatile int TickLeft = 0;
volatile int DirLeft = 0;

CMPS03Class CMPS03;          // The Compass class
Servo IRServo;               // The Servo class used for IR sensor


int motor_begin()
{
  // H bridge setup
  pinMode(InMotorRight1Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorRight1Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(InMotorRight2Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorRight2Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(InMotorLeft1Pin, OUTPUT);       // set the pin as output
  pinMode(EnableMotorLeft1Pin, OUTPUT);   // set the analogig pin as output for PWM
  pinMode(InMotorLeft2Pin, OUTPUT);       // set the pin as output
  pinMode(EnableMotorLeft2Pin, OUTPUT);   // set the analogig pin as output for PWM
  Serial.println("Init motors OK"); 
  
  // initialize the pin connected to the sensor 
  GP2Y0A21YK_init(GP2Y0A21YK_Pin); 
  Serial.println("Init IR sensor OK");

  // initialize the PWM pin connected to the servo used for the IR sensor and initialize the associate Timer interrupt
  IRServo.attach(IRSERVO_Pin);  
  // reset the servo position
  IRServo.write(90);  // reset servo position
  delay(15);          // waits 15ms for the servo to reach the position  
  Serial.println("Init IR servo OK");  
    
  // initialize the compas  
  CMPS03.CMPS03_begin();
  Serial.println("Init compas OK");
      
  // interrupts setup
  pinMode(EncoderTickRightPin, INPUT);      // set the pin as input
  pinMode(EncoderTickLeftPin, INPUT);       // set the pin as input
  attachInterrupt(EncoderTickRightINT, IntrTickRight, FALLING);  // set right tick interrupt
  attachInterrupt(EncoderTickLeftINT, IntrTickLeft, FALLING);    // set left tick interrupt
  
  interrupts(); // enable all interrupts
  	
  Serial.println("End Motor Init");
  Serial.println("");
  return SUCCESS;
  
}


void IntrTickRight()  // right tick interrupt
{
    TickRight++;
} 


void IntrTickLeft()  // left tick interrupt
{
    TickLeft++;
}


int get_TickRight()
{  
  return TickRight;  
}

int get_TickLeft()
{
  return TickLeft;  
}

void reset_TickRight()
{  
  TickRight = 0;  
}

void reset_TickLeft()
{
  TickLeft = 0;  
}

int get_SpeedMotorRight()
{  
  return SpeedMotorRight;  
}

int get_SpeedMotorLeft()
{
  return SpeedMotorLeft;  
}


void forward()
{
     
  digitalWrite(InMotorRight1Pin, HIGH); 
  digitalWrite(InMotorRight2Pin, HIGH); 
  digitalWrite(InMotorLeft1Pin,  HIGH); 
  digitalWrite(InMotorLeft2Pin,  HIGH); 
   
  return;  
}

void forward_test(int num) // for test only
{
  if (num == 1) {
        digitalWrite(InMotorRight1Pin, HIGH); 
  }
  if (num == 2) {
        digitalWrite(InMotorRight2Pin, HIGH); 
  }
  if (num == 3) {
        digitalWrite(InMotorLeft1Pin,  HIGH); 
  }      
  if (num == 4) {
        digitalWrite(InMotorLeft2Pin,  HIGH); 
  } 
  return;  
}

void backward()
{
  
  digitalWrite(InMotorRight1Pin, LOW); 
  digitalWrite(InMotorRight2Pin, LOW);   
  digitalWrite(InMotorLeft1Pin,  LOW); 
  digitalWrite(InMotorLeft2Pin,  LOW); 
   
  return;   
}

void start_forward()
{
     
  forward();
  
  SpeedMotorRight = SPEEDNOMINAL;
  SpeedMotorLeft  = SPEEDNOMINAL;
  
  analogWrite(EnableMotorRight1Pin, SpeedMotorRight);
  analogWrite(EnableMotorRight2Pin, SpeedMotorRight);    
  analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
  analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);
  
  return;  
}


void start_forward_test(int num) // for test only
{
     
  forward_test(num);
  
  if (num == 1) {
        analogWrite(EnableMotorRight1Pin, SPEEDNOMINAL);
  }      
  if (num == 2) {
        analogWrite(EnableMotorRight2Pin, SPEEDNOMINAL);    
  }
  if (num == 3) {
        analogWrite(EnableMotorLeft1Pin,  SPEEDNOMINAL);
  }
  if (num == 4) {
        analogWrite(EnableMotorLeft2Pin,  SPEEDNOMINAL);
  }
  return;  
}


void start_backward()
{
     
  backward();
  
  SpeedMotorRight = SPEEDNOMINAL;
  SpeedMotorLeft  = SPEEDNOMINAL;
  
  analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
  analogWrite(EnableMotorRight2Pin, SpeedMotorRight);  
  analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
  analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);  
  return;  
}

void stop()
{
     
  SpeedMotorRight = 0;
  SpeedMotorLeft  = 0;
  
  analogWrite(EnableMotorRight1Pin, SpeedMotorRight);
  analogWrite(EnableMotorRight2Pin, SpeedMotorRight);    
  analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
  analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);
      
  return;  
}


int accelerate (int motor)
{
 if (motor == LEFT_MOTOR) {
       if  (SpeedMotorLeft < SPEEDMAX)   SpeedMotorLeft++;
       else return SPEED_ERROR; 
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);       
 }
 else if (motor == RIGHT_MOTOR) {
       if  (SpeedMotorRight < SPEEDMAX)  SpeedMotorRight++;
       else return SPEED_ERROR; 
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);         
 }
 else {
       if  (SpeedMotorRight < SPEEDMAX)  SpeedMotorRight++;
       else return SPEED_ERROR; 
       if  (SpeedMotorLeft < SPEEDMAX)   SpeedMotorLeft++; 
       else return SPEED_ERROR;  
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight);
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);  
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);
       
 }
 return SUCCESS;
}


int deccelerate(int motor)
{
 if (motor == LEFT_MOTOR) {
       if  (SpeedMotorLeft > 0)   SpeedMotorLeft--;
       else return SPEED_ERROR; 
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);       
 }
 else if (motor == RIGHT_MOTOR) {
       if  (SpeedMotorRight > 0)  SpeedMotorRight--;
       else return SPEED_ERROR; 
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);          
 }
 else {
       if  (SpeedMotorRight > 0)  SpeedMotorRight--;
       else return SPEED_ERROR; 
       if  (SpeedMotorLeft >0)   SpeedMotorLeft--; 
       else return SPEED_ERROR;  
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);         
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);       
 }     
 return SUCCESS; 
}


int accelerate_n(int motor, int n)
{
 int ret = SUCCESS;
  
 for (int i=0;i<n;i++)
 {
   ret = accelerate(motor);
        
   if (ret == SPEED_ERROR) return i;  //stop if any error
 }     
 return n; 
}


int deccelerate_n(int motor, int n)
{
 int ret = SUCCESS;
  
 for (int i=0;i<n;i++)
 {
   ret = deccelerate(motor);
        
   if (ret == SPEED_ERROR) return i;  //stop if any error
 }     
 return n; 
}


int go(int d, int pid_ind)
{
 int ret = 0;
 int Tick = 0;
 int pid;
 int distance = 0;
 int direction = 0; /* direction between 0-254, 0: North */
 
 TickLeft = 0;  // reset ticks
 TickRight = 0;
 
 while (Tick < d) {
     
       Tick = TickLeft + TickRight;
       if (pid_ind == 1) {
             if (TickLeft > TickRight) {
                   pid = computePID (TickLeft - TickRight); // compute PID
                   ret = adjustMotor (LEFT_MOTOR, pid);     // Adjust according PID
                   if (ret == SPEED_ERROR) return SPEED_ERROR;
             }      
              if (TickLeft < TickRight) {
                   pid = computePID (TickRight - TickLeft);  // compute PID
                   ret = adjustMotor (RIGHT_MOTOR, pid);     // Adjust according PID
                   if (ret == SPEED_ERROR) return SPEED_ERROR;
             }
       } // end PID
       
       distance = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance minimum

       if ((distance > 0) && (distance < DISTANCE_MIN))
       {
           return OBSTACLE;       
       }
       
 }  // end while Tick < d
 
 return SUCCESS; 
}


int check_around()
{
    int distance_right = 0;
    int distance_left = 0; 
   
    IRServo.write(0);    // turn servo left
    delay(15*90);        // waits the servo to reach the position 
    distance_left = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance on right side
    if ((distance_left > 0) && (distance_left < DISTANCE_MIN)) distance_left = 0;  // Robot need a min distance to turn
       
    IRServo.write(180);  // turn servo right
    delay(15*180);       // waits the servo to reach the position 
    distance_right = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance on left side
    if ((distance_right > 0) && (distance_right < DISTANCE_MIN)) distance_right = 0;  // Robot need a min distance to turn
  
    IRServo.write(90);   // reset servo position
    delay(15*90);        // waits the servo to reach the position 
	 
    if (distance_left == -1) 
    {
         return LEFT_DIRECTION;
    }     
    else if (distance_right == -1) 
    {
         return RIGHT_DIRECTION;
    }    
    else if ((distance_left > 0) && (distance_left > distance_right)) {
         return LEFT_DIRECTION;     
    }
    else if (distance_right > 0) 
    {
         return RIGHT_DIRECTION; 
    }
    else
    {
         return OBSTACLE; 
    }      
}


int adjustMotor (int motor, int pid)
{
  if (motor == LEFT_MOTOR) {
       if  ((SpeedMotorLeft - pid) > SPEEDNOMINAL){
             SpeedMotorLeft = SpeedMotorLeft - pid;
       }
       else if (SpeedMotorLeft > SPEEDNOMINAL){     
             SpeedMotorLeft = SPEEDNOMINAL;
             SpeedMotorRight = SpeedMotorRight + pid - SPEEDNOMINAL;            
       } 
       else {     
             SpeedMotorRight = SpeedMotorRight + pid;            
       }      
  }
  else
  {
       if  ((SpeedMotorRight - pid) > SPEEDNOMINAL){
             SpeedMotorRight = SpeedMotorRight - pid;
       }
       else if (SpeedMotorRight > SPEEDNOMINAL){     
             SpeedMotorRight = SPEEDNOMINAL;
             SpeedMotorLeft = SpeedMotorLeft + pid - SPEEDNOMINAL;         
       } 
       else {     
             SpeedMotorLeft = SpeedMotorLeft + pid;            
       }      
  } 
  
  // check speed max
  
  if (SpeedMotorRight - SPEEDMAX > SpeedMotorLeft) return SPEED_ERROR;
  if (SpeedMotorRight - SPEEDMAX > 0) {
       SpeedMotorRight = SPEEDMAX;
       SpeedMotorLeft = SpeedMotorLeft - (SPEEDMAX - SpeedMotorRight) ;
  } 
  
  if (SpeedMotorLeft - SPEEDMAX > SpeedMotorRight) return SPEED_ERROR;
  if (SpeedMotorLeft - SPEEDMAX > 0) {
       SpeedMotorLeft = SPEEDMAX;
       SpeedMotorRight = SpeedMotorRight - (SPEEDMAX - SpeedMotorLeft) ;
  }
   
  analogWrite(EnableMotorRight1Pin,  SpeedMotorRight); 
  analogWrite(EnableMotorRight2Pin,  SpeedMotorRight); 
  analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
  analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);
  
  return SUCCESS;
   
}

 
int turn(double alpha, unsigned long timeout)
{
  int direction = 0; /* direction between 0-254, 0: North */
  int direction_target = 0; /* direction between 0-254, 0: North */
  int delta1 = 0;
  int delta2 = 0;
  int end_turn =0;
  
  if ((alpha == 0) || (alpha < -180) || (alpha > 180)) return BAD_ANGLE; // alpha between -180 and +180 and <> 0
  
  direction = CMPS03.CMPS03_read(); // get initial direction
  if (direction < 0)  return COMPASS_ERROR;
  
  direction_target = direction + (int)(254.0*alpha/360.0); // compute target direction 
  
  if (alpha > 0 ) {
        delta1 = accelerate_n(LEFT_MOTOR, SPEEDDELTA); // turns right
  	    delta2 = deccelerate_n(RIGHT_MOTOR, SPEEDDELTA + SPEEDDELTA - delta1);  
  }
  else
  {
        delta1 = accelerate_n(RIGHT_MOTOR, SPEEDDELTA); // turns left
  	    delta2 = deccelerate_n(LEFT_MOTOR, SPEEDDELTA + SPEEDDELTA - delta1);  
  }
  
  unsigned long start = millis();
  while ((millis() - start < timeout) || end_turn == 1) {  // turn during maximum timeout milliseconds   
        direction = CMPS03.CMPS03_read(); // get current direction
      
        if ( ((alpha > 0) && (direction > direction_target)) || ((alpha < 0) && (direction < direction_target)) ) end_turn = 1;
   
        if ( (direction < 0)  || ((alpha > 0) && (direction > direction_target)) || ((alpha < 0) && (direction < direction_target)) ) {


        }
  } 
  
  if (alpha > 0)
  {
          deccelerate_n(LEFT_MOTOR, delta1); // stop turns right
          accelerate_n(RIGHT_MOTOR, delta2);  
  }
  else
  {
          deccelerate_n(RIGHT_MOTOR, delta1); // stop turns left
          accelerate_n(LEFT_MOTOR, delta2);  
  }
   
  if(end_turn == 1)        return SUCCESS;
  else if (direction < 0)  return COMPASS_ERROR;
  else                     return TIMEOUT; 
}
