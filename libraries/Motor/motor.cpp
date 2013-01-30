
#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <LSY201.h>     // Camera
#include <SD.h>         // SD-Card
#include <XBee.h>       // XBee
#include <xBeeTools.h>  //XBee tools

int SpeedMotorRight = 0;      // Duty cycle PWM motor right between 0 and SPEEDMAX( 255)
int SpeedMotorLeft = 0;       // Duty cycle PWM motor left between 0 and SPEEDMAX (255)

// data updated during interrupts
volatile int TickRight = 0;   
volatile int DirRight = 0;
volatile int TickLeft = 0;
volatile int DirLeft = 0;

CMPS03Class CMPS03;          // The Compass class
Servo IRServo;               // The Servo class
JPEGCameraClass JPEGCamera;  // The Camera class

Sd2Card card;       // SD Card       
SdVolume volume;    // SD Volume
SdFile root;        // SD Root
SdFile FilePicture; // SD File

int no_picture = 0;
 
xBeeTools xBT;           // The Xbee tools class


int motor_init()
{
  int ret = SUCCESS;
   
  // H bridge setup
  pinMode(In1MotorRight1Pin, OUTPUT);     // set the pin as output
  pinMode(In2MotorRight1Pin, OUTPUT);     // set the pin as output
  pinMode(EnableMotorRight1Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(In1MotorRight2Pin, OUTPUT);     // set the pin as output
  pinMode(In2MotorRight2Pin, OUTPUT);     // set the pin as output
  pinMode(EnableMotorRight2Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(In1MotorLeft1Pin, OUTPUT);      // set the pin as output
  pinMode(In2MotorLeft1Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorLeft1Pin, OUTPUT);   // set the analogig pin as output for PWM
  pinMode(In1MotorLeft2Pin, OUTPUT);      // set the pin as output
  pinMode(In2MotorLeft2Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorLeft2Pin, OUTPUT);   // set the analogig pin as output for PWM
    
  // initialize the pin connected to the sensor 
  GP2Y0A21YK_init(GP2Y0A21YK_Pin); 

  // initialize the PWM pin connected to the servo used for the IR sensor and initialize the associate Timer interrupt
  IRServo.attach(SERVO_Pin);  
 
  // reset the servo position
  IRServo.write(90);  // reset servo position
  delay(15);          // waits 15ms for the servo to reach the position 
 
  // reset the Timer interrupt associate to the Timer interrupt
  IRServo.detach(); 
  
  // initialize the compas  
  CMPS03.CMPS03_begin();
  
  // Initialize the camera
  ret=JPEGCamera.begin();
  if (ret != SUCCESS) return ret;
    
  // Initialize the SD-Card
  if (!card.init(SPI_HALF_SPEED, SS_CS_Pin)) return -1; //Set SCK rate to F_CPU/4 (mode 1)
  // initialize a FAT volume
  if (!volume.init(&card)) return -2; 
  // Open volume
  if (!root.openRoot(&volume)) return -3; 

  // interrupts setup
  attachInterrupt(EncodeurTickRightINT, IntrTickRight, FALLING);  //set right tick interrupt
  attachInterrupt(EncodeurTickLeftINT, IntrTickLeft, FALLING);    //set left tick interrupt
  
  interrupts();                          // enable all interrupts
  
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



void forward()
{
     
  digitalWrite(In1MotorRight1Pin, HIGH); 
  digitalWrite(In2MotorRight1Pin, LOW);
  digitalWrite(In1MotorRight2Pin, HIGH); 
  digitalWrite(In2MotorRight2Pin, LOW);  
  digitalWrite(In1MotorLeft1Pin,  HIGH); 
  digitalWrite(In2MotorLeft1Pin,  LOW); 
  digitalWrite(In1MotorLeft2Pin,  HIGH); 
  digitalWrite(In2MotorLeft2Pin,  LOW); 
   
  return;  
}

void forward_test(int num) // for test only
{
  digitalWrite(In1MotorRight1Pin, LOW); 
  digitalWrite(In2MotorRight1Pin, LOW);
  digitalWrite(In1MotorRight2Pin, LOW); 
  digitalWrite(In2MotorRight2Pin, LOW);  
  digitalWrite(In1MotorLeft1Pin,  LOW); 
  digitalWrite(In2MotorLeft1Pin,  LOW); 
  digitalWrite(In1MotorLeft2Pin,  LOW); 
  digitalWrite(In2MotorLeft2Pin,  LOW);   
  if (num == 1) {
        digitalWrite(In1MotorRight1Pin, HIGH); 
  }
  if (num == 2) {
        digitalWrite(In1MotorRight2Pin, HIGH); 
  }
  if (num == 3) {
        digitalWrite(In1MotorLeft1Pin,  HIGH); 
  }      
  if (num == 4) {
        digitalWrite(In1MotorLeft2Pin,  HIGH); 
  } 
  return;  
}

void backward()
{
  
  digitalWrite(In1MotorRight1Pin, LOW); 
  digitalWrite(In2MotorRight1Pin, HIGH); 
  digitalWrite(In1MotorRight2Pin, LOW); 
  digitalWrite(In2MotorRight2Pin, HIGH);   
  digitalWrite(In1MotorLeft1Pin,  LOW); 
  digitalWrite(In2MotorLeft1Pin,  HIGH); 
  digitalWrite(In1MotorLeft2Pin,  LOW); 
  digitalWrite(In2MotorLeft2Pin,  HIGH); 
 
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
    
  digitalWrite(In1MotorRight1Pin, LOW); 
  digitalWrite(In2MotorRight1Pin, LOW);
  digitalWrite(In1MotorRight2Pin, LOW); 
  digitalWrite(In2MotorRight2Pin, LOW);  
  digitalWrite(In1MotorLeft1Pin,  LOW); 
  digitalWrite(In2MotorLeft1Pin,  LOW); 
  digitalWrite(In1MotorLeft2Pin,  LOW); 
  digitalWrite(In2MotorLeft2Pin,  LOW);  
  
  return;  
}


int accelerate (int motor)
{
 if (motor == LEFT_MOTOR) {
       if  (SpeedMotorLeft < SPEEDMAX)   SpeedMotorLeft++;
       else return -1; 
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);       
 }
 else if (motor == RIGHT_MOTOR) {
       if  (SpeedMotorRight < SPEEDMAX)  SpeedMotorRight++;
       else return -1; 
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);         
 }
 else {
       if  (SpeedMotorRight < SPEEDMAX)  SpeedMotorRight++;
       else return -1; 
       if  (SpeedMotorLeft < SPEEDMAX)   SpeedMotorLeft++; 
       else return -1;  
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
       else return -1; 
       analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
       analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);       
 }
 else if (motor == RIGHT_MOTOR) {
       if  (SpeedMotorRight > 0)  SpeedMotorRight--;
       else return -1; 
       analogWrite(EnableMotorRight1Pin, SpeedMotorRight); 
       analogWrite(EnableMotorRight2Pin, SpeedMotorRight);          
 }
 else {
       if  (SpeedMotorRight > 0)  SpeedMotorRight--;
       else return -1; 
       if  (SpeedMotorLeft >0)   SpeedMotorLeft--; 
       else return -1;  
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
        
   if (ret < 0) return i;  //stop if any error
 }     
 return n; 
}


