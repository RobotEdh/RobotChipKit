#include <robot.h>         
#include <motor.h>             // Motor
#include <GP2Y0A21YK.h>        // IR sensor
#include <CMPS03.h>            // Compas
#include <TMP102.h>            // Temperature
#include <Servo.h>             // Servo
#include <TiltPan.h>           // Tilt&Pan
#include <LSY201.h>            // Camera
#include <LiquidCrystal_I2C.h> // LCD


int motor_state = STATE_STOP;

LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

JPEGCameraClass JPEGCamera;  // The Camera class  
int no_picture = 0;          // Picture number

void blink(int led)
{
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
 
  // initialize the lcd 
  lcd.init();                      
  lcd.backlight();
  lcd.print("Start Robot Init");
  
  Serial.println("Start Robot Init"); 
  pinMode(Led_Yellow, OUTPUT);     // set the pin as output
  blink(Led_Yellow);  
  pinMode(Led_Red, OUTPUT);        // set the pin as output
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
 
  digitalWrite(Led_Yellow, LOW);  // turn off led yellow
  digitalWrite(Led_Red, LOW);     // turn off led red
  
  Serial.println("End Robot Init");
  Serial.println("");
  lcd.setCursor(0,1); 
  lcd.print("End   Robot Init");
  
  return SUCCESS;
  
} 


