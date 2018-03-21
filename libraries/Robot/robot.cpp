#include <robot.h>         
#include <motor.h>             // Motor
#include <GP2Y0A21YK.h>        // IR sensor
#include <CMPS03.h>            // Compas
#include <TMP102.h>            // Temperature
#include <TEMT6000.h>          // Brightness
#include <Servo.h>             // Servo
#include <TiltPan.h>           // Tilt&Pan
#include <LSY201.h>            // Camera
#include <LiquidCrystal_I2C.h> // LCD
#include <Motion.h>            // Motion
#include <Micro.h>             // Micro
#include <sdcard.h>            // SD Card
#include <SD.h> 
#include <WiFiRobot.h>
#include <Serial2IOT.h>

/* classes aleady defined in motor */
extern GP2Y0A21YKClass GP2Y0A21YK;  // The IR sensor class
extern LiquidCrystal_I2C lcd;       // The LCD class
extern CMPS03Class CMPS03;          // The Compass class

       TMP102Class TMP102;          // The Temperature class  
       TEMT6000Class TEMT6000;      // The Brightness class
       MotionClass Motion;          // The Motion class
       MicroClass Micro;            // The Micro class
       JPEGCameraClass JPEGCamera;  // The Camera class 
       WiFiRobotClass WiFiRobot;    // The WIFI class for the Robot
       RIOTClass IOT;               // The IOT 

// data updated during interrupts
volatile int IntMotion = 0;   
volatile int IntIOT = 0; 

int HPos = 90;
int VPos = 90;
int motor_state = STATE_STOP;
int alert_status = 0;
int no_picture = 0;                 // Picture number
int PI_activated = 0;
unsigned long previousTime = 0;
unsigned long previousTime2 = 0;
unsigned long PI_freqInfos = 0;
unsigned long freqCheck = 10*1000;
unsigned long GOtimeout = 10;
 
double tab_temperature[NB_TEMPERATURE] = {0};
double avg_temperature = 0;

int tab_lux[NB_LUX] = {0};
unsigned long avg_lux = 0;

 

void IntrIOT()  // IOT interrupt
{
    IntIOT = 1;
    digitalWrite(Led_Blue, HIGH);  // turn on led
}

void IntrMotion()  // Motion sensor interrupt
{
    IntMotion = 1;
    digitalWrite(Led_Red, HIGH);  // turn on led
}

void reset_leds()
{
  // turn off all leds
  digitalWrite(Led_Blue, LOW);  
  digitalWrite(Led_Red  , LOW);  
}

void blink(int led)
{
  // turn off all leds
  reset_leds();

  // blink the resquested led
  for (int i=0;i<5;i++){
        digitalWrite(led, HIGH);  // turn on led
        delay(500);
        digitalWrite(led, LOW);  // turn off led
        delay(500);  
  }          
}

void buzz(int buzzNb)
{  
  for (int i=0;i<buzzNb;i++){
      digitalWrite(buzzPin, HIGH);
      delay(1);
      digitalWrite(buzzPin, LOW); 
      delay(200);
   }            
}

