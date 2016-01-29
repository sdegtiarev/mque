
CXX= g++
CC= g++
CXXFLAGS= -g -pthread -std=c++11 
LDFLAGS= 
LDLIBS= #-lboost_thread -lboost_system 



all: que_c0 que_c3 que_b que_d

que_d: que_d.d
	dmd que_d



clean:
	@rm -f que_c0 que_c3 que_b que_d *.o core*

