CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LIBS = -lpthread
all: interseccion
interseccion: interseccion.c
$(CC) $(CFLAGS) interseccion.c -o interseccion $(LIBS)
clean:
rm -f interseccion