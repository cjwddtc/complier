CC=clang
CXX=clang++
CFLAGS= 
CXXFLAGS=-std=c++11 -g -Wno-return-type -Wno-comment -Wno-deprecated-declarations
target=main
SOURCESCPP = $(wildcard *.cpp)
SOURCESC = $(wildcard *.c)
LD_FLAG=
OBJS = $(patsubst %.cpp, %.o,$(SOURCESCPP)) $(patsubst %.c, %.o,$(SOURCESC))
RULES = $(patsubst %.cpp, %.d,$(SOURCESCPP)) $(patsubst %.c, %.d,$(SOURCESC))
$(target): $(OBJS) 
	@echo "source files:" $(SOURCESC) $(SOURCESCPP)
	@echo "object files:" $(OBJS)git
	$(CXX) $(OBJS) -o $(target) $(LD_FLAG)
include $(RULES)
%d : %cpp
	gcc -MM -MD $<
%d : %c
	gcc -MM -MD $<
clean:
	rm -rf $(OBJS)
	rm -f main
	rm -rf $(RULES)