int deccelerate_n(int motor, int n)
{
 int ret = SUCCESS;
  
 for (int i=0;i<n;i++)
 {
   ret = deccelerate(motor);
        
   if (ret < 0) return i;  //stop if any error
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
                   if (ret != 0) return ret;
             }      
              if (TickLeft < TickRight) {
                   pid = computePID (TickRight - TickLeft);  // compute PID
                   ret = adjustMotor (RIGHT_MOTOR, pid);     // Adjust according PID
                   if (ret != 0) return ret;
             }
       } // end PID
       
       distance = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance minimum

       if ((distance > 0) && (distance < DISTANCE_MIN))
       {
           return OBSTACLE;       
       }
       
 }  // end while Tick < d
 
 direction = CMPS03.CMPS03_read(); // get direction 
 return direction; 
}

int check_around()
{
 int distance_right = 0;
 int distance_left = 0; 
   
    // initialize the PWM pin connected to the servo used for the IR sensor and initialize the associate Timer interrupt
    IRServo.attach(SERVO_Pin);  
       
    IRServo.write(0);  // turn servo left
    delay(15*90);      // waits 15ms for the servo to reach the position 
    distance_left = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance on right side
    if ((distance_left > 0) && (distance_left < DISTANCE_MIN)) distance_left = 0;  // Robot need a min distance to turn
       
    IRServo.write(180);  // turn servo right
    delay(15*180);       // waits 15ms for the servo to reach the position 
    distance_right = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin); // Check distance on left side
    if ((distance_right > 0) && (distance_right < DISTANCE_MIN)) distance_right = 0;  // Robot need a min distance to turn
  
    IRServo.write(90);  // reset servo position
    delay(15*90);       // waits 15ms for the servo to reach the position 
	 
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
  
  if (SpeedMotorRight - SPEEDMAX > SpeedMotorLeft) return -1;
  if (SpeedMotorRight - SPEEDMAX > 0) {
       SpeedMotorRight = SPEEDMAX;
       SpeedMotorLeft = SpeedMotorLeft - (SPEEDMAX - SpeedMotorRight) ;
  } 
  
  if (SpeedMotorLeft - SPEEDMAX > SpeedMotorRight) return -1;
  if (SpeedMotorLeft - SPEEDMAX > 0) {
       SpeedMotorLeft = SPEEDMAX;
       SpeedMotorRight = SpeedMotorRight - (SPEEDMAX - SpeedMotorLeft) ;
  }
   
  analogWrite(EnableMotorRight1Pin,  SpeedMotorRight); 
  analogWrite(EnableMotorRight2Pin,  SpeedMotorRight); 
  analogWrite(EnableMotorLeft1Pin,  SpeedMotorLeft);
  analogWrite(EnableMotorLeft2Pin,  SpeedMotorLeft);
  
  return 0;
   
}

 
int turn(double alpha)
{
  int direction = 0; /* direction between 0-254, 0: North */
  int direction_target = 0; /* direction between 0-254, 0: North */
  int delta1 = 0;
  int delta2 = 0;
  
  if ((alpha == 0) || (alpha < -180) || (alpha > 180)) return -1; // alpha between -180 and +180 and <> 0
  
  direction = CMPS03.CMPS03_read(); // get initial direction 
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
  
  for (int i=0;i<1000;i++)  // simulate a timeout
  {      
        direction = CMPS03.CMPS03_read(); // get current direction 
         Serial.print(i);  
         Serial.print("/");
         Serial.print(direction); 
         Serial.print("/");
         Serial.print(direction_target); 
         Serial.print("\n");
        if ( ((alpha > 0) && (direction > direction_target)) || ((alpha < 0) && (direction < direction_target)) ) {
                 if (alpha > 0) {
  	                   deccelerate_n(LEFT_MOTOR, delta1); // stop turns right
  	                   accelerate_n(RIGHT_MOTOR, delta2);  
                 }
                 else
                 {
    	               deccelerate_n(RIGHT_MOTOR, delta1); // stop turns left
  	                   accelerate_n(LEFT_MOTOR, delta2);  
                 }
                 return SUCCESS;
        }
  } 
      
  return TIMEOUT; // timeout
}

