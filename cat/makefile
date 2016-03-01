all : cat

clean : 
	rm -f cat.o cat

cat.o : cat.c
	gcc -c cat.c

cat : cat.o
	gcc cat.o -o cat
