/*
  TMP102.h - Library for commmunicationg with temperature sensor TMP102
  Created by EDH, June 12, 2012.
  Released into the public domain.
*/


#ifndef TMP102_h
#define TMP102_h


#define TMP102_ADDRESS 0x48 //1001000: temperature sensor address 0X90 on 7 bits for I2c protocol


class TMP102Class
{
  private:
     
  public:
    
  TMP102Class();
  
  void TMP102_init (void); 
 /* initialize I2C, must NOT be called if I2C is already initialized           */                                            
 /* input:       none                                                          */
 /* output:      none                                                          */                             
 /* lib:         Wire.begin                                                    */                                

  double TMP102_read (void);
 /* return temperature in Celsius                                              */                                            
 /* input:       none                                                          */
 /* output:      return                                                        */
 /*                  = -1: length to long for buffer                           */
 /*                  = -2: address send, NACK received -> bad address          */
 /*                  = -3: data send, NACK received -> bad register            */
 /*                  = -4: other error (lost bus arbitration, bus error, ..    */
 /*                       -> missing 1Ok pull-down resistor on SDA & SDL pins  */
 /*                  = temperature in Celsius otherwise                        */                           
 /* lib:         Wire.beginTransmission                                        */ 
 /*              Wire.send                                                     */ 
 /*              Wire.endTransmission                                          */ 
 /*              delay                                                         */ 
 /*              Wire.requestFrom                                              */
 /*              Wire.receive                                                  */  
};

#endif
