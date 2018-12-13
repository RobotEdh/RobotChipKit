#include <LSY201.h>
#include <SD.h>
#include <sdcard.h>

//Create an instance of the SD file
File myFile;

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
  if (!SD.begin(8)) Serial.println("\n\n\ninitialization SD card failed!"); else Serial.println("\n\n\ninitialization card OK!");
  myFile = SD.open("test63.jpg", FILE_WRITE);
 
  ret=JPEGCamera.begin();
  Serial.print ("Camera begin, ret:");
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
          for(int i=0; i<count; i++)
	  {  
                myFile.write(buf[i]);
          }          
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
   
  myFile.close();
  Serial.print ("close file ok ");
  Serial.println ();
  
 
}

  

void loop()
{

  

}




