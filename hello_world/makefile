all : hello_world

clean : 
	rm -f hello_world.o hello_world

hello_world.o : hello_world.c
	gcc -c hello_world.c

hello_world : hello_world.o
	gcc hello_world.o -o hello_world