int robot_begin()
{
  int ret = SUCCESS;
 
  ret = motor_begin(); 
  if (ret != SUCCESS) return ret;

  Serial.println("Begin Robot Init");
   
  pinMode(Led_Blue, OUTPUT);     // set the pin as output
  blink(Led_Blue);  
  pinMode(Led_Red, OUTPUT);      // set the pin as output
  blink(Led_Red);  
  Serial.println("Init Leds OK");
    
  // initialize the buzzer
  pinMode(buzzPin, OUTPUT); 
  buzz(3);       
  Serial.println("Init Buzzer OK");
   
  // initialize the Tilt&Pan servos  
  TiltPan_begin(HSERVO_Pin, VSERVO_Pin);
  Serial.println("Init Tilt&Pan servos OK");

  // initialize the camera
  Serial.println(" ");
  Serial.println("Begin Init Camera...");
  ret=JPEGCamera.begin();
  if (ret != SUCCESS)
  {  
        Serial.print("Error Init Camera, error: ");
        Serial.println(ret);
  }        
  else
  {
        Serial.println("Init Camera OK");
  }    
  
  // initialize the SD-Card    
  ret = initSDCard();
  if (ret != SUCCESS)
  {  
        Serial.print("Error Init SD-Card, error: ");
        Serial.println(ret);
  }                                                                    
  else
  {
        Serial.println("Init SD-Card OK");
  }
    
  // get infos from SD-Card  
  ret=infoSDCard();
  if (ret < 0)
  {  
        Serial.print("Error Infos SD-Card, error: ");
        Serial.println(ret);
  }
  else
  {
        no_picture = ret+1;
        Serial.print("no_picture starts at: ");
        Serial.println(no_picture);
  }     
    
  // initialize the brightness sensor   
  TEMT6000.TEMT6000_init(TEMT6000_Pin); // initialize the pin connected to the sensor
  Serial.println("Init Brightness sensor OK");
  
  // initialize the temperature sensor   
  TMP102.TMP102_init();
  Serial.println("Init Temperature sensor OK");
  
  // initialize the electret micro   
  //Micro.Micro_init(MICRO_Pin); // initialize the pin connected to the micro
  //Serial.println("Init Micro OK");

  // initialize the motion sensor, interrupt setting 
  pinMode(MOTION_PIN, INPUT);                        // set the pin as input
  attachInterrupt(MOTION_INT, IntrMotion, RISING);   // set Motion interrupt
  Serial.println("Init Motion sensor OK");

  // initialize the IOT, interrupt setting
  IOT.begin();
  pinMode(IOT_PIN, INPUT);                           // set the pin as input
  attachInterrupt(IOT_INT, IntrIOT, RISING);         // set IOT interrupt
 
  interrupts();                                      // enable all interrupts
  Serial.println ("Init IOT OK");

  lcd.setCursor(0,1); 
  lcd.print("End   Robot Init");
 
  Serial.println("");
  Serial.println("End Robot Init");
  Serial.println("**************");
  Serial.println("");
 
  return SUCCESS;
  
} 

int infos (uint16_t *resp, uint8_t *resplen)
{    
     // 0: motor_state
     resp[0] = motor_state;
     Serial.print("motor_state: ");Serial.println((int)resp[0]);
     
     // 1: SpeedMotorRight
     resp[1] = get_SpeedMotorRight();
     Serial.print("SpeedMotorRight: ");Serial.println((int)resp[1]);
     
     // 2: SpeedMotorLeft
     resp[2] = get_SpeedMotorLeft();
     Serial.print("SpeedMotorLeft: ");Serial.println((int)resp[2]);
     
     // 3: direction
     resp[3] = CMPS03.CMPS03_read();
     Serial.print("direction: ");Serial.println((int)resp[3]);
     
     // 4: distance
     int dist = GP2Y0A21YK.GP2Y0A21YK_getDistanceCentimeter();
     if (dist > 0) resp[4] = dist;
     else          resp[4] = 0;
     Serial.print("distance: ");Serial.println((int)resp[4]);
     
     // 5: temperature
     double temperature = TMP102.TMP102_read();
     resp[5] = (int16_t)(100.0 * temperature);
     Serial.print("temperature: ");Serial.println((int)resp[5]);
     
     // 6: brightness
     resp[6] = TEMT6000.TEMT6000_getLight();
     Serial.print("brightness: ");Serial.println((int)resp[6]);
     
     // 7: noise
     //resp[7] = Micro.Micro_getNoise();
     resp[7] = 0;
     Serial.print("noise: ");Serial.println((int)resp[7]);
     
     // 8: alert status
     resp[8] = alert_status;
     Serial.print("alert status: ");Serial.println((int)resp[8]);
     
     // 9: picture number
     resp[9] = no_picture;
     Serial.print("no_picture: ");Serial.println((int)resp[9]);

     *resplen = 9+1;
 
     return SUCCESS;
}     


