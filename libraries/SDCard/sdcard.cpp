#include <sdcard.h>
#include <SD.h>       

Sd2Card card;       // SD Card       
SdVolume volume;    // SD Volume
SdFile root;        // SD Root


int initSDCard(void){
  if (!card.init(SPI_HALF_SPEED, SS_CS_Pin))  //Set SCK rate to F_CPU/4 (mode 1)
  {
  	Serial.println("\nError Init SD-Card");
  	return SDCARD_ERROR;
  }
  else
  {	 
  	// initialize a FAT volume
  	if (!volume.init(&card))
  	{
  	  	Serial.println("\nError Init Volume in SD-Card");
  	  	return SDCARD_ERROR;
	}  	  	
  	else
  	{ 
  		// Open volume
  		if (!root.openRoot(&volume))
  		{
  			Serial.println("\nError Open Volume in SD-Card");
  			return SDCARD_ERROR;
		}  	  	
  		else
  		{   
  			Serial.println("\nInitialization SD-Card OK");
  			return SUCCESS;    			
        } 
    }
  }            			
}    			


int infoSDCard(void){
    	
   	Serial.print("\nSD-Card type is ");
   	switch(card.type()){
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
		return SDCARD_ERROR;
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
    return SUCCESS;		
}
		