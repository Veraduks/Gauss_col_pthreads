#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.h"

#define arg 10 //максимальное число строк/столбцов матрицы, выводимое в консоли

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0)
    {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    printf("Время работы = %lld.%.5ld\n", (long long)temp.tv_sec, temp.tv_nsec);
    return temp;
}

static double f(int i, int j)
{
	return 1.0 / (i + j + 1.0);
}

//изготовление нехорошей матрицы
void GenerateBadMatrix(FILE *file, int n)
{
int i, j;
	
for (i = 0; i<n; i++)
{
    for (j=0; j<n; j++)
    {
    	fprintf(file, "%lf ", f(i, j));
    }
}
}

//изготовление случайной матрицы
void GenerateRandMatrix(FILE *file, int n)
{
int i;
double k;/*
        for (k = first_row; k < last_row; k++)
                for (i = n - 1; i >= 0; i--)
                {
                        tmp = x[i * n + k];
                        for (j = i + 1; j < n; j++)
                                tmp -= a[i * n + j] * x[j * n + k];
                        x[i * n + k] = tmp;
                }
*/
for (i = 0; i<n*n; i++) {
	k = rand()%10;
    fprintf (file, "%lf ", k);
    }
}

//чтение матрицы из файла
int MatrixInput(int n, double* matrix, int mode, char* f)
{
int i, j;
double a = 6;
FILE *file;
switch (mode)
{
case 1: //если матрица лежит в файле "input.txt"

	file = fopen("text.txt", "r");
	if (!file) return -1;
    for (i = 0; i<n; i++)
    {
        for (j=0; j<n; j++)
        {
           if (fscanf(file, "%lf", &matrix[i * n + j]) != 1)
					return -8;
        }
    }
	return 1;
	break;

case 0: //если выбрали тип генерации матрицы
file = fopen("input.txt", "w");
if (strcmp(f, "BadMatrix") == 0)
    GenerateBadMatrix(file, n);
if (strcmp(f, "RandMatrix") == 0)
    GenerateRandMatrix(file, n);
if (strncmp(f, "f_1_1", 32) == 0){ 
            for (i = 0; i < n; ++i){
                for (j = 0; j < n; ++j){
                    if (i == j)
                    fprintf (file, "%d ", 1);
                    if ((j > i)||(j == n))
					fprintf (file, "%d ", -1);
                    if (j < i)
 					fprintf (file, "%d ", 0);
                }
				fprintf(file, "\n");
            }
  }
if (strcmp(f, "f_1_2")==0){ //Клетка Жордана
            for (i = 0; i < n; ++i){
                for (j = 0; j < n; ++j){
                    if (i == j)
                   	fprintf (file, "%d ", 1);
                    else if (j == i+1)
                       fprintf (file, "%lf ", a);
                    else  fprintf (file, "%d ", 0);
                }
				fprintf(file, "\n");
            }
        }
 if (strcmp(f, "f_1_9") == 0){
            for (i = 0; i < n; ++i){
                for (j = 0; j < n; j++){
                    if (j >= i)
                         fprintf (file, "%d ", n-j);
                    if (i >= j)
                        fprintf (file, "%d ", n-i);
                }
				fprintf(file, "\n");
            }
        }
if (strcmp(f, "f_1_18") == 0)	
{
	for (i = 0; i < n; i++){
       for (j = 0; j < n; j++){
    if (i == 0 && j == 0)
	fprintf (file, "%lf ", 1-1.0/(n-1));
	else
    if (i == n-1 && j == n-1) 
		fprintf (file, "%lf ", 1-1.0/(n-1));
	else
    if (i == 0 && j == n-1) 
		fprintf (file, "%lf ", -1.0/(n-1));
	else
    if (i == n-1 && j == 0) 
		fprintf (file, "%lf ", -1.0/(n-1));
	else
    if (i == j)
		fprintf (file, "%lf ", 2.0);
	else
    if (i == j - 1 || j == i - 1) fprintf (file, "%lf ", -1.0); 
	
	else fprintf (file, "%d ", 0);
	
	}
		fprintf(file, "\n");
   }
}
if (strcmp(f, "f_1_20") == 0){
            for (i = 0; i < n; ++i){
                for (j = 0; j < n; ++j){
                    if (i+j+1 < n)
                         fprintf (file, "%lf ", a+1);
                    if ((i+j+1 >= n)&&((i+1)*(j+1) != n*n))
                        fprintf (file, "%lf ", a);
					if ((i+1)*(j+1) == n*n)
					fprintf (file, "%lf ", a-1);
                }
				fprintf(file, "\n");
            }
        }
if (strcmp(f, "f_1_22") == 0){
 for (i = 0; i < n; ++i){
                for (j = 0; j < n; ++j){
if (i == j && i != n-1) fprintf (file, "%lf ", 1.0); else
    if (i == j && i == n-1) fprintf (file, "%d ", n); else
    if (i == n-1) fprintf (file, "%d ", j+1); else
    if (j == n-1) fprintf (file, "%d ", i+1); else
	fprintf (file, "%d ", 0);
	}
		fprintf(file, "\n");
         }
     }
	
fclose(file);
 file = fopen("input.txt", "r");
	for (i = 0; i<n; i++)
    {
        for (j=0; j<n; j++)
        {
           if (fscanf(file, "%lf", &matrix[i * n + j]) != 1)
					return -10;
        }
    }
	fclose(file);
	break;
case -1: //если ничего не выбрали

	printf("Нечего считать - ни матрицы, ни файла, попробуйте ещё раз! \n");
	return -1;
	break;

}
return 2;
}

