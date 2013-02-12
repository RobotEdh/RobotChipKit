
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
Servo IRServo;               // The Servo class used for IR sensor
Servo HServo;                // The Servo class used for Horizontal Tilt & Pan
Servo VServo;                // The Servo class used for Vertical Tilt & Pan
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
   
  Serial.println("\nInit motors OK"); 
  
  // initialize the pin connected to the sensor 
  GP2Y0A21YK_init(GP2Y0A21YK_Pin); 
  Serial.println("\nInit IR sensor OK");

  // initialize the PWM pin connected to the servo used for the IR sensor and initialize the associate Timer interrupt
  IRServo.attach(IRSERVO_Pin);  
  // reset the servo position
  IRServo.write(90);  // reset servo position
  delay(15);          // waits 15ms for the servo to reach the position 
  // reset the Timer interrupt associate to the Timer interrupt
  IRServo.detach(); 
  
  // initialize the PWM pin connected to the servo used for the Horizontal Tilt&Pan and initialize the associate Timer interrupt
  HServo.attach(HSERVO_Pin);   
  // reset the servo position
  HServo.write(90);    // reset servo position
  delay(15);           // waits 15ms for the servo to reach the position 
  // reset the Timer interrupt associate to the Timer interrupt
  HServo.detach();
  
  // initialize the PWM pin connected to the servo used for the Vertical Tilt&Pan and initialize the associate Timer interrupt
  VServo.attach(VSERVO_Pin);   
  // reset the servo position
  VServo.write(90);    // reset servo position
  delay(15);           // waits 15ms for the servo to reach the position 
  // reset the Timer interrupt associate to the Timer interrupt
  VServo.detach(); 
  Serial.println("\nInit servos OK");   
 
  // Initialize the SD-Card
  if (!card.init(SPI_HALF_SPEED, SS_CS_Pin))  //Set SCK rate to F_CPU/4 (mode 1)
  {
  	Serial.println("\nError Init SD-Card");
  }
  else
  {	 
  	// initialize a FAT volume
  	if (!volume.init(&card))
  	{
  	  	Serial.println("\nError Init Volume in SD-Card");
	}  	  	
  	else
  	{ 
  		// Open volume
  		if (!root.openRoot(&volume))
  		{
  			Serial.println("\nError Open Volume in SD-Card");
		}  	  	
  		else
  		{   
    			Serial.print("\nSD-Card type is ");
    			switch(card.type()) {
    			case SD_CARD_TYPE_SD1:
      				Serial.print("SD1");
      			break;
    			case SD_CARD_TYPE_SD2:
      				Serial.print("SD2");
      			break;
    			case SD_CARD_TYPE_SDHC:
      				Serial.print("SDHC");
      			break;
    			default:
      				Serial.println("Unknown");
  			}

  			cid_t cid;
  			if (!card.readCID(&cid))
   			{
   				Serial.print("\nError Open read CID of SD-Card");
			}  	  	
  			else
  			{  
  				Serial.print("\nManufacturer ID: ");
  				Serial.print(cid.mid, HEX);

  				Serial.print("\nOEM/Application ID: ");
  				Serial.print(cid.oid[0]);
  				Serial.print(cid.oid[1]);
  
  				Serial.print("\nProduct name: ");
  				for (uint8_t i = 0; i < 5; i++) {
    					Serial.print(cid.pnm[i]);
  				}
 
  				Serial.print("\nProduct revision: ");
  				Serial.print(cid.prv_m, DEC);
  				Serial.print(".");
  				Serial.print(cid.prv_n, DEC);

  				Serial.print("\nProduct serial number: ");
  				Serial.print(cid.psn);
 
  				Serial.print("\nManufacturing date: ");
  				Serial.print(cid.mdt_month);
  				Serial.print('/');
  				Serial.print(2000 + (10*cid.mdt_year_high) + cid.mdt_year_low);

  				// print the type and size of the first FAT-type volume
				Serial.print("\nVolume type is FAT");
				Serial.print(volume.fatType(), DEC);
				  				
  				uint32_t volumesize, volume_free;
				volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
				volume_free = volume.blocksPerCluster();  
				Serial.print("\nNb blocks per cluster: ");
				Serial.print(volumesize);
				volumesize *= volume.clusterCount();       // we'll have a lot of clusters
			    volume_free *= volume.freeClusterCount();
				Serial.print("\nClusters count: ");
				Serial.print(volume.clusterCount());  
				volumesize *= 512;
				volume_free *= 512;
				Serial.print("\nBlock size: 512");        // SD card blocks are always 512 bytes
				Serial.print("\nVolume size (bytes): ");
				Serial.print(volumesize);
				Serial.print(" / Volume free (bytes): ");
				Serial.print(volume_free);	
				Serial.print(" / % free: ");
				Serial.print(100.0*(double)(volume_free)/(double)(volumesize));			
				Serial.print("\nVolume size (Kbytes): ");
				volumesize /= 1024;
				Serial.print(volumesize);
				Serial.print(" / Volume free (Kbytes): ");
				volume_free /= 1024;
				Serial.print(volume_free);				
				Serial.print("\nVolume size (Mbytes): ");
				volumesize /= 1024;
				Serial.print(volumesize);
				Serial.print(" / Volume free (Mbytes): ");
				volume_free /= 1024;
				Serial.print(volume_free);
				
				// list all files in the card with date and size			    
			    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
				root.ls(LS_R | LS_DATE | LS_SIZE);
			}				
		}
	}
  }			
  // initialize the compas  
  CMPS03.CMPS03_begin();
  Serial.println("\nInit compas OK");

  // Initialize the camera
  ret=JPEGCamera.begin();
  if (ret != SUCCESS)
  {  
        Serial.println("\nError Init Camera");
  }  	  	
  else
  {
        Serial.println("\nInit Camera OK");
  } 
      
  // interrupts setup
  attachInterrupt(EncodeurTickRightINT, IntrTickRight, FALLING);  //set right tick interrupt
  attachInterrupt(EncodeurTickLeftINT, IntrTickLeft, FALLING);    //set left tick interrupt
  
  interrupts();                          // enable all interrupts
  	
  Serial.println("\nEnd Init");
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
   
    // initialize the PWM pin connected to the servo used for the IR sensor and initialize the associate Timer interrupt (if not already done)
    IRServo.attach(IRSERVO_Pin);  
       
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
  while (millis() - start < timeout) {  // turn during maximum timeout milliseconds   
        direction = CMPS03.CMPS03_read(); // get current direction
 
        if ( (direction < 0)  || ((alpha > 0) && (direction > direction_target)) || ((alpha < 0) && (direction < direction_target)) ) {
                 if (alpha > 0) {
  	                   deccelerate_n(LEFT_MOTOR, delta1); // stop turns right
  	                   accelerate_n(RIGHT_MOTOR, delta2);  
                 }
                 else
                 {
    	               deccelerate_n(RIGHT_MOTOR, delta1); // stop turns left
  	                   accelerate_n(LEFT_MOTOR, delta2);  
                 }
                 if (direction < 0)  return COMPASS_ERROR;
                 else                return SUCCESS;
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
  if (!FilePicture.open(&root, filename, O_CREAT|O_WRITE|O_TRUNC)) return FILE_OPEN_ERROR;  
  
  //Take a picture
  ret=JPEGCamera.takePicture();
  if (ret != SUCCESS) return CAMERA_ERROR;
     
  //Get the size of the picture    
  ret=JPEGCamera.getSize(&size);
  if (ret != SUCCESS ) return CAMERA_ERROR;
 
  //Starting at address 0, keep reading data until we've read 'size' data
  while(address < size)
  {       
        //Read the data starting at the current address
        ret=JPEGCamera.readData(address, buf, &count, &eof);
        if (ret != SUCCESS ) return CAMERA_ERROR;
 
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
  if (ret != SUCCESS ) return CAMERA_ERROR;
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}

  

int sendPicture (int n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  char buf[PAYLOAD_SIZE-1];     //First byte is used as indicator
  uint8_t buffer[PAYLOAD_SIZE];
  char filename[12+1];
  
  
  // Open the file
  sprintf(filename, "PICT%02d.jpg", n);
  if (!FilePicture.open(&root, filename, O_READ)) return FILE_OPEN_ERROR;  

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
    
       unsigned int idx = 0;
       for (unsigned int i = 1;i<nbytes+1;i++)
       {
           buffer [i] = buf[idx++];
       }
   	
       ret = xBT.xBTsendXbee(buffer, nbytes+1);
       if (ret != SUCCESS ) return XBEE_ERROR;
 
  }// while
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}



void move_Tilt_Pan(uint8_t HPos, uint8_t VPos)
{
 
    // initialize the PWM pin connected to the servo used for the Tilt&Pan and initialize the associate Timer interrupt (if not already done)
    HServo.attach(HSERVO_Pin);  
    VServo.attach(VSERVO_Pin);  

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



int mainRobot ()
{
 int ret = SUCCESS;
 int state = STATE_STOP;
 long nb_go = 0;
 long nb_obstacle = 0;

 uint8_t cmd[PAYLOAD_SIZE];
 uint8_t resp[RESP_SIZE];

     
     ret = xBT.xBTreceiveXbee(cmd, 5000); // read 5 ms max
   
     if (ret == SUCCESS)
     {
           Serial.print(ret);
           if (cmd[0] == CMD_STOP)
           { 
               Serial.print("CMD_STOP"); 
               stop();
               state = STATE_STOP; 
           }
           else if (cmd[0] == CMD_START)
           { 
               Serial.print("CMD_START"); 
               start_forward();
               state = STATE_GO;
           }
           else if (cmd[0] == CMD_CHECK_AROUND)
           { 
               Serial.print("CMD_CHECK_AROUND"); 
               ret = check_around();
               // byte 0: response code
               resp[0] = RESP_CHECK_AROUND;
               // byte 1: direction
               resp[1] = ret;
               delay (3000);
               ret = xBT.xBTsendXbee(resp, sizeof (resp));
               if (ret != SUCCESS){  Serial.print("RESP_CHECK_AROUND error"); Serial.print(ret);}
           }
           else if (cmd[0] == CMD_MOVE_TILT_PAN)
           { 
               Serial.print("CMD_MOVE_TILT_PAN"); 
               move_Tilt_Pan(cmd[1], cmd[2]);
           }                      
           else if (cmd[0] == CMD_TURN_RIGHT)
           { 
               if (state == STATE_GO)
               { 
                     Serial.print("CMD_TURN_RIGHT");
                     ret = turn ((double)cmd[1], 100);
                     if (ret != SUCCESS){  Serial.print("CMD_TURN_RIGHT error"); Serial.print(ret);}              }       
           }
           else if (cmd[0] == CMD_TURN_LEFT)
           { 
               if (state == STATE_GO)
               { 
                     Serial.print("CMD_TURN_LEFT");
                     ret = turn (-(double)cmd[1], 100);
                     if (ret != SUCCESS){  Serial.print("CMD_TURN_LEFT error"); Serial.print(ret);}
               }       
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
               if (ret != SUCCESS){  Serial.print("RESP_INFOS error"); Serial.print(ret);}
           } 
           else if (cmd[0] == CMD_PICTURE) 
           { 
               Serial.print("CMD_PICTURE");
               no_picture++;
               ret = makePicture (no_picture);

               if (ret == SUCCESS)
               { 
                   ret= sendPicture (no_picture);
                   if (ret != SUCCESS){  Serial.print("sendPicture error"); Serial.print(ret);}
               }
               else
               {
                  Serial.print("makePicture error");
                  Serial.print(ret);
               }                 
           }
     }          
     
     if (state == STATE_GO)
     {    
           nb_go++;
/*         ret = go(10,0);
           if (ret == OBSTACLE)
           {   
               nb_obstacle++;
               ret = check_around ();
               switch (ret) {
                     case LEFT_DIRECTION:
                            ret = turn (-90,100);
                            break;
                     case RIGHT_DIRECTION:
                            ret = turn (+90,100);
                            break;
                      default:
                            stop();
                            state = STATE_STOP;  
               }
           }               
*/    } 
}