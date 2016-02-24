/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */

#ifndef Matrix_h
#define Matrix_h

#include "MPU_types.h"

/**
 *	on multiplie de matrices carrés d'ordre 3
 *	résultat dans M
 */
void Matrix_Multiply(dMatrix_t A, dMatrix_t B, dMatrix_t M);

/**
 * on multiplie une matrice de rotation par un vecteur et on le retourne dans Vprime
 */
void Vector_Apply_Matrix(dMatrix_t M, dVector_t V, dVector_t Vprime);

/**
 * On calcule la transposé de la matrice M
 */
void Matrix_Transpose(dMatrix_t M, dMatrix_t Out);

/**
 * Copy Matrix M to Out
 */
void Matrix_Copy(dMatrix_t M, dMatrix_t Out);

/**
 * On calcule la trace de la matrice de rotation
 */
double Matrix_Trace(dMatrix_t M);

/**
 * on multiplie une matrice par une scalaire
 */
void Matrix_Scale(dMatrix_t M, double scalar);

/**
 * on affiche une matrice (double ou int)
 */
void Matrix_Display(dMatrix_t M);


#endif