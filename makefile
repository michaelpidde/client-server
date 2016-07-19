client: bin
	g++ SocketHandler.cpp Client.cpp -o ./bin/client

server: bin
	g++ SocketHandler.cpp Server.cpp -o ./bin/server

bin:
	mkdir ./bin