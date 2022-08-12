#ifndef MATRIX_H
#define MATRIX_H

class MATRIX
	{
	public:
		int rows;
		int cols;
		double **matrix;
	public:
		MATRIX(int,int);
		~MATRIX();
	};

class VECTOR 
	{
	public:
		int rows;
		double *vector;
	public:
		VECTOR(int);
		~VECTOR();
	};


int 	matrix_mul(MATRIX *,MATRIX *,MATRIX *);
int 	matrix_add(MATRIX *,MATRIX *,MATRIX *);
int 	matrix_vector_mul(MATRIX *,VECTOR *,VECTOR *);
int 	vector_add(VECTOR *,VECTOR *,VECTOR *);
MATRIX	*read_matrix(Memory_File *);
VECTOR	*read_vector(Memory_File *);

#endif
