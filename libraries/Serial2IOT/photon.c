#define MSG_SIZE_MAX 255
#define MAX_TAGS     63
#define MAX_PARAMS   63

#define CMD_START         0x01
#define CMD_STOP          0x02
#define CMD_INFOS         0x03
#define CMD_PICTURE       0x04
#define CMD_TURN          0x05
#define CMD_CHECK_AROUND  0x06
#define CMD_MOVE_TILT_PAN 0x07
#define CMD_GO            0x08
#define CMD_ALERT         0x09
#define CMD_CHECK         0x0A
#define CMD_PI            0x0B

#define CMD_SIZE   3


// 0: motor_state
// 1: SpeedMotorRight
// 2: SpeedMotorLeft    
// 3: direction
// 4: distance
// 5: temperature
// 6: brightness
// 7: noise
// 8 alert status
// 9: picture number
#define RESP_SIZE 10

const uint8_t CMD          = 1;
const uint8_t RESP_OK      = 2;
const uint8_t RESP_KO      = 3;

const uint8_t TAG_CMD      = 'C';
const uint8_t TAG_PARAM    = 'P';
const uint8_t TAG_RESP     = 'R';
const uint8_t TAGSYMBOL    = '#';
const uint8_t SBN1 = 0xFA;
const uint8_t SBN2 = 0xFB;
const uint8_t EBN1 = 0xFE;
const uint8_t EBN2 = 0xFF;

char infos [622] = "{\"MotorState\" : 1, \"Direction\" : 120,\"Distance\" : 30,\"Temperature\" : 25,\"Brightness\"  : 140,\"Noise\" : 300,\"AlertStatus\" : 2,\"PictureNumber\" : 3}";
unsigned long previousTime = 0;
unsigned long freqInfos = 0;

void RIOTsend(uint8_t msgtype, uint8_t value1, uint16_t *param, uint8_t paramlen)
{
    Serial1.write(SBN1);
    Serial1.write(SBN2);
    
    Serial1.write(TAGSYMBOL);
    
    switch (msgtype) {
        
    case RESP_OK:
    case RESP_KO:
       Serial1.write(TAG_RESP);
       Serial1.write(uint8_t(0));
       Serial1.write(msgtype);
 
       break; 
   
    case CMD:
       Serial1.write(TAG_CMD);
       Serial1.write(uint8_t(0));
       Serial1.write(value1);
 
       break;         
    }     
        
    for (uint8_t j=0; j<paramlen; j++)
    {
       Serial1.write(TAGSYMBOL);
       Serial1.write(TAG_PARAM);
       Serial1.write((uint8_t)(param[j]>>8));
       Serial1.write((uint8_t)(param[j]));
    }
        
    Serial1.write(EBN1);
    Serial1.write(EBN2);

}

int RIOTread(uint8_t *msg, uint8_t *msglen)
{
	int ibuf =-1;
	uint8_t istart = 0;
	uint8_t istop  = 0;
	uint8_t i = 0;

	//Get the response from the IOT and add it to the response string
	while ((istop != 2) && (i < MSG_SIZE_MAX))
	{
		while(Serial1.available() == 0); // waiting for data in the serial buffer
		Serial.print("RIOTread, read ");
 
		ibuf = Serial1.read();
		Serial.print("ibuf: "); Serial.println(ibuf);
 
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		
		if      (ibuf == SBN1) istart = 1;
		else if (ibuf == SBN2) istart++;
		else if (ibuf == EBN1) istop = 1;
		else if (ibuf == EBN2) istop++;    
        else if (istart == 2) msg[i++] = (uint8_t)ibuf;
	}
    
    if (i == MSG_SIZE_MAX) return -11;
    
    *msglen = i;
	return 0;
}

