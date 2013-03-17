#ifndef TILTPAN_h
#define TILTPAN_h

#include <inttypes.h> // used for uint8_t type

void TiltPan_begin(int Hpin, int Vpin);    
/* Description: initialize everything, must be called during setup            */                                            
/* input:       Hpin                                                          */
/*                  = Horizontal servo                                        */
/*              Vpin                                                          */
/*                  = Vertical servo                                          */
/* output:      none                                                          */                             
/* lib:         Servo.attach                                                  */                                
/*              Servo.write                                                   */                                 
/*              delay                                                         */                                


void TiltPan_move(uint8_t HPos, uint8_t VPos);
/* Description: move the Tilt&Pan depending on the                            */
/*              Horizontal and Vertical position                              */
/* input:       HPos                                                          */ 
/*                  = Horizontal position to move the Tilt&Pan (0< HPos <180) */
/*              VPos                                                          */ 
/*                  = Vertical position to move the Tilt&Pan  (0< VPos <180)  */      
/* output:      none                                                          */                            
/* lib:         Servo.write                                                   */                                                              
/*              delay                                                         */

#endif