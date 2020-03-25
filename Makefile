all: main.o matrix.o task.o
	gcc main.o matrix.o task.o -lm -lpthread -o prog -O3

main.o: main.c matrix.h task.h
	gcc main.c -c -o main.o -O3

matrix.o: matrix.c matrix.h
	gcc matrix.c -c -o matrix.o -O3

task.o: task.c task.h
	gcc task.c -c -o task.o -O3

clean:
	rm -f *.o prog