void RIOTgetTags(uint8_t *buf, uint8_t *tag, uint16_t *value, uint8_t *nbtags)
{
   uint8_t i=0;
   uint8_t p=0;

   while ((buf[i] == TAGSYMBOL) && (p < MAX_TAGS))
   {
       tag[p]   = buf[i+1];
       value[p] = (uint16_t)(buf[i+2]<<8) | buf[i+3];
       p++;
       i=i+4;
   }
   *nbtags = p;
   
}
int robotInfos() {
    uint16_t param[MAX_PARAMS];
    uint8_t msg[MSG_SIZE_MAX];
    uint8_t msg_len = 0;
    uint8_t tag [MAX_TAGS];
    uint16_t value [MAX_TAGS];
    uint8_t nbtags;    
    
    Serial.print("Begin robotInfos");
   
    digitalWrite(D0, HIGH); // interrupt the Robot
    RIOTsend(CMD, CMD_INFOS, param, 0); // send the command INFOS to the Robot
    digitalWrite(D0, LOW); // reset interrupt of the Robot

    int ret = RIOTread(msg, &msg_len); // read the response from the Robot
    Serial.print("Call RIOTread, ret: "); Serial.print(ret); Serial.print(", msg_len: "); Serial.println((int)msg_len);
   
    RIOTgetTags(msg, tag, value, &nbtags); // parse the response  
    Serial.print("nbtags: "); Serial.print((int)nbtags);
    
    if (nbtags == 0)         return -1; 
    
    Serial.print("tag[0]: "); Serial.print((int)tag[0]);  
    if (tag[0]!= TAG_RESP)   return -2;

    Serial.print("value[0]: "); Serial.print((int)value[0]);        
    if (value[0] == RESP_KO) return -3;
    if (value[0] != RESP_OK) return -4;
        
    Serial.print("value[1]: "); Serial.print((int)value[1]);               
    Serial.print("value[2]: "); Serial.print((int)value[2]);               
    Serial.print("value[3]: "); Serial.print((int)value[3]);               
    Serial.print("value[4]: "); Serial.print((int)value[4]);               
    Serial.print("value[5]: "); Serial.print((int)value[5]);               
    Serial.print("value[6]: "); Serial.print((int)value[6]);               
    Serial.print("value[7]: "); Serial.print((int)value[7]);               
    Serial.print("value[8]: "); Serial.print((int)value[8]);               
    
    // encode in JSON
    sprintf(infos,"{\"MotorState\": %lu, \"Direction\": %lu, \"Distance\": %lu, \"Temperature\": %lu, \"Brightness\": %lu, \"Noise\": %lu, \"AlertStatus\": %lu, \"PictureNumber\": %lu}",value[1],value[2],value[3],value[4],value[5],value[6],value[7],value[8]);
 
    Serial.print("End OK robotInfos");
    return 0;
}

void setup() {

    pinMode(D0, OUTPUT);            // sets pin as output,used to interrupt the robot

    Serial.begin(9600);   // open serial port 0, used for logging, sets data rate to 9600 bps
    Serial1.begin(38400); // open serial port 1, used to communicate with the robot, sets data rate to 38400 bps
  
    Particle.function("robotCmd", robotCmd);          // Declare the function used to handle the commands received
    Particle.variable("robotInfos", infos, STRING);   // Declare the variable used to provide infos

    Serial.print("End OK setup");

}

void loop() {
    unsigned long currentTime = millis();
    int ret = 0;  
  
    if ((freqInfos > 0) && (currentTime > previousTime + freqInfos)) {
        previousTime = currentTime;
        
        ret = robotInfos();
    }  
   
}

