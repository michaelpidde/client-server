server: bin
	g++ -std=c++11 File.cpp Config.cpp Request.cpp Socket.cpp Server.cpp -o ./bin/server

install: bin
	cp config-default.conf ./bin/config-default.conf

bin:
	mkdir ./bin