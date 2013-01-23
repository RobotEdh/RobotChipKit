/*
  CMPS03.h - Library for commmunicationg with compass CMPS03
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef CMPS03_h
#define CMPS03_h


#define CMPS03_ADDRESS 0x60 //1100000: compass address 0XC0 on 7 bits for I2c protocol


class CMPS03Class
{
  private:
     
  public:
    
  CMPS03Class();
  
  void CMPS03_begin (void);   // must be called during setup
  int CMPS03_revision (void); // return Software Revision Number
  int CMPS03_read (void);     // return 0-254 for a full circle, 0: North
};



#endif