int robotCmd(String command) {
  int cmdTag = -1;
  int paramTag = -1;
  int Separator = -1;
  int Start = -1;
  int Stop = -1;
  String szcmd;
  String szparam[10];
  int iparam[10];

  uint8_t cmd;
  uint16_t param[MAX_PARAMS];
  uint8_t paramlen = 0;
  uint8_t msg[MSG_SIZE_MAX];
  uint8_t msg_len = 0;
  uint8_t tag [MAX_TAGS];
  uint16_t value [MAX_TAGS];
  uint8_t nbtags;
  
     
  Serial.print("Start robotCmd, command :"); Serial.println(command);

  cmdTag = command.indexOf("CMD=");
  if (cmdTag != -1)
  {   
       Serial.print("CMD=");
       Separator = command.indexOf('|', cmdTag + 1);
       Stop = command.indexOf(">");
       if (Stop == -1) return -1;
       else if (Separator == -1) szcmd = command.substring(cmdTag + 4, Stop);
       else szcmd = command.substring(cmdTag + 4, Separator);
       Serial.println(szcmd);

       paramTag = command.indexOf("PARAM=");
       if (paramTag != -1)
       {   
          Start = paramTag + 6;
          Separator = command.indexOf('|', Start + 1);
          for (int p=0; (Separator != -1) && (Start < Stop) ; p++)
          {
               szparam[p] = command.substring(Start, Separator);
               iparam[p] = szparam[p].toInt();
               Serial.print("param"); Serial.print(p); Serial.print("=");
               Serial.print(szparam[p]); Serial.print("/"); Serial.println(iparam[p]);
               Start = Separator + 1;
               if ( Start < Stop) Separator = command.indexOf('|', Start + 1);              
          }                                   
      }
      else Serial.println("No PARAM");
  
  }
  else Serial.println("No CMD");
   

       if (szcmd == "START")
       {
               cmd = CMD_START;
               param[paramlen++] = 0;
       }        
       else if (szcmd == "STOP")
       {
               cmd = CMD_STOP;
        }                                 
       else if (szcmd == "CHECK")
       {
               cmd = CMD_CHECK;
       }                                    
       else if (szcmd == "PICTURE")
       {
               cmd = CMD_PICTURE;
       } 
       else if (szcmd == "ALERT")
       {
               cmd = CMD_ALERT;
       } 
       else if (szcmd == "TURN")
       {
               cmd = CMD_TURN;
               param[paramlen++] = abs(iparam[0]);
               param[paramlen++] = ((iparam[0] != abs(iparam[0])) ? (1):(0));
       }                                                                       
       else if (szcmd == "CHECK_AROUND")
       {
               cmd = CMD_CHECK_AROUND;
       }                                    
       else if (szcmd == "MOVE_TILT_PAN")
       {
               cmd = CMD_MOVE_TILT_PAN;
               param[paramlen++] = abs(iparam[0]);
               param[paramlen++] = ((iparam[0] != abs(iparam[0])) ? (1):(0));
               param[paramlen++] = abs(iparam[1]);
               param[paramlen++] = ((iparam[1] != abs(iparam[1])) ? (1):(0));
       }                                    
       else if (szcmd == "GO")
       {
               cmd = CMD_GO;
               param[paramlen++] = iparam[0];
       } 
       else if (szcmd == "PI")
       {
               cmd = CMD_PI;
               param[paramlen++] = iparam[0];
               param[paramlen++] = iparam[1];
       }        
       else if (szcmd == "INFOS")
       {
               freqInfos = (unsigned long)iparam[0]*1000;
               Serial.print("new freqInfos: "); Serial.println(freqInfos);
               return 0;  
       }
       else if (szcmd == "TEST")
       {
               Serial.print("Test command with param: "); ;  Serial.println((int)iparam[0]);    
               return 0;
       }
  
    Serial.print("cmd: "); Serial.println((int)cmd);               
    Serial.print("param[0]: "); Serial.println((int)param[0]);               
    Serial.print("param[1]: "); Serial.println((int)param[1]);               
    Serial.print("param[2]: "); Serial.println((int)param[2]); 
    Serial.print("param[3]: "); Serial.println((int)param[3]);       
  
    digitalWrite(D0, HIGH); // interrupt the Robot
    RIOTsend(CMD, cmd, param, paramlen); // send the command to the Robot
    digitalWrite(D0, LOW); // reset interrupt of the Robot

    int ret = RIOTread(msg, &msg_len); // read the response from the Robot
    Serial.print("Call RIOTread, ret: "); Serial.print(ret); Serial.print(", msg_len: "); Serial.println((int)msg_len);
   
    RIOTgetTags(msg, tag, value, &nbtags); // parse the response  
    Serial.print("nbtags: "); Serial.print((int)nbtags);
    
    if (nbtags == 0)         return -1; 
    
    Serial.print("tag[0]: "); Serial.print((int)tag[0]);  
    if (tag[0]!= TAG_RESP)   return -2;

    Serial.print("value[0]: "); Serial.print((int)value[0]);        
    if (value[0] == RESP_KO) return -3;
    if (value[0] != RESP_OK) return -4;
        
    Serial.print("value[1]: "); Serial.print((int)value[1]);               
    if (nbtags > 1)          return value[1]; 

    return 0;
    
}