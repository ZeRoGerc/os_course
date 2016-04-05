all : midtermsh

clean : 
	rm -f midtermsh.o midtermsh

midtermsh.o : midtermsh.cpp
	g++ -c -std=c++11 midtermsh.cpp

midtermsh : midtermsh.o
	g++ midtermsh.o -o midtermsh
