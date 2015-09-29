/*
  LSY201.cpp - Library for commmunicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/

#include <WProgram.h> // used for delay function (core lib)

/*
  LSY201.h - Library for communicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/


/* Camera                                                                */
/* UART2 interface is provided on pins:                                  */
/*        1 = Power +5V                                                  */                
/*        2 = TX connected to J4-04 (AETXD1/SCK1A/U1BTX/U1ARTS/CN21/RD15)*/
/*        3 = RX connected to J4-03 (AETXD0/SS1A/U1BRX/U1ACTS/CN20/RD14) */
/*        4 = Ground                                                     */


#include <inttypes.h>

#define SUCCESS 0
#define CAMERA_ERROR -6
#define FILE_OPEN_ERROR -1000
#define FILE_CLOSE_ERROR -1001


class JPEGCameraClass
{
	public:
		JPEGCameraClass();
			
		int reset(void);
        /* Description: Reset the camera                                              */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */                            
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -5 if bad init message received from the camera         */
        /*                  = -10 if error reading the serial buffer or sendCommand   */
        /*                  = 0 otherwise                                             */ 
        /* lib:         sendCommand                                                   */
        /*              uint8Compare                                                  */
        /*              Serial1.flush                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        		
		int begin(void);
        /* Description: Initialize the serial1 port and call reset ()                 */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */ 
        /*                  = return of the reset() function otherwise                */ 
        /* lib:       	Serial1.begin                                                 */
        /*              reset                                                         */	
			
		int getSize(int * size);
        /* Description: Get the size of the image currently stored in the camera      */                                            
        /* input:       none                                                          */                      
        /* output:      size                                                          */
        /*                  = size of the image                                       */
        /*              return                                                        */                            
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial1.flush                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        /*              Serial1.write                                                 */       		
		
		int takePicture(void);
        /* Description: Take a picture                                                */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
 		
		
		int stopPictures(void);
        /* Description: Stop taking pictures                                          */                                            
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
		
		int readData(long int address, uint8_t *buf, int *count, int * eof);
        /* Description: Get the picture data from the camera                          */                                            
        /* input:       address                                                       */
        /*                  = address of the image, must start at 0                   */
        /* output:      buf                                                           */
        /*                  = buffer read from camera but without headers             */
        /*              count                                                         */
        /*                  = number of bytes read                                    */                
        /*              eof                                                           */
        /*                  = 1 if end of picture else = 0                            */ 
        /*              return                                                        */                            
        /*                  = -2, -20 if bad answer from the camera                   */
        /*                  = -10, -100, -1000 if error reading the serial buffer     */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        /*              Serial1.write                                                 */ 
        		
		int compress(int ratio);
        /* Description: Compress the picture according the ratio                      */                                            
        /* input:       ratio                                                         */
        /*                  = ratio for compression (0X00 < ratio < 0XFF, usually 36) */                     
        /* output:      return                                                        */                           
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -3 if invalid ratio                                     */
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              Serial1.flush                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        /*              Serial1.write                                                 */       		
		
		int imageSizeSmall(void);
        /* Description: Change the size of the image is Small format (160*120)        */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */		

		int imageSizeMedium(void);
        /* Description: Change the size of the image is Medium format (320*240)       */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */			
	    
	    int imageSizeLarge(void);
        /* Description: Change the size of the image is Large format (640*480)        */
        /*              but without saving the configuration                          */
        /*              so it will turn back to 320*240 after disconnect or reset     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = 0 otherwise                                             */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */	
        
		int imageSizeSmallSave(void);
        /* Description: Change the size of the image is Small format (160*120)        */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */			
		
		int imageSizeMediumSave(void);
        /* Description: Change the size of the image is Medium format (320*240)       */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
        		
		int imageSizeLargeSave(void);
        /* Description: Change the size of the image is Large format (640*480)        */
        /*              with saving the configuration, then reset                     */
        /* input:       none                                                          */                      
        /* output:      return                                                        */
        /*                  = -2 if bad answer from the camera                        */
        /*                  = -10 if error sendCommand                                */
        /*                  = return of the reset() function otherwise                */ 
        /* lib:         uint8Compare                                                  */
        /*              sendCommand                                                   */
                
