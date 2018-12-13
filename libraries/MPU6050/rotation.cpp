/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

#include "Arduino.h"
#include "rotation.h"
#include "matrix.h"

dMatrix_t RotMat;

void compute_rotation_matrix(double gamma_rad, double beta_rad, double alpha_rad)
{
	// A roll (roulis) is a counterclockwise rotation of gamma about the x-axis. The rotation matrix Rx(gamma) is given by
	double cos_gamma = cos(gamma_rad); // optimisations
	double sin_gamma = sin(gamma_rad);

	dMatrix_t Mgamma = {
			{1.0,   0.0,            0.0     },
			{0.0,   cos_gamma,    -sin_gamma},
			{0.0,   sin_gamma,     cos_gamma}
	};

	// A pitch (tangage) is a counterclockwise rotation beta of about the y-axis. The rotation matrix Ry(beta) is given by
	double cos_beta = cos(beta_rad);
	double sin_beta = sin(beta_rad);

	dMatrix_t Mbeta = {
			{ cos_beta,    0.0,    sin_beta},
			{0.0,          1.0,    0.0     },
			{-sin_beta,    0.0,    cos_beta}
	};

	// A yaw (laçet) is a counterclockwise rotation of alpha about the z-axis. The rotation matrix Rz(alpha) is given by:
	double cos_alpha = cos(alpha_rad);
	double sin_alpha = sin(alpha_rad);

	dMatrix_t Malpha = {
			{cos_alpha,   -sin_alpha,   0.0},
			{sin_alpha,    cos_alpha,   0.0},
			{0.0,          0.0,         1.0}
	};


	//Compute Rotation matrix RotMat = Mgamma * Mbeta * Malpha
	dMatrix_t Mtemp;
	Matrix_Multiply(Mgamma, Mbeta, Mtemp); Matrix_Multiply(Mtemp, Malpha, RotMat);
}

