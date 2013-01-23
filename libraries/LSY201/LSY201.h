/*
  LSY201.h - Library for commmunicating with JPEG serial camera LYS201
  Created by EDH, July 26, 2012.
  Thanks to Ryan Owens
  Released into the public domain.
*/

#ifndef LSY201_h
#define LSY201_h

#include <inttypes.h>

class JPEGCameraClass
{
	public:
		JPEGCameraClass();
		
		int begin(void);
		int reset(void);
		int getSize(int * size);
		int takePicture(void);
		int stopPictures(void);
		int readData(long int address, uint8_t *buf, int *count, int * eof);
		int compress( int ratio);
		int imageSizeSmall(void);
		int imageSizeMedium(void);
	    int imageSizeLarge(void);
		int imageSizeSmallSave(void);
		int imageSizeMediumSave(void);
		int imageSizeLargeSave(void);		
	private:
		int sendCommand(const uint8_t *command, uint8_t *response, int wlen, int rlen);
		int uint8Compare(const uint8_t *a1, const uint8_t *a2, int len);
};

#endif