//вывод матрицы на экран
void MatrixOutput(int n, double* matrix)
{
    int i, j;
	
    //printf ("Matrix: \n");
  if (n<=arg)
  {
    for (i=0; i<n; i++)
    {
        for (j=0; j<n; j++)
            printf("%10.3g ", matrix[i*n + j]);
        printf("\n");
    }
 } else {
	    //распечатаем первый квадрат матрицы размером arg*arg
       for (i=0; i<arg; i++) {
	  	for (j=0; j<arg; j++)
            printf("%10.3g ", matrix[i*n + j]);
		printf("... ");
		printf("%10.3g ", matrix[i*n + n-1]);
        printf("\n");
		}
	 //распечатаем точки
      for (i = 0; i<30*2+1; i++) printf (".");
	   printf (" \n");
	 //печатаем последнюю строку
       for (j=0; j<arg; j++)
           printf("%10.3g ", matrix[n*n - n + j]);
       printf("... ");
       printf("%10.3g ", matrix[n*n-1]);
      }
	  //printf (" \n");
	  
}

//перестановка двух строк i и k
void SwapRows(int n, double* a, int i, int k) {
	double tmp;
	int j;
	for (j = 0; j < n; ++j) 
		{
			tmp = a[i * n + j];
			a[i * n + j] = a[k * n + j];
			a[k * n + j] = tmp;
		}
}

// перестановка двух столбцов i и k
void SwapColumns(int n, double* a, int i, int k) {
double tmp;
int j;
	for (j = 0; j < n; ++j) 
	{   
		tmp = a[i*n + j];
		a[i * n + j] = a[k * n + j];
        a[k * n + j] = tmp;
	}
}

// умножение строки i на tmp в матрицах a и x, причём у "x" начиная с 1 столбца, у "a" - с i-го
void DivideRow(int n, double* a, double* x, int i, double tmp) {
int j;
		for (j = i; j < n; ++j)
			a[i * n + j] *= tmp;
		for (j = 0; j < n; ++j)
			x[i * n + j] *= tmp;
}

// вычитание из всех строк ниже row, строки row, умноженной на 1 / matrix[i][row]
void SubRows(int n, double* matrix, int row) {
double value = 0;
int i, j;
	for (i = row + 1; i < n; ++i) {
                value = 1./matrix[i*n + row];

		for (j = row; j < n; ++j)
                        matrix[i*n + j - 1] -= matrix[row + j] * value;
	}
}

void Division(int n, double* a, double* x, int i, int first_row, int last_row)
{
int k, j;
double tmp;
		for (j = first_row; j < last_row; j++)
		{
			tmp = a[j * n + i];
			for (k = i; k < n; k++)
				a[j * n + k] -= tmp * a[i * n + k];
			for (k = 0; k < n; k++)
				x[j * n + k] -= tmp * x[i * n + k];
		}
}
