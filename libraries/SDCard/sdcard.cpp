#include <sdcard.h>

Sd2Card card;       // SD Card       
SdVolume volume;    // SD Volume
SdFile root;        // SD Root

int initSDCard(void){
    
  if (!card.init(SPI_HALF_SPEED, SS_CS_Pin))  //Set SCK rate to F_CPU/4 (mode 1)
  {
  	Serial.println("Error Init SD-Card");
  	return SDCARD_ERROR;
  }
  else
  {	 
  	// initialize a FAT volume
  	if (!volume.init(&card))
  	{
  	  	Serial.println("Error Init Volume in SD-Card");
  	  	return SDCARD_ERROR;
	}  	  	
  	else
  	{ 
  		// Open volume
  		if (!root.openRoot(&volume))
  		{
  			Serial.println("Error Open Volume in SD-Card");
  			return SDCARD_ERROR;
		}  	  	
  		else
  		{   
  			return SUCCESS;    			
        } 
    }
  }            			
}    			


int infoSDCard(void){
    	
   	Serial.print("SD-Card type is ");
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
				  				
    uint32_t volumesize;
    volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
    volumesize *= volume.clusterCount();       // we'll have a lot of clusters
    volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
    Serial.print("Volume size (Kb):  ");
    Serial.println(volumesize);
    Serial.print("Volume size (Mb):  ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Gb):  ");
    Serial.println((float)volumesize / 1024.0);

	// list all files in the card with date and size			    
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
	Serial.println("");
				
    return 0;		
}
		