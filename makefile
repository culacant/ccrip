CFLAGS=-I./include -L./lib -lraylib -lglfw3 -lopenAL32 -ggdb

main: ccrip
	gcc  -o ccrip $(CFLAGS) obj/ccrip.o obj/unit.o obj/map.o obj/ai.o obj/bullet.o
ccrip:  ccrip.c bullet unit map ai
	gcc -c -o obj/ccrip.o $(CFLAGS) ccrip.c
bullet: bullet.c
	gcc -c -o obj/bullet.o $(CFLAGS) bullet.c
unit: unit.c
	gcc -c -o obj/unit.o $(CFLAGS) unit.c
ai: ai.c
	gcc -c -o obj/ai.o $(CFLAGS) ai.c
map: map.c
	gcc -c -o obj/map.o $(CFLAGS) map.c
