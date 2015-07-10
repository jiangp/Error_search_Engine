INC_DIR:=./include/ ./include/sock_epoll_h/ ./include/thread_pool_h/
SRC_DIR:=./src/ ./src/sock_epoll/ ./src/thread_pool/
SRCS:=$(wildcard ./src/*.cpp) $(wildcard ./src/sock_epoll/*.cpp) $(wildcard ./src/thread_pool/*.cpp)
LIBS:= -lpthread
CXX:=g++
CXXFLAGS:= -Wall -g -std=c++11 -lpthread  -lhiredis  $(addprefix -I, $(INC_DIR) )
EXE:=./bin/main.o
$(EXE):$(SRCS)
	$(CXX) -o $(EXE) $(SRCS) $(CXXFLAGS)

clean:
	rm -rf $(EXE)
	rm -rf $(OBJS)
