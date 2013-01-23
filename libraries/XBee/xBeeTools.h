/*
  xBeeTools.h - Tools class fro Xbee
  Copyright (c) 2013 EDH.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef xBeeTools_h
#define xBeeTools_h

#ifndef _SYS_INTTYPES_H_
	#include <inttypes.h>
#endif
#ifndef	_STDIO_H_
	#include <stdio.h> // for size_t
#endif

#include "WString.h"

#define PRINT_DEC 10
#define PRINT_HEX 16
#define PRINT_OCT 8
#define PRINT_BIN 2
#define PRINT_BYTE 0

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0


#define BUFFER_SIZE 256

#define SUCCESS 0
#define UNEXPECTED_APIID -1
#define STATUS_ERROR -2
#define RESPONSE_ERROR -3
#define NO_RESPONSE -4

#define PAYLOAD_SIZE 100


class xBeeTools
{
    public:
	char s_buffer[BUFFER_SIZE];	//buffer used to send data
	int xBTprintNumber(long, uint8_t);
	int xBTprintFloat(double, uint8_t);
	int xBTsendbufferXbee(char *buf, unsigned int buf_len);
	int xBTsendXbee(char* msg,  unsigned int msg_len);
	int xBTreceiveXbee(char * msg, int timeout);
	int xBTprint(const char *str, int size);
	int xBTprint(const uint8_t *buffer, size_t size);
    int xBTprint(const String &);
	int xBTprint(char, int = PRINT_BYTE);
	int xBTprint(unsigned char, int = PRINT_BYTE);
	int xBTprint(int, int = PRINT_DEC);
	int xBTprint(unsigned int, int = PRINT_DEC);
	int xBTprint(long, int = PRINT_DEC);
	int xBTprint(unsigned long, int = PRINT_DEC);
	int xBTprint(double, int = 2);
};



#endif
