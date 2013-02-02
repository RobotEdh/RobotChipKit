#include <XBee.h>

/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "xBeeTools.h"

#include <SD.h>


void setup() {
  
  int ret = SUCCESS;
  
  Sd2Card card;
  SdVolume volume;
  SdFile FilePicture;
  SdFile root;
  int16_t nbytes; 
  xBeeTools xBT;           // The Xbee tools class

  uint8_t buf[PAYLOAD_SIZE-1]; //First byte is used as indicator
  uint8_t buffer[PAYLOAD_SIZE];
  
  Serial.begin(9600); // initialize serial port for debug

  if (!card.init(SPI_HALF_SPEED,4)) 
  {
      Serial.println("Error card init");
  }
  // initialize a FAT volume
  if (!volume.init(&card)) 
  {
      Serial.println("Error volume init");
  }
  // Ouverture du volume
  if (!root.openRoot(&volume)) 
  {
      Serial.println("Error volume open");
  }

  Serial.println("Initialization done.");
    
 
  // Open the file
  if (!FilePicture.open(&root, "PICT01.jpg", O_READ)) Serial.println("Error file open"); 

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

  }// while
   Serial.println("end");
   Serial.println("+++++++");
  //Close file
  if (!FilePicture.close()) Serial.println("Error file close");
  
}

void loop() {

 
 
}
  