all : cat

clean : 
	rm cat.o cat

cat.o : cat.c
	gcc -c cat.c

cat : cat.o
	gcc cat.o -o cat
