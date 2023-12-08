CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0 
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

SRCS = csim.cpp
OBJS = $(SRCS:%.cpp=%.o)

all : csim

csim : $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

clean :
	rm -f $(OBJS) csim depend.mak

depend :
	$(CC) $(CFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

include depend.mak