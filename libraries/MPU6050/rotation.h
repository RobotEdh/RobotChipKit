/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

#ifndef Rotation_h
#define Rotation_h

#include "MPU_types.h"

/**
 * Met à jour la matrice de rotation avec les 3 angles de rotations issues des gyroscopes
 * pitch > roll > yaw
 */
void compute_rotation_matrix(double phi, double theta, double psi);

#endif