int makePicture (int n)
{
  int ret=SUCCESS;
  uint8_t buf[32];     //Create a character array to store the cameras data
  int size=0;          //Size of the jpeg image
  long int address=0;  //This will keep track of the data address being read from the camera
  int eof=0;           //eof is a flag for the sketch to determine when the end of a file is detected 
                       //while reading the file data from the camera.
  int count=0;         // nb bytes read
  char filename[12+1];
  
  
  // Open the file
  sprintf(filename, "PICT%02d.jpg", n);
  if (!FilePicture.open(&root, filename, O_CREAT|O_WRITE|O_TRUNC)) return -1000;  
  
  //Take a picture
  ret=JPEGCamera.takePicture();
  if (ret != SUCCESS) return ret;
     
  //Get the size of the picture    
  ret=JPEGCamera.getSize(&size);
  if (ret != SUCCESS ) return ret;
 
  //Starting at address 0, keep reading data until we've read 'size' data
  while(address < size)
  {       
        //Read the data starting at the current address
        ret=JPEGCamera.readData(address, buf, &count, &eof);
        if (ret != SUCCESS ) return ret;
 
        for(int i=0; i<count; i++)
        {  
                FilePicture.write(buf[i]);
        }          
        if(eof==1) break;   
  
        //Increment the current address by the number of bytes we read
        address+=count;                     
  }// while

  //Stop taking picture
  ret=JPEGCamera.stopPictures();
  if (ret != SUCCESS ) return ret;
  
  //Close file
  if (!FilePicture.close()) return -2000;  
  
  return SUCCESS;
}

  