int check ()
{
  
  int i = 0;
  
  // Check Motion
  if (IntMotion) {
       IntMotion = 0;  // reset interrupt for next time
       digitalWrite(Led_Red, LOW);  // turn on led
       Serial.print("Alert Motion trigerred: ");	
       return ALERT_MOTION;
  }
  // Check Noise
  //Serial.print("noise: "); Serial.print(noise);Serial.print(" -- THR_NOISE: "); Serial.println(THR_NOISE);
  //if (noise > THR_NOISE) return ALERT_NOISE;

   
  // Check Temperature Variation
  double temperature = TMP102.TMP102_read();
  Serial.print("temperature: "); Serial.print(temperature);Serial.print(" -- previous_temperature: "); Serial.print(tab_temperature[0]);Serial.print(" -- MAX_VAR_TEMPERATURE: "); Serial.println(MAX_VAR_TEMPERATURE);
  if (temperature != tab_temperature[0]) {
      if (MAX_VAR_TEMPERATURE < abs(avg_temperature - temperature) && tab_temperature[NB_TEMPERATURE-1] != 0) {
          return ALERT_TEMPERATURE;
      }    
 
      avg_temperature = 0;
      for (i=NB_TEMPERATURE;i<2;i--) { 
          tab_temperature[i-1] = tab_temperature[i-2];
          avg_temperature += tab_temperature[i-1];
      }
      tab_temperature[0]=temperature;
      avg_temperature = (avg_temperature+temperature)/NB_TEMPERATURE;
  }
         
  // Check Lux Variation
  int lux = TEMT6000.TEMT6000_getLight();
  Serial.print("lux: "); Serial.print(lux);Serial.print(" -- previous_lux: "); Serial.print(tab_lux[0]);Serial.print(" -- MAX_VAR_LUX: "); Serial.println(MAX_VAR_LUX);
  if (lux != tab_lux[0]) {
      if (MAX_VAR_LUX < abs(avg_lux - lux) && tab_lux[NB_LUX-1] != 0) {
          return ALERT_LUX;
      }    
 
      avg_lux = 0;
      for (i=NB_LUX;i<2;i--) { 
          tab_lux[i-1] = tab_lux[i-2];
          avg_lux += tab_lux[i-1];
      }
      tab_lux[0]=lux;
      avg_lux = (avg_lux+lux)/NB_LUX;
  }
      
  return NO_ALERT;              
}        

   

