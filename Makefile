CFLAGS=-Wall -O2
LIBS=-lgsl -lgslcblas -lm
CC=gcc

obligatorio: main.o imagen.o filtros.o
	cc -o obligatorio main.o imagen.o filtros.o -lm

main.o: main.c
	cc -c main.c

imagen.o: imagen.c
	cc -c imagen.c

filtros.o: filtros.c
	cc -c filtros.c

clean:
	-rm -f main.o
	-rm -f imagen.o
	-rm -f filtros.o
	-rm -f obligatorio
