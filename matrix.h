#ifndef __MATRIX_H_INCLUDED__
#define __MATRIX_H_INCLUDED__

int MatrixInput(int n, double* matrix, int mode, char* f);

void MatrixOutput(int n, double *matrix);

void GenerateBadMatrix(FILE *file, int n);

void GenerateRandMatrix(FILE *file, int n);

void SwapRows(int n, double *matrix, int i1, int i2);

void SwapColumns(int n, double *matrix, int j1, int j2);

void DivideRow(int n, double* a, double* x, int i, double tmp);

void SubRows(int n, double *matrix, int row);

void Division(int n, double* a, double* x, int i, int first_row, int last_row);

struct timespec diff(struct timespec start, struct timespec end);

#endif /* not __MATRIX_H_INCLUDED__ */
