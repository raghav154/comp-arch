// matrix multiply program

#include <stdio.h>
#include <stdlib.h>

#define N	512

// C = A x B

void matrix_multiply (double A[N][N], double B[N][N], double C[N][N]) {
	int i, j, k;
	/* Using a register */
	register double reg=0.0;
	/* Getting transpose of B */
	/*double temp;
	for(i=0; i<N; i++) {
		for(j=0; j<N; j++) {
			if(i<j){
			temp = B[i][j];
			B[i][j] = B[j][i];
			B[j][i] = temp;
			}
		}
	}*/
		
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			reg = 0.0;
			for (k=0; k<N; k++) {
				reg += A[i][k] * B[k][j];
			}
			/* Inserting the value of the register back to the array */
			C[i][j] = reg;
		}
	}
}

// read a matrix from standard input

void read_matrix (double A[N][N]) {
	int i, j;

	for (i=0; i<N; i++) {
		double d;
		int n;
		for (j=0; j<N; j++) {
			n = scanf ("%lf", &d);
			if (n != 1) {
				fprintf (stderr, "double expected!\n");
				exit (1);
			}
			A[i][j] = d;
		}
	}
}

// print a matrix to standard output

void print_matrix (double A[N][N]) {
	int i, j;

	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) printf ("%f ", A[i][j]);
		printf ("\n");
	}
}

double X[N][N], Y[N][N], Z[N][N];

int main () {

	// read two N x N matrices

	read_matrix (X);
	read_matrix (Y);

	// multiply the matrices 

	matrix_multiply (X, Y, Z);

	// output the resulting matrix

	print_matrix (Z);
	return 0;
}
