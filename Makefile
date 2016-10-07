#
# Makefile
#

CXX = g++
EXEC_HTTP = server_http

SOURCE_HTTP = main_http.cpp

OBJECTS_HTTP = main_http.o

LDFLAGS=-L/usr/lib

LDFLAGS_COMMON = -std=c++11 -O3 -pthread -L/usr/local/bin -lboost_system
LDFLAGS_HTTP = 

LPATH_COMMON = -I/usr/include/boost
LPATH_HTTP = 

LLIB_COMMON = -L/usr/lib
LLIB_HTTP =

http:
	$(CXX) $(SOURCE_HTTP) $(LDFLAGS_COMMON) $(LDFLAGS_HTTP) $(LDPATH_COMMON) $(LSPATH_HTTP) $(LLIB_COMMON) $(LLIB_HHTP) -o $(EXEC_HTTP)

clean:
	rm -f $(EXEC_HTTP) *.o