int robot_command (uint16_t *cmd, uint16_t *resp, uint8_t *resplen)
{    
 unsigned long start = 0;
 uint8_t infolen = 0;
 int checkdir;
 int motor_state_save;
 int error = -1;
 int ret = SUCCESS;
 
 lcd.clear();     // clear LCD
 reset_leds();    // turn off all leds
 digitalWrite(Led_Blue, HIGH);  // turn on led blue
 
 switch (cmd[0]) {
 
 case CMD_STOP:
     Serial.println("CMD_STOP");
     lcd.print("STOP"); 
     
     stop();
     motor_state = STATE_STOP;
     *resplen = 0;
     break; 
  
 case CMD_START:
     if (cmd[1] == 0)
     {  
           Serial.println("CMD_START");
           lcd.print("START"); 
           
           start_forward();
     }
     else
     {       
           Serial.print("CMD_START_TEST motor: "); Serial.println((int)cmd[1]);
           lcd.print("START motor: "); lcd.print((int)cmd[1]); 
           
           start_forward_test(cmd[1]);           
     }                      
     motor_state = STATE_GO;
     *resplen = 0;
     break; 
 
 case CMD_CHECK_AROUND:
     Serial.println("CMD_CHECK_AROUND");
     lcd.print("CHECK AROUND");
    
     checkdir = check_around();
     
     lcd.setCursor(0,1); 
     if      (checkdir == DIRECTION_LEFT)  lcd.print("LEFT");
     else if (checkdir == DIRECTION_RIGHT) lcd.print("RIGHT");
     else if (checkdir == OBSTACLE_LEFT)   lcd.print("OBSTACLE LEFT");
     else if (checkdir == OBSTACLE_RIGHT)  lcd.print("OBSTACLE RIGHT");
     else if (checkdir == OBSTACLE)        lcd.print("OBSTACLE");
     else                                  lcd.print("?");

     resp[0] = checkdir;
     *resplen = 0+1;
     break;
     
 case CMD_MOVE_TILT_PAN:    
     Serial.print("CMD_MOVE_TILT_PAN: "); Serial.print((int)cmd[1]);Serial.print((int)cmd[2]);Serial.print((int)cmd[3]);Serial.println((int)cmd[4]);    
     if (cmd[2] == 0) HPos = (int)cmd[1] + 90; else HPos = 90 - (int)cmd[1]; 
     if (cmd[4] == 0) VPos = (int)cmd[3] + 90; else VPos = 90 - (int)cmd[3]; 
     Serial.print("CMD_MOVE_TILT_PAN, HPos VPos: "); Serial.print(HPos);Serial.println(VPos);   
     lcd.print("MOVE TILT&PAN");
     lcd.setCursor(0,1); 
     lcd.print("X: ");
     lcd.print(HPos);
     lcd.print(" Y: ");
     lcd.print(VPos);
     
     TiltPan_move(HPos, VPos);
    
     *resplen = 0;
     break; 
                    
 case CMD_TURN:
       if (cmd[1] == 180)
     {       
           Serial.print("CMD_TURN_BACK");
           lcd.print("TURN BACK ");  
         
           ret = turnback (10);  // 10s max
           if (ret != SUCCESS){
           	  Serial.print("turnback error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print("turnback error: "); lcd.print(ret);
           	  error = 1;
           }
     }       
     else if (motor_state == STATE_GO)
     { 
           Serial.print("CMD_TURN, alpha: "); Serial.print((cmd[2] != 1) ? ('+'):('-')); Serial.println((int)cmd[1]); 
           lcd.print("TURN"); lcd.print((cmd[2] != 1) ? ('+'):('-')); lcd.print((int)cmd[1]);lcd.print((char)223); //degree   
         
           ret = turn ((double)((cmd[2] != 1) ? (cmd[1]):(-cmd[1])), 5);  // 5s max        
           if (ret != SUCCESS){
           	  Serial.print("turn error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print(" turn error: "); lcd.print(ret);
           	  error = 1;
           }           
    }
    
    *resplen = 0;
    break;        
     
     
 case CMD_INFOS: 
     Serial.println("CMD_INFOS");

     ret = infos (resp, &infolen);
       
     if (resp[0] == STATE_GO) {
         lcd.print((int)resp[1]);lcd.print((char)126);lcd.print((byte)lcd_pipe);lcd.print((int)resp[2]);lcd.print((char)127);
     }    
     else
     {
         lcd.print("STOPPED");
     }
     lcd.setCursor(0,1);   
     lcd.print((int)resp[7]); lcd.print((byte)lcd_celcius);lcd.print((byte)lcd_pipe);   
     lcd.print((int)resp[6]); lcd.print("cm");lcd.print((byte)lcd_pipe);
     lcd.print((int)resp[5]); lcd.print((char)223); //degree    
 
     *resplen = infolen;
     break;
      
 case CMD_PICTURE: 
     Serial.print("CMD_PICTURE, no_picture: ");
     no_picture++;
     Serial.println(no_picture);
     lcd.print("PICTURE ");
     
     motor_state_save = motor_state;
     if (motor_state == STATE_GO) {
        Serial.println("Stop"); 
        stop();
        motor_state = STATE_STOP;
      }
   
     ret = JPEGCamera.makePicture (no_picture);
     if (ret == SUCCESS)
     { 
       lcd.setCursor(0,1);
        lcd.print("picture: "); lcd.print(no_picture);
     }
     else
     {
        Serial.print("makePicture error: "); Serial.println(ret);
        lcd.setCursor(0,1); 
        lcd.print("error: "); lcd.print(ret);       
        error = 1;
     }
       
     if (motor_state_save == STATE_GO) {          
        Serial.println("Start");
        start_forward();                     
        motor_state = STATE_GO;
     }
        
     // byte 0: picture number
     resp[0] = no_picture;
     *resplen = 0+1;    
     break;

 case CMD_ALERT: 
     Serial.println("CMD_ALERT");
     lcd.print("Alert"); 
    
     blink(Led_Blue);  
     buzz(5); 
     
     // If motor_state == STATE_GO => Stop           
     if (motor_state == STATE_GO) {
        Serial.println("Stop"); 
        stop();
        motor_state = STATE_STOP;
     }
   
     // Make 3 pictures left, front and right
     if ((HPos != 90) || (VPos !=90))
     { 
        HPos = 90;
        VPos = 90;
        TiltPan_move(HPos, VPos);
     }
     
     Serial.print("makePicture, no_picture: ");
     no_picture++;
     Serial.println(no_picture);
     lcd.print("PICTURE ");
     
     ret = JPEGCamera.makePicture (no_picture);
     if (ret == SUCCESS)
     { 
        lcd.setCursor(0,1);
        lcd.print("picture: "); lcd.print(no_picture);
     }
     else
     {
        Serial.print("makePicture error: "); Serial.println(ret);
        lcd.setCursor(0,1); 
        lcd.print("error: "); lcd.print(ret);       
        error = 1;
     }
             
     if (ret == SUCCESS)
     { 
        HPos = 0;
        VPos = 90;
        TiltPan_move(HPos, VPos);

        Serial.print("makePicture, no_picture: ");
        no_picture++;
        Serial.println(no_picture);
        lcd.print("PICTURE ");
        
        ret = JPEGCamera.makePicture (no_picture);

        lcd.setCursor(0,1);
        lcd.print("picture: "); lcd.print(no_picture);
     }
     else
     {
        Serial.print("makePicture error: "); Serial.println(ret);
        lcd.setCursor(0,1); 
        lcd.print("error: "); lcd.print(ret);       
        error = 1;
     }
      
     if (ret == SUCCESS)
     { 
        HPos = 180;
        VPos = 90;
        TiltPan_move(HPos, VPos);
     
        Serial.print("makePicture, no_picture: ");
        no_picture++;
        Serial.println(no_picture);
        lcd.print("PICTURE ");
        
        ret = JPEGCamera.makePicture (no_picture);

        lcd.setCursor(0,1);
        lcd.print("picture: "); lcd.print(no_picture);
     }
     else
     {
        Serial.print("makePicture error: "); Serial.println(ret);
        lcd.setCursor(0,1); 
        lcd.print("error: "); lcd.print(ret);       
        error = 1;
     }

     // byte 0: last picture number
     resp[0] = no_picture;
     *resplen = 0+1;
          
     HPos = 90;
     VPos = 90;
     TiltPan_move(HPos, VPos);
              
     break; 
     
 case CMD_CHECK: 
     Serial.println("CMD_CHECK");
     lcd.print("Check"); 
        
     alert_status = check();
     
     if (alert_status != SUCCESS) {
           Serial.print("Alert detected: ");Serial.println(alert_status);
           lcd.setCursor(0,1); 
           lcd.print("Alert: "); lcd.print(alert_status);                
     }
     else
     {
           Serial.print("No alert detected: ");
           lcd.setCursor(0,1); 
           lcd.print(" No Alert");               
     } 
  
     // byte 0: alert
     resp[0] = alert_status;
     *resplen = 0+1;
     break; 
  
 case CMD_GO: 
     Serial.print("CMD_GO, nb seconds: "); Serial.print((int)cmd[1]);
     lcd.print("GO ");lcd.print((int)cmd[1]);lcd.print("secs");
     
     if (motor_state != STATE_GO)
     {  
           Serial.println("start_forward");
           start_forward();
           motor_state = STATE_GO;
     }
     
     error = -1;
     GOtimeout = (unsigned long)cmd[1];
     start = millis();
     while((millis() - start < GOtimeout*1000) && (error == -1)) {
          ret = go(GOtimeout);  
     
          if ((ret != SUCCESS) && (ret != OBSTACLE) && (ret != OBSTACLE_LEFT) && (ret != OBSTACLE_RIGHT))
          {
              stop();
              motor_state = STATE_STOP;
     	      error = 1;
     	                   
              Serial.print("CMD_GO error: "); Serial.println(ret);
              Serial.println("Stop");              
              lcd.setCursor(0,1); 
              lcd.print("error: "); lcd.print(ret);                

          }
          else if ((ret == OBSTACLE) || (ret == OBSTACLE_LEFT) || (ret == OBSTACLE_RIGHT))
          {
              stop();
              motor_state = STATE_STOP;
              
              buzz(3);
              blink(Led_Red);     
              Serial.print("CMD_GO Obstacle: ");Serial.println(ret);
              Serial.println("Stop");
              lcd.setCursor(0,1); 
              if (ret == OBSTACLE_LEFT)        lcd.print("OBSTACLE LEFT");
              else if (ret == OBSTACLE_RIGHT)  lcd.print("OBSTACLE RIGHT");
              else if (ret == OBSTACLE)        lcd.print("OBSTACLE");
              else 

                              
              ret = SUCCESS;            
              checkdir = check_around();
         
              Serial.print("check_around, direction: "); Serial.println(checkdir);
              lcd.clear();
              lcd.print("check around");
              lcd.setCursor(0,1); 
              if      (checkdir == DIRECTION_LEFT)  lcd.print("LEFT");
              else if (checkdir == DIRECTION_RIGHT) lcd.print("RIGHT");
              else if (checkdir == OBSTACLE_LEFT)   lcd.print("OBSTACLE LEFT");
              else if (checkdir == OBSTACLE_RIGHT)  lcd.print("OBSTACLE RIGHT");
              else if (checkdir == OBSTACLE)        lcd.print("OBSTACLE");
              else                             lcd.print("?");;
         
              if (checkdir == DIRECTION_LEFT) {
                   start_forward();
                   motor_state = STATE_GO;
                   ret = turn (-45,  5); // turn  -45 degrees during 5s max
                   if (ret != SUCCESS)
                   {
                      stop();
                      motor_state = STATE_STOP;                   	  
                   	  error = 1;
                   	                     	  
                   	  Serial.print("turn error: "); Serial.println(ret);
                      Serial.println("Stop");                                         	  
                   	  lcd.clear();                   	  
                   	  lcd.print("turn left");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret); 
                   }
                   else
                   {
                      lcd.clear();                   	  
                   	  lcd.print("turn left OK");                  	
                   }
              }
              else if (checkdir == DIRECTION_RIGHT) {
                   start_forward();
                   motor_state = STATE_GO;
                   ret = turn (+45,  5); // turn  +45 degrees during 5s max
                   if (ret != SUCCESS)
                   {
                   	  stop();
                      motor_state = STATE_STOP;  
                   	  error = 1;
                   	                     	 
                   	  Serial.print("turn error: "); Serial.println(ret);
                   	  Serial.println("Stop"); 
                   	  lcd.clear();                   	  
                   	  lcd.print("turn right");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret); 
                   }
                   else
                   {
                      lcd.clear();                   	  
                   	  lcd.print("turn right OK");                  	
                   }                  
              }
              else 
              {
              	   buzz(3);
                   blink(Led_Red);
              	   motor_state = STATE_GO;
              	   ret = turnback (10); // turn back during 10s max
                   if (ret != SUCCESS)
                   {
                      stop();
                      motor_state = STATE_STOP;
                      error = 1; 
                      
                      Serial.print("turnback error"); Serial.println(ret);
                   	  Serial.println("Stop");
                   	  lcd.clear();                   	  
                   	  lcd.print("turnback");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret);                    	                                           	  
                   }
                   else
                   {
                      lcd.clear();                   	  
                   	  lcd.print("turnback OK");                  	
                   }                   
              }                 
          }
          else
          {
              	   Serial.println("GO OK");
              	   lcd.clear();                   	  
                   lcd.print("GO OK");
                   error = 0;
          }           
     } // end while
     
     ret = infos (resp, &infolen);
              
     if (error == 0) {
         if (resp[0] == STATE_GO) {
             lcd.print((int)resp[1]);lcd.print((char)126);lcd.print((byte)lcd_pipe);lcd.print((int)resp[2]);lcd.print((char)127);
         }    
         else
         {
             lcd.print("STOPPED");
         }
         lcd.setCursor(0,1);   
         lcd.print((int)resp[7]); lcd.print((byte)lcd_celcius);lcd.print((byte)lcd_pipe);   
         lcd.print((int)resp[6]); lcd.print("cm");lcd.print((byte)lcd_pipe);
         lcd.print((int)resp[5]); lcd.print((char)223); //degree   
     } 
     
     *resplen = infolen;      
     break;
 
 case CMD_PI: 
     Serial.print("CMD_PI activated, type: "); Serial.print((int)cmd[1]);  Serial.print(" - freq: ");Serial.println((int)cmd[2]);
     lcd.print("PI activated ");lcd.print((int)cmd[1]);
 
     PI_activated = (int)cmd[1];
     if (PI_activated == 2) PI_freqInfos = (int)cmd[2]* 1000;
        
     Serial.print("PI_activated: "); Serial.println(PI_activated);
     Serial.print("PI_freqInfos: ");Serial.println(PI_freqInfos);
  
     
    *resplen = 0;     
     break;
          
 default:
    Serial.println("invalid command");
    lcd.print("invalid command");
    
    *resplen = 0;      
    break;                     
 
 } //end switch
    
    
 if (error == 1) {
    blink(Led_Red);
    blink(Led_Red);
    buzz(7);
    *resplen = 0; 
 }
                        
 reset_leds();    // turn off all leds
 return ret;
}

