/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

/**
 *	Fonction mathématiques supplémentaires
 */

#ifndef MPU_math_h
#define MPU_math_h

#include "MPU_types.h"

/************************/
/****** FUNCTIONS *******/
/************************/


/**
 * Low Pass Filter : Remove high-frequencies (Smooth)
 * @param alpha : coeff
 * @param In : input value
 * @param S : Last Smoothed value
 */
double LowPassFilter(double alpha, double In, double S);

/**
 * High Pass Filter : Remove low-frequencies
 * @param alpha : coeff
 * @param In : input value
 * @param LIn : last value
 * @param S : Last Filtered value
 */
double HighPassFilter(double alpha, double In, double LIn, double S);


void BetaGammaFromCosinDirect(dVector_t g, double *gamma_rad, double *beta_rad);


#endif