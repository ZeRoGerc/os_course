all : rshd

clean :
	rm -f *.o rshd

rshd : proxy.o proxy_client.o event_queue.o socket.o rshd.cpp
	g++ -std=c++11 $? -o rshd

%.o : %.cpp %.hpp
	g++ -std=c++11 -c -o $@ $<
