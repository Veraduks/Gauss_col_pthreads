#ifndef __TASK_H_INCLUDED__
#define __TASK_H_INCLUDED__

#include <math.h>

typedef struct _ARGS{
	int n; //размер матрицы
    double *matrix; //исходная матрица
    double *invmatr; //матрица, в которой будет обратная
	double *b; //матрица-дубликат
    int rank; //номер потока
    int total_threads; //общее число потоков
	double solution_error; //норма невязки
	int *flag; //флаг проверки вырожденности
	double *max; //максимальный элемент (для каждого потока)
}ARGS;

void Reverse(int n, double* a, double* x, int first_row, int last_row);

int FindMax(int n, double* a, int indMax, double max, int i);

//double SolutionError(int n, double* a, double* x);

double SolutionError(double *a, double *x, int n, int total_threads, int rank, double *solution_eror);

//int InvertMatrix(int n, double* a, double* x);

void synchronize(int total_threads);

int Inverse(double *matrix, int n, double *invmatr, int rank, int total_threads, int *flag);

#endif /* not __TASK_H_INCLUDED__ */
