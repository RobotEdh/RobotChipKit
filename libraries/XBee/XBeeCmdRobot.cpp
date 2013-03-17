
#include <XBeeCmdRobot.h>
#include <motor.h>
#include <XBee.h>       // XBee
#include <XBeeTools.h>  // XBee tools
#include <sdcard.h>     // used to read the picture on a SD-Card


XBeeTools xBT;           // The Xbee tools class



int XBeeSendPicture (int n)
{
  int ret=SUCCESS;
  int16_t nbytes; 
  char buf[PAYLOAD_SIZE-1];     //First byte is used as indicator
  uint8_t buffer[PAYLOAD_SIZE];
  
  SdFile root;        // SD Root
  SdFile FilePicture; // SD File
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
       if (ret != SUCCESS ) return -1;
 
  }// while
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}


int XBeeCmdRobot ()
{
 uint8_t cmd[3];
 uint8_t resp[10];
 int resp_len = 0;
 int ret = SUCCESS;
 
 while (1) { 
     ret = xBT.xBTreceiveXbee(cmd, 5000); // read 5 ms max
   
     if (ret == SUCCESS)
     {
           ret = CmdRobot (cmd, resp, &resp_len);
           if (ret == SUCCESS)
           {
                 if (resp_len > 0)
                 {
                       delay (3000);
                       if (cmd[0] == CMD_PICTURE)
                       { 
                             ret= XBeeSendPicture (resp[0]);
                             if (ret != SUCCESS){  Serial.print("XBeeSendPicture error"); Serial.print(ret);}
                       }
                       else
                       {
                             ret = xBT.xBTsendXbee(resp, sizeof (resp));
                             if (ret != SUCCESS)
                             {
                                 Serial.println("error xBTsendXbee");
                             }
                       }                                    
                  }
           }
           else
           {
                 Serial.println("error Callcmd");
           }                                       

     }   
 }// end while 1
}