int sendPicture (int n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  unsigned int idx = 0;
  char buf[PAYLOAD_SIZE-1];     //First byte is used as indicator
  uint8_t buffer[PAYLOAD_SIZE];
  char filename[12+1];
  
  
  // Open the file
  sprintf(filename, "PICT%02d.jpg", n);
  if (!FilePicture.open(&root, filename, O_READ)) return -1000;  

  // read from the file until there's nothing else in it:
  while ((nbytes = FilePicture.read(buf, sizeof(buf))) > 0 && ret == SUCCESS) {
      
       if (nbytes == sizeof(buf)) 
       {
           buffer[0] = 0;
       }
       else
       {
           buffer[0] = 1; //end file read
       }  
    
       for (unsigned int i = 1;i<nbytes+1;i++)
       {
           buffer [i] = buf[idx++];
       }
   	
       ret = xBT.xBTsendXbee(buffer, nbytes+1);
 
  }// while
  
  //Close file
  if (!FilePicture.close()) return -2000;  
  
  return SUCCESS;
}


int mainRobot ()
{
 int ret = SUCCESS;
 int state = STATE_STOP;
 long nb_go = 0;
 long nb_obstacle = 0;

 uint8_t cmd[PAYLOAD_SIZE];
 uint8_t resp[RESP_SIZE];
 
     
     ret = xBT.xBTreceiveXbee(cmd, 5000); // read 5 ms max
   
     if (ret > 0)
     {
           Serial.print(ret);
           if (cmd[0] == CMD_STOP)
           { 
               stop();
               state = STATE_STOP; 
               Serial.print("CMD_STOP"); 
           }
           else if (cmd[0] == CMD_START)
           { 
               start_forward();
               state = STATE_GO;
               Serial.print("CMD_START"); 
           }
           else if (cmd[0] == CMD_INFOS)
           { 
               Serial.print("CMD_INFOS");
               // byte 0: response code
               resp[0] = RESP_INFOS;
               // byte 1: state
               resp[1] = state;
               // byte 2: SpeedMotorRight
               resp[2] = SpeedMotorRight;
               // byte 3: SpeedMotorLeft
               resp[3] = SpeedMotorLeft;
               // byte 4: nb go
               resp[4] = nb_go;
               // byte 5: nb obstacle
               resp[5] = nb_obstacle;
               // byte 6: direction
               resp[6] = CMPS03.CMPS03_read();
               // byte 7: distance
               resp[7] = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
               delay (3000);
               ret = xBT.xBTsendXbee(resp, sizeof (resp));
               if (ret != SUCCESS){  Serial.print("CMD_INFOS error"); Serial.print(ret);}
           } 
           else if (cmd[0] == CMD_PICTURE) 
           { 
               Serial.print("CMD_PICTURE");
               no_picture++;
               ret = makePicture (no_picture);
                Serial.print("CMD_PICTURE, ret:");
               Serial.print(ret);
               if (ret == SUCCESS)
               { 
                   ret= sendPicture (no_picture);
                   Serial.print("sendPicture, ret:");
                   Serial.print(ret);
               }                 
           }
     }          
     
     if (state == STATE_GO)
     {    
           nb_go++;
           ret = go(10,0);
           if (ret == OBSTACLE)
           {   
               nb_obstacle++;
               ret = check_around ();
               switch (ret) {
                     case LEFT_DIRECTION:
                            ret = turn (-90);
                            break;
                     case RIGHT_DIRECTION:
                            ret = turn (+90);
                            break;
                      default:
                            stop();
                            state = STATE_STOP;  
               }
           }               
     } 
}