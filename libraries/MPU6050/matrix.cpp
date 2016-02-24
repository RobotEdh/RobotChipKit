/**
 * @author : Dietrich Lucas (Kestrel)
 * @email : lucas.dietrich.pro@gmail.com
 * @website http://www.kestrel.fr
 */
#include "WProgram.h"
#include "matrix.h"

dMatrix_t IdentityMatrix = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

// A * B = M
void Matrix_Multiply(dMatrix_t A, dMatrix_t B, dMatrix_t M)
{
  double op;

  // pour chaque ligne
  for(uint8_t line = 0; line < 3; line++)
  {
	  // pour chaque colonnes
    for(uint8_t column = 0; column < 3; column++)
    {
      op = 0.0;

      // somme de 3 produits
      for(uint8_t w = 0; w < 3; w++)
      {
    	  op += A[line][w] * B[w][column];
      }

      // output matrix
      M[line][column] = op;
    }
  }
}

// M * scalar
void Matrix_Scale(dMatrix_t M, double scalar)
{
	for(uint8_t line = 0; line < 3; line++)
	{
		for(uint8_t column = 0; column < 3; column++)
		{
			M[line][column] *= scalar;
		}
	}
}

// transposée
void Matrix_Transpose(dMatrix_t M, dMatrix_t Out)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		for (uint8_t j = 0; j < 3; j++)
		{
			Out[j][i] = M[i][j];
		}
	}
}

// copy
void Matrix_Copy(dMatrix_t M, dMatrix_t Out)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		for (uint8_t j = 0; j < 3; j++)
		{
			Out[j][i] = M[j][i];
		}
	}
}

// calcule la trace de la matrice M
double Matrix_Trace(dMatrix_t M)
{
	return M[0][0] + M[1][1] + M[2][2];
}

// Vprime = M * V
void Vector_Apply_Matrix(dMatrix_t M, dVector_t V, dVector_t Vprime)
{
	for(uint8_t line = 0; line < 3; line++)
	{
		double op = 0.0;

		for(uint8_t w = 0; w < 3; w++)
		{
			op += M[line][w] * V[w];
		}

		Vprime[line] = op;
	}
}


// Serial.print(M) 
void Matrix_Display(dMatrix_t M)
{
	Serial.print("Matrix (3, 3) : ");
	for(uint8_t i = 0; i < 3; i++)
	{
		Serial.print("\n");
		for(uint8_t j = 0; j < 3; j++)
		{
			Serial.print("\t");
			Serial.print(M[i][j]);
		}
	}
	Serial.println("");
}

