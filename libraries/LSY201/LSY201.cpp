/*
  LSY201.cpp - Library for commmunicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/

#include <Arduino.h> // used for delay function (core lib)

#include <LSY201.h>
#include <sdcard.h>   // used to store the picture on a SD-Card



//Commands for the LinkSprite Serial JPEG Camera LSY201
const uint8_t RESET_CAMERA[4] =        {0x56, 0x00, 0x26, 0x00};
const uint8_t RESET_CAMERA_OK[4] =     {0x76, 0x00, 0x26, 0x00};
const uint8_t TAKE_PICTURE[5] =        {0x56, 0x00, 0x36, 0x01, 0x00};
const uint8_t TAKE_PICTURE_OK[5] =     {0x76, 0x00, 0x36, 0x00, 0x00};
const uint8_t STOP_TAKING_PICS[5] =    {0x56, 0x00, 0x36, 0x01, 0x03};
const uint8_t STOP_TAKING_PICS_OK[5] = {0x76, 0x00, 0x36, 0x00, 0x00};
const uint8_t GET_SIZE[5] =            {0x56, 0x00, 0x34, 0x01, 0x00};
const uint8_t GET_SIZE_OK[7] =         {0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00};
const uint8_t COMPRESS[8] =            {0x56, 0x00, 0x31, 0x05, 0x01, 0x01, 0x12, 0x04};
const uint8_t COMPRESS_OK[5] =         {0x76, 0x00, 0x31, 0x00, 0x00};
const uint8_t IMAGE_SIZE_LARGE[5] =    {0x56, 0x00, 0x54, 0x01, 0x00};
const uint8_t IMAGE_SIZE_MEDIUM[5] =   {0x56, 0x00, 0x54, 0x01, 0x11};
const uint8_t IMAGE_SIZE_SMALL[5] =    {0x56, 0x00, 0x54, 0x01, 0x22};
const uint8_t IMAGE_SIZE_OK[5] =       {0x76, 0x00, 0x54, 0x00, 0x00};
const uint8_t IMAGE_SIZE_LARGE_S[9] =  {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x00};
const uint8_t IMAGE_SIZE_MEDIUM_S[9] = {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x11};
const uint8_t IMAGE_SIZE_SMALL_S[9] =  {0x56, 0x00, 0x31, 0x05, 0x04, 0x01, 0x00, 0x19, 0x22};
const uint8_t IMAGE_SIZE_S_OK[5] =     {0x76, 0x00, 0x31, 0x00, 0x00};
const uint8_t READ_DATA[8] =           {0x56, 0x00, 0x32, 0x0C, 0x00, 0x0A, 0x00, 0x00};
const uint8_t READ_DATA_OK[5] =        {0x76, 0x00, 0x32, 0x00, 0x00};

//Message send by the camera following power on
static const char *PWR_ON_MSG = "Init end\x0d\x0a";

//SD card
extern SdFile root;        // SD Root     
       SdFile FilePicture; // SD File
   
// Constructor
JPEGCameraClass::JPEGCameraClass()
{
}

//Initialize the Serial3 port and call reset ()
int JPEGCameraClass::begin(void)
{
	//Camera baud rate is 38400
	Serial3.begin(38400);
	
	//Reset the camera
	return reset();	
}


//Compare 2 uint8_t arrays
int JPEGCameraClass::uint8Compare(const uint8_t *a1, const uint8_t *a2, int len) 
{
  for(int i=0; i<len; i++)
      if(a1[i]!=a2[i])
          return -1;

  return 0;
}

//Send a basic command to the camera and get the response 
int JPEGCameraClass::sendCommand(const uint8_t * command, uint8_t* response, int wlen, int rlen)
{
	int ibuf=-1;

	while(Serial3.available() > 0) {
	    ibuf = Serial3.read();
    } // waiting for data in the serial buffer
 
	//Send each character in the command string to the camera through the camera serial port
	for(int i=0; i<wlen; i++){
		Serial3.write(*command++);
	}

	//Get the response from the camera and add it to the response string
	for(int i=0; i<rlen; i++)
	{
		while(Serial3.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial3.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}
 
	return 0;
}

//Get the size of the image currently stored in the camera
int JPEGCameraClass::getSize(int * size)
{
	int ibuf=-1;
	uint8_t response[7];

	//Flush out any data currently in the serial buffer
	Serial3.flush();
	
	//Send the command to get read_size bytes of data from the current address
	for(int i=0; i<5; i++) Serial3.write(GET_SIZE[i]);


	//Read the response header.
	for(int i=0; i<7; i++){
		while(Serial3.available() == 0);
		ibuf = Serial3.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;;
	}
	
	if (uint8Compare(response,GET_SIZE_OK,7) != 0) return -2;   
	
	//Now read the actual data and add it to the response string.
    for(int j=0; j<2; j++){
		while(Serial3.available() == 0);
		ibuf = Serial3.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[j] = (uint8_t)ibuf;;
	}
	
	//The size is in the last 2 characters of the response.
	//Parse them and convert the characters to an integer
    *size = (int)response[0] * 256;
    *size += (int)response[1] & 0x00FF;	
    
	return 0;
}

//Reset the camera
int JPEGCameraClass::reset()
{
	int ret=0;
	uint8_t response[4];
	int ibuf=-1;

 	//Flush out any data currently in the serial buffer
	Serial3.flush();

 	ret = sendCommand(RESET_CAMERA, response, 4, 4);

	if (ret != 0) return ret;
	if (uint8Compare(response,RESET_CAMERA_OK,4) != 0) return -2;   
  
    // Wait for Init end
    for (int k = 0; k < strlen(PWR_ON_MSG); k++) {
          int cnt = 0;

          do {
		    while(Serial3.available() == 0); // waiting for data in the serial buffer
		    ibuf = Serial3.read();
		    if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		    Serial.write(byte(ibuf));    // display the init message
            cnt++;
            if (cnt > 128) return -5;
          } while (ibuf != PWR_ON_MSG[k]);
    }    
	    
	delay(3000); // wait 3s after a reset
	 
	return 0;
}

//Take a picture
int JPEGCameraClass::takePicture()
{
    int ret=0;
    uint8_t response[5];
		
    ret = sendCommand(TAKE_PICTURE, response, 5, 5);
    if (ret != 0) return -10;
    
    if (uint8Compare(response,TAKE_PICTURE_OK,5) != 0) return -2;
	    
    return 0;      
}

//Stop taking pictures
int JPEGCameraClass::stopPictures()
{
	int ret=0;
	uint8_t response[5];

	ret = sendCommand(STOP_TAKING_PICS, response, 5, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,STOP_TAKING_PICS_OK,5) != 0) return -2;
	    
	return 0; 
}

//Change the size of the image is Small format (160*120) but without saving the configuration so it will turn back to 320*240 after disconnect or reset
int JPEGCameraClass::imageSizeSmall()
{
	int ret=0;
	uint8_t response[5];
	
	ret = sendCommand(IMAGE_SIZE_SMALL, response, 5, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,IMAGE_SIZE_OK,5) != 0) return -2;
	    
	return 0;	
}

//Change the size of the image is Medium format (320*240) but without saving the configuration so it will turn back to 320*240 after disconnect or reset
int JPEGCameraClass::imageSizeMedium()
{
	int ret=0;
	uint8_t response[5];
		
	ret = sendCommand(IMAGE_SIZE_MEDIUM, response, 5, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,IMAGE_SIZE_OK,5) != 0) return -2;
	    
	return 0;	
}

//Change the size of the image is Large format (640*480) but without saving the configuration so it will turn back to 320*240 after disconnect or reset
int JPEGCameraClass::imageSizeLarge()
{
	int ret=0;
	uint8_t response[5];
		
	ret = sendCommand(IMAGE_SIZE_LARGE, response, 5, 5);
	if (ret != 0) return -10;
	    
	if (uint8Compare(response,IMAGE_SIZE_OK,5) != 0) return -2;
	    
	return 0;	
}

//Change the size of the image is Small format (160*120) with saving the configuration then reset
int JPEGCameraClass::imageSizeSmallSave()
{
	int ret=0;
	uint8_t response[5];
		
	ret = sendCommand(IMAGE_SIZE_SMALL_S, response, 9, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,IMAGE_SIZE_S_OK,5) != 0) return -2;
	    
	return reset();
	
}

//Change the size of the image is Medium format (320*240) with saving the configuration then reset
int JPEGCameraClass::imageSizeMediumSave()
{
    int ret=0;
	uint8_t response[5];
		
	ret = sendCommand(IMAGE_SIZE_MEDIUM_S, response, 9, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,IMAGE_SIZE_S_OK,5) != 0) return -2;	
	    
	return reset();
	
}

//Change the size of the image is Large format (640*480) with saving the configuration then reset
int JPEGCameraClass::imageSizeLargeSave()
{
    int ret=0;
	uint8_t response[5];
		
	ret = sendCommand(IMAGE_SIZE_LARGE_S, response, 9, 5);
	if (ret != 0) return -10;
	
	if (uint8Compare(response,IMAGE_SIZE_S_OK,5) != 0) return -2;	
	
	return reset();
		
}

//Compress the picture according the ratio
int JPEGCameraClass::compress(int ratio)
{
	int ibuf=-1;
	uint8_t response[5];

    // Ratio must be defined between 0X00 and 0XFF, usually 36
    if ((ratio < 0x00) || (ratio > 0xFF)) return -3;
        
	//Flush out any data currently in the serial buffer
	Serial3.flush();
	
	//Send the command to compress according the ratio
	for(int i=0; i<8; i++)Serial3.write(COMPRESS[i]);
	
	Serial3.write((uint8_t)ratio);
	
	//Get the response from the camera and add it to the response string. Always 5 bytes for basic commands
	for(int i=0; i<5; i++)
	{
		while(Serial3.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial3.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}

	if (uint8Compare(response,COMPRESS_OK,5) != 0) return -2;
	    
	return 0; 	
}

//Get the picture data from the camera 
int JPEGCameraClass::readData(long int address, uint8_t * buf, int * count, int * eof)
{
    const int read_size=32; //We read data from the camera in chunks, this is the chunk size
    int ibuf=-1;
	uint8_t response[5]; 
	int j=0;
	int icount=0;
	int end_frame=0;

	//Send the command to get read_size bytes of data from the current address
	for(int i=0; i<8; i++)Serial3.write(READ_DATA[i]);
	
	Serial3.write((uint8_t)(address>>8));
	Serial3.write((uint8_t)address);
	Serial3.write((uint8_t)0x00);
	Serial3.write((uint8_t)0x00);
	Serial3.write((uint8_t)(read_size>>8));
	Serial3.write((uint8_t)read_size);
	Serial3.write((uint8_t)0x00);
	Serial3.write((uint8_t)0x0A);	// delay between 2 frames read must be = 0x0A*0.01 ms = 0.1 ms

	//Read the response header.
	for(int i=0; i<5; i++)
	{
		while(Serial3.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial3.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}

	if (uint8Compare(response,READ_DATA_OK,5) != 0)  return -2;
	    	
	//Now read the actual data.
	icount=0;
	j= 0;
	while(j < read_size)
	{
		while(Serial3.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial3.read();
		if (ibuf == -1) return -100; // serial buffer empty, should not happen as we wait before
	        buf[j] = (uint8_t)ibuf;
	        if((j > 1) && (buf[j-1] == (uint8_t)0xFF) && (buf[j] == (uint8_t)0xD9))end_frame = 1; //end of frame detected
	        if (end_frame == 0) icount+=1;
	        j+=1;     
	}
	
	//Read the response footer.
	for(int i=0; i<5; i++)
	{
		while(Serial3.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial3.read();
		if (ibuf == -1) return -1000; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}
	
	if (uint8Compare(response,READ_DATA_OK,5) != 0)  return -20;
	
	// set eof in cae of end of frame
	if (end_frame == 1) *eof = 1;
	else *eof = 0;
	
	// set count
	*count = icount;      
	    
	return SUCCESS;
}


//make a picture
int JPEGCameraClass::makePicture (int n)
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
  sprintf(filename, "PICT%d.jpg", n);
  if (!FilePicture.open(&root, filename, O_CREAT|O_WRITE|O_TRUNC)) return FILE_OPEN_ERROR;  
  
  //Take a picture
  ret=takePicture();
  if (ret != SUCCESS) return CAMERA_ERROR;
     
  //Get the size of the picture    
  ret=getSize(&size);
  if (ret != SUCCESS ) return CAMERA_ERROR;
 
  //Starting at address 0, keep reading data until we've read 'size' data
  while(address < size)
  {       
        //Read the data starting at the current address
        ret=readData(address, buf, &count, &eof);
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
  ret=stopPictures();
  if (ret != SUCCESS ) return CAMERA_ERROR;
  
  //Close file
  if (!FilePicture.close()) return FILE_CLOSE_ERROR;  
  
  return SUCCESS;
}
