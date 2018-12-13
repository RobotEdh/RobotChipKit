/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

#include "Arduino.h"
#include "MPU_math.h"


/**
 * Low Pass Filter : Remove high-frequencies (Smooth)
 * @param a : coeff
 * @param In : input value
 * @param *S : Last Smoothed value
 */
double LowPassFilter(double a, double In, double S)
{
	return a * In + (1.0 - a) * S;
}

/**
 * High Pass Filter : Remove low-frequencies
 * @param a : coeff
 * @param In : input value
 * @param LIn : last value
 * @param *S : Last Filtered value
 */
double HighPassFilter(double a, double In, double LIn, double S)
{
	return ((a * S) + (a * (In - LIn)));
}


void BetaGammaFromCosinDirect(dVector_t V, double *gamma_rad, double *beta_rad)
{
	*gamma_rad  = atan2(-V[Y], V[Z]); 	
	*beta_rad   = atan2( V[X], hypot(V[Y],V[Z])); 	
}