int robot_main ()
{    
 uint16_t cmd[CMD_SIZE];
 uint16_t resp[RESP_SIZE];
 uint8_t resplen = 0;
 char buf[1024]= "";
 char szresp[10]= "";
 unsigned long currentTime;
 unsigned long currentTime2;
    
 int ret = SUCCESS;
 
 while (1) {
       
       currentTime2 = millis();
       if (currentTime2 > previousTime2 + freqCheck) {
             previousTime2 = currentTime2;   
             Serial.print("Call command CHECK every ");Serial.print(freqCheck/1000);Serial.println(" seconds");
             cmd[0] = CMD_CHECK;
             ret = robot_command (cmd, resp, &resplen);
 
             Serial.print("Call robot_command, ret: "); Serial.println(ret);	
             alert_status = resp[0];  
             Serial.print("Alert: "); Serial.println(alert_status);
       }
       
       currentTime = millis();
    
       if (alert_status > 0)
       {
             Serial.print("Alert trigerred: "); Serial.println(alert_status);	// robot has detected something...
             Serial.println("***************");
             Serial.println("");
             
             Serial.println("Call command INFOS");
             cmd[0] = CMD_INFOS;
             ret = robot_command (cmd, resp, &resplen);
             Serial.print("Call robot_command, ret: "); Serial.print(ret); Serial.print(", resplen: "); Serial.println(resplen);
            
             for(uint8_t j=0; j<resplen; j++)
             {
                    strcat(buf,szField[j]);
                    strcat(buf,"=");
                    sprintf(szresp, "%10d", (int)resp[j]);
                    strcat(buf,szresp);
                    strcat(buf,"&");
             }
			 Serial.println(buf);
 
             Serial.println("Call command ALERT");
  	         cmd[0] = CMD_ALERT;
             ret = robot_command (cmd, resp, &resplen);  
             Serial.print("Call robot_command, ret: "); Serial.println(ret);
             
             // Send Infos + last 3 pictures in WIFI to the PI server if it is activated
             if (PI_activated > 0) {
                Serial.println("Call WiFiRobot_SendAlert");
                ret = WiFiRobot.WiFiRobot_SendAlert(buf, resp[0]); // resp[0] = last picture number
                if (ret != SUCCESS) {
                      Serial.print("WiFiRobot_SendAlert error: "); Serial.println(ret);
                }
                else
                {
                      Serial.println("WiFiRobot_SendAlert OK"); 
                      alert_status = 0;
                }
             }
             else
             {
                      Serial.println("PI communucation not activated"); 
                      alert_status = 0;   
             }                        
       }
       else if (IntIOT > 0) { // Photon wants to do something...
             Serial.println("Request received from Photon, call robot_IOT");	
             Serial.println("***************");
             Serial.println("");
 
             ret = robot_IOT(); 
             if (ret != SUCCESS) {
                   Serial.print("robot_IOT error: "); Serial.println(ret);
             }
             else
             {
                   Serial.println("robot_IOT OK"); 
                   IntIOT = 0;
             }

       }
       else if (PI_activated == 2)
       {
             if ((PI_freqInfos > 0) && (currentTime > previousTime + PI_freqInfos)) {
                   previousTime = currentTime;      

                   Serial.println("Call command INFOS");
                   cmd[0] = CMD_INFOS;
                   ret = robot_command (cmd, resp, &resplen);
                   Serial.print("Call robot_command, ret: "); Serial.println(ret);
            
                   for(uint8_t j=0; j<resplen; j++)
                   {
                          strcat(buf,szField[j]);
                          strcat(buf,"=");
                          sprintf(szresp, "%10d", (int)resp[j]);
                          strcat(buf,szresp);
                          strcat(buf,"&");
                   }
			       Serial.println(buf);
			 
                   Serial.println("Call WiFiRobot_SendInfos");
                   ret = WiFiRobot.WiFiSendInfos(buf);
                   if (ret != SUCCESS) {
                         Serial.print("WiFiRobot_SendInfos error: "); Serial.println(ret);
                   }
             }
        
       }   
       else if (motor_state == STATE_GO)
       {       
             Serial.print("Call command GO for "); Serial.print(GOtimeout); Serial.println(" seconds");
             cmd[0] = CMD_GO;
             cmd[1] = GOtimeout;
             cmd[2] = 0;   // na  
             ret = robot_command (cmd, resp, &resplen);
  
             Serial.print("Call robot_command, ret: "); Serial.println(ret);
       }         
 } // end while 
 
   
}

