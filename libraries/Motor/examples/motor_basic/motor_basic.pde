
#include <motor.h>




  
void setup()
{ 
  Serial.begin(9600); // initialize serial port
  // H bridge setup
  pinMode(InMotorRight1Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorRight1Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(InMotorRight2Pin, OUTPUT);      // set the pin as output
  pinMode(EnableMotorRight2Pin, OUTPUT);  // set the analogig pin as output for PWM
  pinMode(InMotorLeft1Pin, OUTPUT);       // set the pin as output
  pinMode(EnableMotorLeft1Pin, OUTPUT);   // set the analogig pin as output for PWM
  pinMode(InMotorLeft2Pin, OUTPUT);       // set the pin as output
  pinMode(EnableMotorLeft2Pin, OUTPUT);   // set the analogig pin as output for PWM
  
  Serial.println("H bridge setup done");  


}


void loop()
{

  Serial.println(" --> start_forward motor 1"); 
  digitalWrite(InMotorRight1Pin, HIGH); 
  analogWrite(EnableMotorRight1Pin, SPEEDNOMINAL);
  delay(5000); //make it readable 
   
  Serial.println(" --> stop");  
  analogWrite(EnableMotorRight1Pin, 0);
  delay(5000); //make it readable 


  Serial.println(" --> start_backward motor 1"); 
  digitalWrite(InMotorRight1Pin, LOW); 
  analogWrite(EnableMotorRight1Pin, SPEEDNOMINAL); 
  delay(5000); //make it readable 

  Serial.println(" --> stop");  
  analogWrite(EnableMotorRight1Pin, 0);;
  delay(5000); //make it readable 
  
  
  Serial.println(" --> start_forward motor 2"); 
  digitalWrite(InMotorRight2Pin, HIGH); 
  analogWrite(EnableMotorRight2Pin, SPEEDNOMINAL);
  delay(5000); //make it readable 
   
  Serial.println(" --> stop");  
  analogWrite(EnableMotorRight2Pin, 0);
  delay(5000); //make it readable 


  Serial.println(" --> start_backward motor 2"); 
  digitalWrite(InMotorRight2Pin, LOW); 
  analogWrite(EnableMotorRight2Pin, SPEEDNOMINAL); 
  delay(5000); //make it readable 

  Serial.println(" --> stop");  
  analogWrite(EnableMotorRight2Pin, 0);;
  delay(5000); //make it readable 
  
  
    Serial.println(" --> start_forward motor 3"); 
  digitalWrite(InMotorLeft1Pin, HIGH); 
  analogWrite(EnableMotorLeft1Pin, SPEEDNOMINAL);
  delay(5000); //make it readable 
   
  Serial.println(" --> stop");  
  analogWrite(EnableMotorLeft1Pin, 0);
  delay(5000); //make it readable 


  Serial.println(" --> start_backward motor 3"); 
  digitalWrite(InMotorLeft1Pin, LOW); 
  analogWrite(EnableMotorLeft1Pin, SPEEDNOMINAL); 
  delay(5000); //make it readable 

  Serial.println(" --> stop");  
  analogWrite(EnableMotorLeft1Pin, 0);;
  delay(5000); //make it readable 
  
  
  
  Serial.println(" --> start_forward motor 4"); 
  digitalWrite(InMotorLeft2Pin, HIGH); 
  analogWrite(EnableMotorLeft2Pin, SPEEDNOMINAL);
  delay(5000); //make it readable 
   
  Serial.println(" --> stop");  
  analogWrite(EnableMotorLeft2Pin, 0);
  delay(5000); //make it readable 


  Serial.println(" --> start_backward motor 4"); 
  digitalWrite(InMotorLeft2Pin, LOW); 
  analogWrite(EnableMotorLeft2Pin, SPEEDNOMINAL); 
  delay(5000); //make it readable 

  Serial.println(" --> stop");  
  analogWrite(EnableMotorLeft2Pin, 0);;
  delay(5000); //make it readable 
  

}

