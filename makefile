CC = gcc.exe
CFLAGS=-I./include -ggdb
LFLAGS=-L./lib -lraylibdll
OBJ=obj/stargrunts.o

main: stargrunts
	$(CC) -o stargrunts.exe $(OBJ) $(LFLAGS)
stargrunts:  stargrunts.c
	$(CC) -c stargrunts.c -o obj/stargrunts.o $(CFLAGS)