int robot_IOT ()
{    
 uint8_t msg[MSG_SIZE_MAX];
 uint8_t msg_len = 0;
 uint8_t tag [MAX_TAGS];
 uint16_t value [MAX_TAGS];
 uint8_t nbtags;
 uint16_t cmd[CMD_SIZE];
 uint16_t resp[RESP_SIZE];
 uint8_t resplen = 0;
 uint8_t cmdId = 0;

 int ret = SUCCESS;
 
 Serial.println("Start robot_IOT");

 //Read the message received from Photon
 ret = IOT.RIOTread(msg, &msg_len);
 Serial.print("Call RIOTread, ret: "); Serial.print(ret); Serial.print(", msg_len: "); Serial.println((int)msg_len);

 if (ret != SUCCESS) {
       Serial.println("error RIOTread, Call RIOTsend with RESP_KO");  
       IOT.RIOTsend(RESP_KO, resp, 0);
 }
   
 IOT.RIOTgetTags(msg, tag, value, &nbtags);   
 
 Serial.print("nbtags: "); Serial.print((int)nbtags);
 if (nbtags < 1) {
       Serial.println("nbtags < 1, Call RIOTsend with RESP_KO");    
       IOT.RIOTsend(RESP_KO, resp, 0);
 }
 
 Serial.print(", tag[0]: "); Serial.print((int)tag[0],HEX); Serial.print(", value[0]: "); Serial.println((int)value[0],HEX);
 if (tag[0] != TAG_CMDID) {
       Serial.println("TAG_CMDID Missing, Call RIOTsend with RESP_KO");    
       IOT.RIOTsend(RESP_KO, resp, 0);
 }
 else
 {   
    cmdId = value[0];
    Serial.print("cmdId: "); Serial.println((int)cmdId);
 }
 
 if (tag[1] == TAG_CMD)
 {
       Serial.println("Tag CMD");
       if (nbtags > MAX_TAGS) {
               Serial.println("nbtags > MAX_TAGS, Call RIOTsend with RESP_KO");    
               IOT.RIOTsend(RESP_KO, resp, 0, cmdId);
       }     
             
       for (uint8_t j=0; j<(nbtags-1); j++)
       {
           cmd[j] = value[j+1];
           Serial.print("cmd["); Serial.print((int)j); Serial.print("]: "); Serial.println((int)cmd[j],HEX);
       }
 
       // Execute the command received from Photon
       ret = robot_command (cmd, resp, &resplen);  
       Serial.print("Call robot_command, ret: "); Serial.println(ret);

       if (ret == SUCCESS) { 
        	 Serial.println("Call RIOTsend with RESP_OK");    
             IOT.RIOTsend(RESP_OK, resp, resplen, cmdId);
       }
       else
       {
        	 Serial.println("Call RIOTsend with RESP_KO");    
             IOT.RIOTsend(RESP_KO, resp, 0, cmdId);
       }        
 }
 else if (tag[1] == TAG_RESP)
 {		
      Serial.println("Tag RESP");
      if (nbtags > RESP_SIZE) {
           	 Serial.println("nbtags > RESP_SIZE, Call RIOTsend with RESP_KO");    
             IOT.RIOTsend(RESP_KO, resp, 0);
      }
      
      // Update the robot with the response received from Photon            
      for (uint8_t j=0; j<(nbtags-1); j++)
      {
           resp[j] = value[j+1];
           Serial.print(", resp["); Serial.print((int)j); Serial.print("]: "); Serial.println((int)resp[j],HEX);
      }
 }
 else {
       Serial.println("Call RIOTsend with RESP_KO");    
       IOT.RIOTsend(RESP_KO, resp, 0);
 }
    
 Serial.println("End OK robot_IOT"); 
 return SUCCESS;                     
}			 


