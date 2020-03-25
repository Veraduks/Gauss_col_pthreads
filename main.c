#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>

#include "matrix.h"
#include "task.h"

void *func(void* p_arg);

double thread_time = 0, thread_time_1 = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Params {
int n; // размер матрицы, число -n
char* f; // имя формулы, строка -f
char* i; //имя входного файла, строка -i
char* v; // отладочный вывод, флаг -v
int thr; //число нитей -t

};

void *func(void* p_arg) 
{
ARGS *arg = (ARGS*)p_arg;
double t = 0.0, t11 = 0.0;
struct timespec t1, t2, t3, t4;
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t1);

Inverse(arg->matrix, arg->n, arg->invmatr, arg->rank, arg->total_threads, arg->flag);

clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t2);
t = (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / (1000000000.0);
pthread_mutex_lock(&mutex);
thread_time += t;

if(arg->flag[0] == 1) { //выход в случае вырожденной матрицы
	printf("Поток %d закончил, время = %lf\n", arg->rank, t);
	return NULL;
}

pthread_mutex_unlock(&mutex);
clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t3);

SolutionError(arg->b, arg->invmatr, arg->n, arg->total_threads, arg->rank, &(arg->solution_error)); //норма невязки

clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t4);
t11 = (t4.tv_sec - t3.tv_sec) + (t4.tv_nsec - t3.tv_nsec) / (1000000000.0);
pthread_mutex_lock(&mutex);
thread_time_1 += t11;
pthread_mutex_unlock(&mutex);
printf("Время поиска нормы для %d-го потока = %lf\n", arg->rank, t11);
printf("Поток %d закончил, суммарное время его работы = %lf\n", arg->rank, t + t11);
return NULL;
}