        int makePicture (int n);
        /* Description: make a picture using the JPEGCamera lib                       */
        /*              and save it on a SD card in a file PICTxx.jpg                 */
        /* input:       n                                                             */ 
        /*                  = picture number xx                                       */     
        /* output:      return                                                        */                            
        /*                  = FILE_OPEN_ERROR if an error occurs during file opening  */
        /*                  = FILE_CLOSE_ERROR if an error occurs during file closing */
        /*                  = CAMERA_ERROR if an error occurs with the camera         */
        /*                  = SUCCESS otherwise                                       */  
        /* lib:         sprintf                                                       */                                
        /*              open (file)                                                   */
        /*              write (file)                                                  */
        /*              close (file)                                                  */                                  
        /*              JPEGCamera.takePicture                                        */
        /*              JPEGCamera.getSize                                            */  
        /*              JPEGCamera.readData                                           */
        /*              JPEGCamera.stopPictures                                       */				
	private:
		
		int sendCommand(const uint8_t *command, uint8_t *response, int wlen, int rlen);
        /* Description: Send a basic command to the camera and get the response       */                                            
        /* input:       command                                                       */ 
        /*                  = command to send                                         */ 
        /* input:       wlen                                                          */
        /*                  = command length                                          */ 
        /* input:       rlen                                                          */
        /*                  = response length expected                                */                               
        /* output:      response                                                      */
        /*                  = reponse received                                        */
        /*              return                                                        */                            
        /*                  = -10 if error reading the serial buffer                  */
        /*                  = 0 otherwise                                             */ 
        /* lib:         Serial1.write                                                 */
        /*              Serial1.available                                             */
        /*              Serial1.read                                                  */
        /*              millis                                                        */
        
		int uint8Compare(const uint8_t *a1, const uint8_t *a2, int len);
        /* Description: Compare 2 uint8_t arrays of same size                         */                                            
        /* input:       a1                                                            */ 
        /*                  = first uint8_t to compare                                */ 
        /* input:       a2                                                            */
        /*                  = second uint8_t to compare                               */ 
        /* input:       len                                                           */
        /*                  = length of the 2 arrays                                  */                               
        /* output:      return                                                        */                       
        /*                  = 0 if the 2 arrays are equal                             */
        /*                  = -1 otherwise                                            */ 
        /* lib:         none                                                          */
		
};





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

  
// Constructor
JPEGCameraClass::JPEGCameraClass()
{
}

