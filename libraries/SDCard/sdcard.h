#ifndef SDCARD_h
#define SDCARD_h

#include <SPI.h>
#include <SD.h>   

#define SUCCESS 0
#define SDCARD_ERROR -7
#define FILE_OPEN_ERROR  -1000
#define FILE_CLOSE_ERROR -1001

#define SS_CS_Pin  8   // Serial Select pin connected to digital pin J6-09 (RF1)
/* SD-Card                                                                                          */
/* SPI interface is provided on pins:                                                               */                    
/*         SPI/SD-Card Master Output (MOSI) connected to J5-07(SDO2/PMA3/CN10/RG8) selected by JP5  */
/*         SPI/SD-Card Master Input  (MISO) connected to J5-09(SDI2/PMA5/CN8/RG7) selected by JP7   */

int initSDCard(void);
/* Description: Initialization of the SD-Card                                 */                                            
/* input:       none                                                          */                      
/* output:      return                                                        */                            
/*                  = SDCARD_ERROR if error during initialization SD-Card     */
/*                  = SUCCESS otherwise                                       */ 
/* lib:         card.init                                                     */
/*              volume.init                                                   */
/*              root.openRoot                                                 */
        
int infoSDCard(void);
/* Description: Get infos from the SD-Card                                    */                                            
/* input:       none                                                          */                      
/* output:      return                                                        */                            
/*                  = SDCARD_ERROR if error during reading CID of the SD-Card */
/*                  = SUCCESS otherwise                                       */ 
/* lib:         card.readCID                                                  */
/*              root.ls                                                       */      

#endif 