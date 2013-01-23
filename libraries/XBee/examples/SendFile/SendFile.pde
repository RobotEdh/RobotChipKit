#include <XBee.h>
#include <xPrint.h>

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

#include <XPrint.h>

#include <SD.h>


void setup() {
  
  int ret = SUCCESS;
  xPrint p;
  
  Sd2Card card;
  SdVolume volume;
  SdFile myFile;
  SdFile root;
  int16_t n; 
  unsigned int idx = 0;
  char buf[PAYLOAD_SIZE-1]; //First byte is used as indicator
  char buffer[PAYLOAD_SIZE];
  
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
    
 
  if (myFile.open(&root, "test.jpg", O_READ)){
 
    Serial.println("File open OK");

    // read from the file until there's nothing else in it:
    while ((n = myFile.read(buf, sizeof(buf))) > 0 || ret != SUCCESS) {
      
       if (n == sizeof(buf)) 
       {
           buffer[0] = 0;
       }
       else
       {
           buffer[0] = 1; //end file read
       }  
    
       for (unsigned int i = 1;i<n+1;i++)
       {
           buffer [i] = buf[idx++];
       }
   	
       ret = p.xsendXbee(buffer, n+1);
 
    }// while
    
    // close the file
    if (!myFile.close())
    {
       Serial.println("Error close file");
    }
  }
  else
  {
    // if the file didn't open, print an error
    Serial.println("Error opening file");
  }

}

void loop() {

 
 
}
  
  