//Initialize the serial1 port and call reset ()
int JPEGCameraClass::begin(void)
{
	//Camera baud rate is 38400
	Serial1.begin(38400);
	
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
	
	delay(3);
        Serial.print("JPEGCameraClass::sendCommand1:   ");
	while(Serial1.available() > 0) {
	    ibuf = Serial1.read();
	    } // waiting for data in the serial buffer

	//Send each character in the command string to the camera through the camera serial port
	for(int i=0; i<wlen; i++){
		Serial1.write(*command++);
                Serial.print(*command++,HEX);
	}
       Serial.print("     JPEGCameraClass::sendCommand2   ");

	//Get the response from the camera and add it to the response string
	for(int i=0; i<rlen; i++)
	{
		unsigned long start = millis();
		while((Serial1.available() == 0)  &&  (millis() - start < 10000)); // waiting for data in the serial buffer for 1s max
		ibuf = Serial1.read();
                Serial.print("     JPEGCameraClass::sendCommand3:   ");
               Serial.print(ibuf);

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
	Serial1.flush();
	
	//Send the command to get read_size bytes of data from the current address
	for(int i=0; i<5; i++) Serial1.write(GET_SIZE[i]);


	//Read the response header.
	for(int i=0; i<7; i++){
		while(Serial1.available() == 0);
		ibuf = Serial1.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;;
	}
	
	if (uint8Compare(response,GET_SIZE_OK,7) != 0) return -2;   
	
	//Now read the actual data and add it to the response string.
    for(int j=0; j<2; j++){
		while(Serial1.available() == 0);
		ibuf = Serial1.read();
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
	//Serial1.flush();

	ret = sendCommand(RESET_CAMERA, response, 4, 4);
	if (ret != 0) return -1;
	if (uint8Compare(response,RESET_CAMERA_OK,4) != 0) return -2;   
  
    // Wait for Init end
    for (int k = 0; k < strlen(PWR_ON_MSG); k++) {
          int cnt = 0;

          do {
		    while(Serial1.available() == 0); // waiting for data in the serial buffer
		    ibuf = Serial1.read();
		    if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		    Serial.print(ibuf,BYTE);    // display the init message
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
	Serial1.flush();
	
	//Send the command to compress according the ratio
	for(int i=0; i<8; i++)Serial1.write(COMPRESS[i]);
	
	Serial1.write((uint8_t)ratio);
	
	//Get the response from the camera and add it to the response string. Always 5 bytes for basic commands
	for(int i=0; i<5; i++)
	{
		while(Serial1.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial1.read();
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
	for(int i=0; i<8; i++)Serial1.write(READ_DATA[i]);
	
	Serial1.write((uint8_t)(address>>8));
	Serial1.write((uint8_t)address);
	Serial1.write((uint8_t)0x00);
	Serial1.write((uint8_t)0x00);
	Serial1.write((uint8_t)(read_size>>8));
	Serial1.write((uint8_t)read_size);
	Serial1.write((uint8_t)0x00);
	Serial1.write((uint8_t)0x0A);	// delay between 2 frames read must be = 0x0A*0.01 ms = 0.1 ms

	//Read the response header.
	for(int i=0; i<5; i++)
	{
		while(Serial1.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial1.read();
		if (ibuf == -1) return -10; // serial buffer empty, should not happen as we wait before
		response[i] = (uint8_t)ibuf;
	}

	if (uint8Compare(response,READ_DATA_OK,5) != 0)  return -2;
	    	
	//Now read the actual data.
	icount=0;
	j= 0;
	while(j < read_size)
	{
		while(Serial1.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial1.read();
		if (ibuf == -1) return -100; // serial buffer empty, should not happen as we wait before
	        buf[j] = (uint8_t)ibuf;
	        if((j > 1) && (buf[j-1] == (uint8_t)0xFF) && (buf[j] == (uint8_t)0xD9))end_frame = 1; //end of frame detected
	        if (end_frame == 0) icount+=1;
	        j+=1;     
	}
	
	//Read the response footer.
	for(int i=0; i<5; i++)
	{
		while(Serial1.available() == 0); // waiting for data in the serial buffer
		ibuf = Serial1.read();
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
 
      
        if(eof==1) break;   
  
        //Increment the current address by the number of bytes we read
        address+=count;                     
  }// while

  //Stop taking picture
  ret=stopPictures();
  if (ret != SUCCESS ) return CAMERA_ERROR;
  
  
  return SUCCESS;
}

//Create an instance of the camera
JPEGCameraClass JPEGCamera; 

//Create a character array to store the cameras data
uint8_t  buf[32];
//return code
int ret=0;
//Size will be set to the size of the jpeg image.
int size=1024;
//This will keep track of the data address being read from the camera
long int address=0;
//eof is a flag for the sketch to determine when the end of a file is detected
//while reading the file data from the camera.
int eof=0;



void setup()
{

  int count;

  Serial.begin(9600);
 
  ret=JPEGCamera.begin();
  Serial.print ("begin, ret:");
  Serial.print (ret);
  Serial.println ();

  
  //Take a picture
  ret=JPEGCamera.takePicture();
  Serial.print ("takePicture, ret:");
  Serial.print (ret);
  Serial.println ();

     
  //Get the size of the picture    
  ret=JPEGCamera.getSize(&size);
  Serial.print ("getSize, ret:");
  Serial.print (ret);
  Serial.print (" , size:");
  Serial.print (size);
  Serial.println ();

 
  //Starting at address 0, keep reading data until we've read 'size' data
  while(address < size)
  {
        
        //Read the data starting at the current address.
        ret=JPEGCamera.readData(address, buf, &count, &eof);
        if (ret == 0) { 
          if(eof==1)break;   
  
          //Increment the current address by the number of bytes we read
          address+=count;
        } 
        else 
        { 
          Serial.print ("\nret:");
          Serial.print (ret);
          Serial.println ();
        }
                       
  }

  //Stop taking picture
  ret=JPEGCamera.stopPictures();
  Serial.print ("stopPictures, ret:");
  Serial.print (ret);
  Serial.println ();
  
 
}

  

void loop()
{

  

}




