client: bin
	g++ SocketHandler.cpp Client.cpp -o ./bin/client

server: bin
	g++ RequestHandler.cpp SocketHandler.cpp Server.cpp -o ./bin/server

install: bin
	cp config-default.conf ./bin/config-default.conf

bin:
	mkdir ./bin