OPTIONS=-g -Wall -Wextra -pedantic -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fstack-protector -lmcheck -D_FORTIFY_SOURCE=2
OPTIONS_R=-O2

all : sshd

clean :
	rm -f *.o sshd

sshd : proxy.o proxy_client.o event_queue.o socket.o sshd.cpp
	g++ -std=c++11 $? -o sshd

%.o : %.cpp %.hpp
	g++ -std=c++11 -c -o $@ $<
