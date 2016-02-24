/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

#include "imu.h"
#include "MPU_math.h"
#include "matrix.h"
#include "rotation.h"


extern MPU6050 MPU6050;             // The accel+gyro class

dVector_t Omega;	// angular velocity			(rad.s^-1)
dVector_t Accel;	// acceleration				(m.s^-2)
dVector_t SmoothedAccel;	// filtered acceleration	(m.s^-2)

double LowPassFilterVal = ACCEL_LOWPASSFILTER_VAL;	// valeur du coefficient du HPF
double qfilter          = COMPLEMENTARY_FILTER_VAL; // filtre d'utilisation des gyroscopes

// Variation angles from gyro
double VarGyro_gamma;	// roll (roulis)	(rad)
double VarGyro_beta;	// pitch (tanguage) (rad)
double VarGyro_alpha;	// yaw (lacet)		(rad)

// angles from accel
double Accel_gamma;	 // roll (roulis)	(rad)
double Accel_beta;	 // pitch (tanguage) (rad)
double Accel_alpha;	 // yaw (lacet)		(rad)

// angles computed
double gamma_result;	// roll (roulis)	(rad)
double beta_result;	    // pitch (tanguage) (rad)
double alpha_result;	// yaw (lacet)		(rad)



static uint32_t previousTime = 0, i = 0;

void IMUInit()
{
  int ret =0;
  
  Serial.println("Start init MPU6050");
 
  // initialize the MPU6050
  ret=MPU6050.initialize();
  if (ret != 0)
  {  
        Serial.print("Error Init MPU6050, error: ");
        Serial.println(ret);
  }        
  else
  {
        Serial.println("Init MPU6050 OK");
  }
  Serial.println(MPU6050.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  int16_t x=MPU6050.getAccelerationX();
  MPU6050.calibAcceleration();
  MPU6050.calibRotation();

}


void IMUExecution()
{

	uint32_t currentTime, dt=0;
    
    // read data from MPU
    MPU6050.getRotation_degree_sec(&Omega[X], &Omega[Y], &Omega[Z]); // gyroscope data in deg/sec
    MPU6050.getAcceleration_g(&Accel[X], &Accel[Y], &Accel[Z]);      // accelerometer in g normalized
   
    // Integrate gyro data
    currentTime = millis();
    if (previousTime > 0) dt = currentTime - previousTime;  // in ms
    previousTime = currentTime;
    if (dt == 0) return;
	
	if (i>10) {
//	   Serial.print("dt: ");Serial.print(dt);
//	   Serial.print(" - Omega[X]: ");Serial.print(Omega[X]);
//	   Serial.print(" - Omega[Y]: ");Serial.print(Omega[Y]);
//	   Serial.print(" - Omega[Z]: ");Serial.println(Omega[Z]);
 	}
 	
	VarGyro_gamma = (Omega[X] * PI * dt / 180000.0); // in rad
	VarGyro_beta  = (Omega[Y] * PI * dt / 180000.0);
	VarGyro_alpha = (Omega[Z] * PI * dt / 180000.0);
	
	
	// Apply a low pass filter for accel data
	for (uint8_t i = 0; i < 3; i++)
		Accel[i] = SmoothedAccel[i] = LowPassFilter(LowPassFilterVal, Accel[i], SmoothedAccel[i]);

    // Compute beta and gamma in rad from accelerometer
 	BetaGammaFromCosinDirect(Accel, &Accel_gamma, &Accel_beta);

	// Apply complementary filter
	gamma_result = (qfilter * (gamma_result + VarGyro_gamma)) + ((1 - qfilter) * Accel_gamma);
	beta_result  = (qfilter * (beta_result  + VarGyro_beta))  + ((1 - qfilter) * Accel_beta); 
	alpha_result =             alpha_result + VarGyro_alpha;

	if (i>10) {
       Serial.print("gamma_result: ");Serial.print(gamma_result*180.0/PI);
	   Serial.print(" - beta_result: ");Serial.print(beta_result*180.0/PI);
	   Serial.print("- alpha_result: ");Serial.println(alpha_result*180.0/PI);
       i=0;
    }
    i++;

}
