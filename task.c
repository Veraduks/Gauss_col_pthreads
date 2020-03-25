#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>
#include <time.h>
#include <pthread.h>

#include "task.h"
#include "matrix.h"

void synchronize(int total_threads) //дождаться в текущем потоке остальных потоков (из общего числа total_threads)
{
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //объект синхронизации типа mutex
	static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER; //объект синхронизации типа condvar
	static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER; //объект синхронизации типа condvar
	static int threads_in = 0; //число пришедших в функцию задач
	static int threads_out = 0; //число ожидающих выхода из функции задач
	
	pthread_mutex_lock(&mutex); //"захватить" mutex для работы с переменными threads_in и threads_out
	
	threads_in++; //увеличить на 1 количество прибывших в эту функцию задач
	//проверяем количество прибывших задач
	if (threads_in >= total_threads) //если текущий поток пришёл последним
	{
		threads_out = 0; //устанавливаем начальное значение для threads_out
		pthread_cond_broadcast (&condvar_in); //разрешаем остальным продолжать работу
	} else //если есть ещё не пришедшие потоки
	//ожидаем, пока в эту функцию не придут все потоки
		while (threads_in < total_threads)
		//ожидаем разрешения продолжить работу: освободить mutex и ждать сигнала от condvar, затем "захватить" mutex опять
			pthread_cond_wait(&condvar_in, &mutex);
	
	threads_out++; //увеличить на 1 количество ожидающих выхода задач
	//проверяем количество прибывших задач
	if (threads_out >= total_threads)
	{
	//текущий поток пришёл в очередь последним
		threads_in = 0; //устанавливаем начальное значение для threads_in 
		pthread_cond_broadcast(&condvar_out); //разрешаем остальным продолжать работу
	} else //если в очереди ожидания ещё есть потоки
	//ожидаем, пока в очередь ожидания не придёт последний поток
		while (threads_out < total_threads) 
		//ожидаем разрешения продолжить работу: освободить mutex и ждать сигнала от condvar, затем "захватить" mutex опять
			pthread_cond_wait(&condvar_out, &mutex);
	
	pthread_mutex_unlock(&mutex); //"освободить" mutex
}


void Reverse(int n, double* a, double* x, int first_row, int last_row) //обратный ход метода Гаусса
{
int i, k, j;
double tmp;
for (k = first_row; k < last_row; k++)
		for (i = n - 1; i >= 0; i--)
		{
                        if (fabs(x[i * n + k]) <1e-50) x[i*n + k] = 1e-50;
                        tmp = x[i * n + k];
			for (j = i + 1; j < n; j++)
				tmp -= a[i * n + j] * x[j * n + k];
			x[i * n + k] = tmp;
		}
}

int FindMax(int n, double* a, int indMax, double max, int i) //поиск максимального элемента по столбцу
{
int j;
	for (j = i + 1; j < n; ++j)
			if (max < fabs(a[j * n + i]))
			{
				max = fabs(a[j * n + i]);
				indMax = j;
			}
         return indMax;
}

double SolutionError(double *a, double *x, int n, int total_threads, int rank, double *solution_error){
	int i, j, k, first_row, last_row;
	double norm = 0.0, tmp;
	first_row = (n * rank) / total_threads;
        last_row = (n * rank + n) / total_threads;
    for (i = first_row; i < last_row; i++)
	{
        for (j = 0; j < n; j++)
		{ 
			tmp = 0.0;
			for (k=0; k<n; k++)
           	 	tmp += a[i * n + k] * x[k*n + j];//i-ая строка на j-ый столбец
        	if (i == j)
				tmp -= 1.0;
				
			norm += tmp*tmp;
    }
}
	synchronize(total_threads);
        if (fabs(norm)<1e-50) norm = 1e-50;
   *solution_error += norm;
    //printf("norma = %e\n", sqrt(norm));
    return sqrt(norm);
}

//вывод нормы невязки
/*
double SolutionError(int n, double* a, double* x)
{
	int i;
	int j;
	int k;
	double tmp;
	double rezult;

	rezult = 0.0;
	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < n; ++j)
		{
			tmp = 0.0;
			for (k = 0; k < n; ++k)
				tmp += a[i * n + k] * x[k * n + j]; //i-ая строка на j-ый столбец

			if (i == j)
				tmp -= 1.0;

			rezult += tmp * tmp;
		}
	}

	return sqrt(rezult);
}
*/

/*
int InvertMatrix(int n, double* a, double* x)
{
	int i;
	int j;
	int indMax;
	double max;

	for (i = 0; i < n; ++i)
		for (j = 0; j < n; ++j)
			x[i * n + j] = (double)(i == j); //делаем матрицу X единичной

	for (i = 0; i < n; ++i)
	{ //инкрементируем максимальный элемент, по умолчанию - (i;i)-ый
		max = fabs(a[i * n + i]);
		indMax = i; 
                indMax = FindMax(n, a, i, max, i); //ищем максимальный элемент по столбцам
			
		SwapColumns(n, a, i, indMax); //переставляем местами столбцы i и indMax, чтобы наибольший элемент оказался наверху
		SwapColumns(n, x, i, indMax); //делаем это же в матрице x	
		if (fabs(a[i * n + i]) < 1e-100) //если вдруг на (i;i)-ом месте оказался 0, выходим - матрица выродилась
			return -1;
			
		DivideRow(n, a, x, i, 1.0 / a[i * n + i]); //делим строки, ниже i-ой на элемент (i;i) в матрицах a и x 
		Division(n, a, x, i); //вычитаем из строк, ниже j, строку j, умноженную на a[j*n + k], начиная с i-го столбца
	}
		Reverse (n, a, x); //обратный ход метода Гаусса - выставляем 0 выше главной диагонали
	return 0;
}
*/

int Inverse(double *a, int n, double *x, int rank, int total_threads, int *flag)
{
        int i, j, k;
	int first_row;
	int last_row;
        double tmp;
        //synchronize(total_threads);
	if (rank == 0)
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				x[i * n + j] = (double)(i == j); //делаем матрицу X единичной

	for (i = 0; i < n; i++)
	{
		if (rank == 0)
		{
                        k = i; //индекс наибольшего элемента
			for (j = i + 1; j < n; j++)
				if (fabs(a[k * n + i]) < fabs(a[j * n + i]))
					k = j;
					
			SwapColumns(n, a, i, k);
			SwapColumns(n, x, i, k);

			tmp = a[i * n + i];
			
                        if (fabs(tmp)<1e-100) //проверка на вырожденность
			{
				flag[0] = 1;
				synchronize(total_threads); //завершаем все потоки
				return -1;
			}
			
			tmp = 1.0/tmp;
			DivideRow(n, a, x, i, tmp);

		}
		
                synchronize(total_threads);

                first_row = (n - i - 1) * rank;
		first_row = first_row/total_threads + i + 1;
		last_row = (n - i - 1) * (rank + 1);
		last_row = last_row/total_threads + i + 1;

                Division(n, a, x, i, first_row, last_row); //вычитаем из строк, ниже i, строку j, умноженную на a[i*n + k], начиная с i-го столбца
	
                synchronize(total_threads);
	}
	first_row = n * rank;
	first_row = first_row/total_threads;
	last_row = n * (rank + 1);
	last_row = last_row/total_threads;
        Reverse(n,  a,  x, first_row, last_row);

	return 0;
}