int main(int argc, char** argv) {
	struct Params params;
        FILE *file;
        int opt, inputMode,
               // rezult,
                inputParameter, i, total_threads, *flag;
        double *matrix = NULL, *x = NULL, *b = NULL, solerr = 0.0;
	struct timespec time_start, time_end;
	pthread_t *threads = NULL;
	ARGS *args = NULL;
	clock_gettime(CLOCK_MONOTONIC, &time_start);

params.f = NULL;
params.i = NULL;
params.n = 0;
params.v = NULL;
params.thr = 0;

	printf("qДоступные на данный момент виды матриц: Bad, Rand, 1, 2, 9, 18, 22 \n");
	
while ((opt = getopt(argc, argv, "n:f:i:v:t:")) != -1) {
switch (opt) {
case 'n': {
    char *next;
    params.n = strtol(optarg, &next, 10);
    break;
}

case 'f': {
    params.f = optarg;
    break;
}
case 'i': {
    params.i = optarg;
    break;
}
case 'v':{
    params.v = optarg;
    break;
}
case 't': {
    char *next;
    params.thr = strtol(optarg, &next, 10);
    break;
}

}
}
if (params.n < 1){ printf ("Введите корректный размер матрицы \n"); return -4;}
    else printf("Введённый размер матрицы: %d \n", params.n);
if (sizeof(double)* params.n * params.n >= INT_MAX)
{
    printf("Вы ввели слишком большое n, в компьютере не хватает памяти \n");
    return -13;
}
if (params.thr < 1){ printf ("Введите корректное число потоков \n"); return -14;}
    else printf("Количество потоков: %d \n", params.thr);
	
if ((params.f==NULL)&&(params.i == NULL)) printf ("Введите корректное название формулы или файла \n");
    else 
	if ((params.f != NULL)&&(params.i != NULL))
	{
		printf("Введённая формула: %s \n", params.f);
		inputMode = 1;
	}
	
if (params.i != NULL) {
	inputMode = 1;
    file = fopen(params.i, "r");
    if (!file) { 
		printf ("Входного файла не существует \n");
		inputMode = -1;
	}
} else {
    printf ("Корректный входной файл не введён! \n");
    if (params.f != NULL) {
        if ((strcmp (params.f, "BadMatrix") == 0)||
        (strcmp(params.f, "RandMatrix") == 0)||
	(strcmp(params.f, "f_1_1") == 0)||
	(strcmp(params.f, "f_1_2")==0)||
	(strcmp(params.f, "f_1_9") == 0)||
	(strcmp(params.f, "f_1_18") == 0)||
	(strcmp(params.f, "f_1_20") == 0)||
	(strcmp(params.f, "f_1_22") == 0))
	{
		printf("Осуществляем генерацию матрицы по формуле %s...\n", params.f); 
		inputMode = 0;
		}
	else printf ("Некорректное имя формулы \n");
	} 
	else {
			printf("Вы не ввели ни название входного файла, ни формулу, по которой матрица будет генерироваться! \n");
			inputMode = -1;
		}
}
if (params.v != NULL) printf ("InputMode: %d \n", inputMode);
/*
if ((params.v != 0)&&(params.v != 1)) printf("Не задан отладочный вывод \n");
    else printf ("Введённый отладочный вывод: %d \n", params.v);
*/

total_threads = params.thr;

matrix = (double*)malloc(sizeof(double) * params.n * params.n); //определим память для введённой матрицы
x = (double*)malloc(sizeof(double)* params.n * params.n); //выделим память под матрицу решения
b = (double*)malloc(sizeof(double)* params.n * params.n); //выделим память под матрицу исходную
threads = (pthread_t*)malloc(total_threads * sizeof(pthread_t));
args = (ARGS*)malloc(total_threads * sizeof(ARGS));
flag = (int*)malloc(sizeof(int));
if (!(matrix && x && b && threads && args && flag))
	{
		printf("Не хватает памяти для матриц!\n");

		if (matrix)
			free(matrix);
		if (x)
			free(x);
        if (b)
            free(b);
		if (threads)
            free(threads);
		if (args)
			free(args);
		if (flag)
			free(flag);
		return -7;
	}
inputParameter = MatrixInput(params.n, matrix, inputMode, params.f);

if ((inputMode != -1)&&(inputParameter == -1)) 
	{ 
	free(matrix);
	free(x);
    free(b);
	free(threads);
	free(args);
	free(flag);
	printf("Ошибка чтения из файла, проверьте данные! \n");
	return -6;
	
}

if ((inputMode != -1)&&(inputParameter == -8)) 
{ 

free(matrix);
free(x);
free(b);
free(threads);
free(args);
free(flag);
	printf("Вы что-то не так ввели в матрице, проверьте входной файл \n");
	return -8;
	
}

if (inputMode == -1) //если в файле что-то не так
{
    if (matrix)
        free(matrix);
    if (x)
        free(x);
    if(b)
        free(b);
    if (threads)
        free(threads);
    if (args)
        free(args);
    if (flag)
        free(flag);
    return -11;
}
printf("\nМатрица A:\n");
	MatrixOutput(params.n, matrix);
	memcpy(b, matrix, params.n*params.n*sizeof(double)); //сохранили исходную матрицу
	printf("\n");
	printf("Ищем обратную...\n");
flag[0] = 0; //сигнализатор вырожденности
	for (i = 0; i < total_threads; i++)
	{
		args[i].n = params.n;
		args[i].matrix = matrix;
		args[i].invmatr = x;
		args[i].b = b;
		args[i].rank = i;
		args[i].total_threads = total_threads;
		args[i].solution_error = 0;
		args[i].flag = flag;
	}
	
	
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	
	for (i = 0; i < total_threads; i++)
		if (pthread_create(threads + i, 0, func, args + i))
		{
			printf("Не могу создать нить %d!\n", i);
		
			if (matrix) free(matrix);
			if (x) free(x);
			if (b) free(b);
			if (threads) free(threads);
			if (args) free(args);
			if (flag) free(flag);
			return -7;
		}
		
	for (i = 0; i < total_threads; i++){
		if (pthread_join(threads[i], 0))
		{
			printf("Не могу дождаться нити %d!\n", i);

			if (matrix) free(matrix);
			if (x) free(x);
			if (b) free(b);
			if (threads) free(threads);
			if (args) free(args);
			if (flag) free(flag);
			return -8;
		}
		
		if(args[i].flag[0] == 1){
            printf("Матрица необратима!\n");
          	if (matrix) free(matrix);
			if (x) free(x);
			if (b) free(b);
			if (threads) free(threads);
			if (args) free(args);
			if (flag) free(flag);
            return -10;
        }
	}
clock_gettime(CLOCK_MONOTONIC, &time_end);

/* switch (rezult)
	{
	case -1:
		printf("Не могу инвертировать - матрица вырожденная.\n");
		break;
		
	case 0:
       */
		printf("\nМатрица A^{-1}:\n");
		MatrixOutput(params.n, x);
                printf("\n\n");

                for (i = 0; i<params.thr; i++)
			solerr += args[i].solution_error; // подсчёт нормы невязки
			
		time_end = diff(time_start, time_end);
		printf("Норма невязки ||A*A^{-1}  - E||\t= %e\n", sqrt(solerr));
                //break;
                /*
	default:
		printf("Неизвестная ошибка!\n");
		break;
	}
        */
printf("\n");

if (params.v != NULL){ printf("Матрица А после метода Гаусса теперь имеет вид: \n");
MatrixOutput(params.n, matrix);}

free(matrix);
free(x);
free(b);
free(threads);
free(args);
free(flag);
return 0;
} 