int CmdRobot (uint16_t cmd [3], uint16_t *resp, int *presp_len)
{    
 CMPS03Class CMPS03;   // The Compass class
 TMP102Class TMP102;   // The Temperature class  
 int resp_len = 0;
 unsigned long timeout = 0;
 unsigned long start = 0;
 int dir;
 int error = -1;
 int ret = SUCCESS;
 
 digitalWrite(Led_Yellow, HIGH);  // turn on led yellow
 digitalWrite(Led_Red, LOW);      // turn off led red
 lcd.clear();                       // clear LCD
 
 switch (cmd[0]) {
 
 case CMD_STOP:
     Serial.println("CMD_STOP");
     lcd.print("STOP"); 
     
     stop();
     motor_state = STATE_STOP;
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
           Serial.print("CMD_START_TEST motor: ");
           Serial.println((int)cmd[1]);
           lcd.print("START motor: ");
           lcd.print((int)cmd[1]); 
           
           start_forward_test(cmd[1]);           
     }                      
     motor_state = STATE_GO;
     break; 
 
 case CMD_CHECK_AROUND:
     Serial.println("CMD_CHECK_AROUND");
     lcd.print("CHECK AROUND");
    
     dir = check_around();
     
     lcd.setCursor(0,1); 
     if      (dir == DIRECTION_LEFT)  lcd.print("LEFT");
     else if (dir == DIRECTION_RIGHT) lcd.print("RIGHT");
     else if (dir == OBSTACLE_LEFT)   lcd.print("OBSTACLE LEFT");
     else if (dir == OBSTACLE_RIGHT)  lcd.print("OBSTACLE RIGHT");
     else if (dir == OBSTACLE)        lcd.print("OBSTACLE");
     else                             lcd.print("?");;

     resp[0] = dir;
     resp_len = 0+1;
     
 case CMD_MOVE_TILT_PAN:
     Serial.print("CMD_MOVE_TILT_PAN, X Y: "); Serial.print((int)cmd[1]);Serial.println((int)cmd[2]);   
     lcd.print("MOVE TILT&PAN");
     lcd.setCursor(0,1); 
     lcd.print("X: ");
     lcd.print((int)cmd[1]);
     lcd.print(" Y: ");
     lcd.print((int)cmd[2]);
          
     TiltPan_move(cmd[1], cmd[2]);
     break; 
                    
 case CMD_TURN_RIGHT:
    lcd.print("TURN RIGHT "); lcd.print((int)cmd[1]);lcd.print((char)223); //degree   
 
     if (cmd[1] == 180)
     {       
           Serial.print("CMD_TURN_BACK");
           
           ret = turnback (10);  // 10s max
           if (ret != SUCCESS){
           	  Serial.print("CMD_TURN_BACK error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print("error: "); lcd.print(ret);
           	  error = 1;
           }
     }       
     else if (motor_state == STATE_GO)
     { 
           Serial.print("CMD_TURN_RIGHT, alpha: "); Serial.println((int)cmd[1]);;
         
           ret = turn ((double)cmd[1], 5);  // 5s max        
           if (ret != SUCCESS){
           	  Serial.print("CMD_TURN_RIGHT error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print("error: "); lcd.print(ret);
           	  error = 1;
           }           
    }
    break;        

 case CMD_TURN_LEFT:
     lcd.print("TURN LEFT "); lcd.print((int)cmd[1]);lcd.print((char)223); //degree   
     
     if (cmd[1] == 180)
     {       
           Serial.print("CMD_TURN_BACK");
                      
           ret = turnback (10);  // 10s max
           if (ret != SUCCESS){
           	  Serial.print("CMD_TURN_BACK error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print("error: "); lcd.print(ret);
           	  error = 1;
           }
     }    
     else if (motor_state == STATE_GO)
     { 
           Serial.print("CMD_TURN_LEFT, alpha: "); Serial.println((int)cmd[1]);
           
           ret = turn (-(double)cmd[1], 5);  // 5s max
           if (ret != SUCCESS){
           	  Serial.print("CMD_TURN_LEFT error"); Serial.println(ret);
           	  lcd.setCursor(0,1); 
           	  lcd.print("error: "); lcd.print(ret);
           	  error = 1;
           }
     }
     break;            
     
 case CMD_INFOS:    
     Serial.println("CMD_INFOS");
     
     // byte 0: motor_state
     resp[0] = motor_state;
     // byte 1: SpeedMotorRight
     resp[1] = get_SpeedMotorRight();
     // byte 2: SpeedMotorLeft
     resp[2] = get_SpeedMotorLeft();
     // byte 3: TickRight
     resp[3] = get_TickRight();
     // byte 4: TickLeft
     resp[4] = get_TickLeft();
     // byte 5: direction
     resp[5] = CMPS03.CMPS03_read();
     // byte 6: distance
     resp[6] = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
     // byte 7: temperature
     resp[7] = TMP102.TMP102_read();
     resp_len = 7+1;
    
     if (resp[0] == STATE_GO) {
         lcd.print((int)resp[1]);lcd.print((char)126);lcd.print(lcd_pipe,BYTE);lcd.print((int)resp[2]);lcd.print((char)127);
     }    
     else
     {
         lcd.print("STOPPED");
     }
     lcd.setCursor(0,1);   
     lcd.print((int)resp[7]); lcd.print(lcd_celcius,BYTE);lcd.print(lcd_pipe,BYTE);   
     lcd.print((int)resp[6]); lcd.print("cm");lcd.print(lcd_pipe,BYTE);
     lcd.print((int)resp[5]); lcd.print((char)223); //degree    
    
     break; 

 case CMD_PICTURE: 
     Serial.print("CMD_PICTURE, no_picture: ");
     no_picture++;
     Serial.println(no_picture);
     lcd.print("PICTURE ");
     
     Serial.println("Stop"); 
     stop();
     motor_state = STATE_STOP;
     
     ret = JPEGCamera.makePicture (no_picture);
     if (ret == SUCCESS)
     { 
           // byte 0: picture number
           resp[0] = no_picture;
           resp_len = 0+1;
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
                
     Serial.println("Start");
     start_forward();                     
     motor_state = STATE_GO;
     
     break;

 case CMD_GO: 
     Serial.print("CMD_GO, nb seconds: ");
     Serial.print((int)cmd[1]);
     Serial.print("\tPID: ");
     Serial.println((int)cmd[2]);
     lcd.print("GO ");lcd.print((int)cmd[1]);lcd.print("secs");
     
     if (motor_state =! STATE_GO)
     {  
           Serial.println("start_forward");
           start_forward();
     }

     motor_state = STATE_GO;
     
     error = -1;
     timeout = (unsigned long)cmd[1];
     start = millis();
     while((millis() - start < timeout*1000) && (error == -1)) {
          ret = go(timeout,(int)cmd[2]);  
     
          if ((ret != SUCCESS) && (ret != OBSTACLE))
          {
              Serial.print("CMD_GO error"); Serial.println(ret);
              lcd.setCursor(0,1); 
              lcd.print("error: "); lcd.print(ret);                
    	      error = 1;
          }
          else if (ret == OBSTACLE)
          {
              ret = SUCCESS;
              Serial.println("CMD_GO Obstacle");
              lcd.setCursor(0,1); 
              lcd.print("Obstacle");
              blink(Led_Red);                
              
              stop();
              motor_state = STATE_STOP;
              
              dir = check_around();
         
              Serial.print("check_around, direction: "); Serial.println(dir);
              lcd.clear();
              lcd.print("check around");
              lcd.setCursor(0,1); 
              if      (dir == DIRECTION_LEFT)  lcd.print("LEFT");
              else if (dir == DIRECTION_RIGHT) lcd.print("RIGHT");
              else if (dir == OBSTACLE_LEFT)   lcd.print("OBSTACLE LEFT");
              else if (dir == OBSTACLE_RIGHT)  lcd.print("OBSTACLE RIGHT");
              else if (dir == OBSTACLE)        lcd.print("OBSTACLE");
              else                             lcd.print("?");;
         
              if (dir == DIRECTION_LEFT) {
                   start_forward();
                   motor_state = STATE_GO;
                   ret = turn (-45,  5); // turn  -45 degrees during 5s max
                   if (ret != SUCCESS)
                   {
                   	  Serial.print("turn error"); Serial.println(ret);
                   	  lcd.clear();                   	  
                   	  lcd.print("turn left");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret);
                   	  error = 1;
                   }
                   else
                   {
                      lcd.clear();                   	  
                   	  lcd.print("turn left OK");                  	
                   }
              }
              else if (dir == DIRECTION_RIGHT) {
                   start_forward();
                   motor_state = STATE_GO;
                   ret = turn (+45,  5); // turn  +45 degrees during 5s max
                   if (ret != SUCCESS)
                   {
                   	  Serial.print("turn error"); Serial.println(ret);
                   	  lcd.clear();                   	  
                   	  lcd.print("turn right");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret);                   	  
                   	  error = 1;
                   }
                   else
                   {
                      lcd.clear();                   	  
                   	  lcd.print("turn right OK");                  	
                   }                  
              }
              else 
              {
              	   blink(Led_Red);
              	   motor_state = STATE_GO;
              	   ret = turnback (10); // turn back during 10s max
                   if (ret != SUCCESS)
                   {
                      Serial.print("turnback error"); Serial.println(ret);
                   	  lcd.clear();                   	  
                   	  lcd.print("turnback");
                   	  lcd.setCursor(0,1);
                   	  lcd.print("error: "); lcd.print(ret);                        
                   	  error = 1;
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
     
     // byte 0: motor_state
     resp[0] = motor_state;
     // byte 1: SpeedMotorRight
     resp[1] = get_SpeedMotorRight();
     // byte 2: SpeedMotorLeft
     resp[2] = get_SpeedMotorLeft();
     // byte 3: TickRight
     resp[3] = get_TickRight();
     // byte 4: TickLeft
     resp[4] = get_TickLeft();
     // byte 5: direction
     resp[5] = CMPS03.CMPS03_read();
     // byte 6: distance
     resp[6] = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
     // byte 7: temperature
     resp[7] = TMP102.TMP102_read();
     resp_len = 7+1;
     
     if (error == 0) {
         if (resp[0] == STATE_GO) {
             lcd.print((int)resp[1]);lcd.print((char)126);lcd.print(lcd_pipe,BYTE);lcd.print((int)resp[2]);lcd.print((char)127);
         }    
         else
         {
             lcd.print("STOPPED");
         }
         lcd.setCursor(0,1);   
         lcd.print((int)resp[7]); lcd.print(lcd_celcius,BYTE);lcd.print(lcd_pipe,BYTE);   
         lcd.print((int)resp[6]); lcd.print("cm");lcd.print(lcd_pipe,BYTE);
         lcd.print((int)resp[5]); lcd.print((char)223); //degree   
     } 
           
     break;
     
 default:
    Serial.println("invalid command");
    lcd.print("invalid command");
    break;                     
 
 } //end switch
    
 *presp_len = resp_len;
    
 if (error == 1) digitalWrite(Led_Red, HIGH); // turn on led red
 digitalWrite(Led_Yellow, LOW);               // turn off led yellow
                     
 return ret;
}
