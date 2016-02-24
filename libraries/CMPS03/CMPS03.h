/*
  CMPS03.h - Library for commmunicationg with compass CMPS03
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef CMPS03_h
#define CMPS03_h


/* Compass                                                              */
/* I2C interface is provided on pins:                                   */
/*         1 = Power +5V                                                */
/*         2 = SCL connected to J3-09 (SCL2/RA2)                        */
/*         3 = SDA connected to J3-11 (SDA2/RA3)                        */
/*         9 = Ground                                                   */

#define CMPS03_ADDRESS 0x60 //1100000: compass address 0XC0 on 7 bits for I2c protocol


class CMPS03Class
{
  private:
     
  public:
    
  CMPS03Class();
  
  void CMPS03_init (void); 
 /* initialize I2C, must NOT be called if I2C is already initialized           */                                            
 /* input:       none                                                          */
 /* output:      none                                                          */                             
 /* lib:         Wire.begin                                                    */                       

  int CMPS03_revision (void);
 /* return Software Revision Number                                            */                                            
 /* input:       none                                                          */
 /* output:      return                                                        */
 /*                  = -1: length to long for buffer                           */
 /*                  = -2: address send, NACK received -> bad address          */
 /*                  = -3: data send, NACK received -> bad register            */
 /*                  = -4: other error (lost bus arbitration, bus error, ..    */
 /*                       -> missing 1Ok pull-down resistor on SDA & SDL pins  */
 /*                  = Software Revision Number otherwise                      */                           
 /* lib:         Wire.beginTransmission                                        */ 
 /*              Wire.send                                                     */ 
 /*              Wire.endTransmission                                          */ 
 /*              delay                                                         */ 
 /*              Wire.requestFrom                                              */
 /*              Wire.receive                                                  */
  
  int CMPS03_read (void);
 /* return 0-254 for a full circle, 0: North                                   */                                            
 /* input:       none                                                          */
 /* output:      return                                                        */
 /*                  = -1: length to long for buffer                           */
 /*                  = -2: address send, NACK received -> bad address          */
 /*                  = -3: data send, NACK received -> bad register            */
 /*                  = -4: other error (lost bus arbitration, bus error, ..    */
 /*                       -> missing 1Ok pull-down resistor on SDA & SDL pins  */
 /*                  = 0-254 for a full circle, 0: North  otherwise            */                           
 /* lib:         Wire.beginTransmission                                        */ 
 /*              Wire.send                                                     */ 
 /*              Wire.endTransmission                                          */ 
 /*              delay                                                         */ 
 /*              Wire.requestFrom                                              */
 /*              Wire.receive                                                  */  
};

#endif
