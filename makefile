all: bin
	g++ SocketHandler.cpp Client.cpp -o ./bin/client

bin:
	mkdir ./bin