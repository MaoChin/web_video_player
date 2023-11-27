main:main.cc
	g++ -o  $@ $^ -std=c++11 -ljsoncpp -lmysqlclient -lcpp-httplib -lpthread

.PHONY:clean
clean:
	rm -rf main
