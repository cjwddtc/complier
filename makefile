CC=clang
CXX=clang++
CFLAGS= 
<<<<<<< HEAD
CXXFLAGS=-std=c++14 -g -Wno-return-type -Wno-comment -Wno-deprecated-declarations
=======
target=main
CXXFLAGS=-std=c++11 -stdlib=libc++ -g -Wno-return-type -Wno-comment
>>>>>>> b76b6ba1dae961c0c634daa9da16fcbfd9640894
SOURCESCPP = $(wildcard *.cpp)
SOURCESC = $(wildcard *.c)
LD_FLAG=
OBJS = $(patsubst %.cpp, %.o,$(SOURCESCPP)) $(patsubst %.c, %.o,$(SOURCESC))
RULES = $(patsubst %.cpp, %.d,$(SOURCESCPP)) $(patsubst %.c, %.d,$(SOURCESC))
<<<<<<< HEAD
main.exe: $(OBJS) 
	@echo "source files:" $(SOURCESC) $(SOURCESCPP)
	@echo "object files:" $(OBJS)
	$(CXX) $(OBJS) -o main.exe $(LD_FLAG)
=======
$(target): $(OBJS) 
	@echo "source files:" $(SOURCESC) $(SOURCESCPP)
	@echo "object files:" $(OBJS)git
	$(CXX) $(OBJS) -o $(target) $(LD_FLAG)
>>>>>>> b76b6ba1dae961c0c634daa9da16fcbfd9640894
include $(RULES)
%d : %cpp
	gcc -MM -MD $<
%d : %c
	gcc -MM -MD $<
clean:
	rm -rf $(OBJS)
	rm -f main
	rm -rf $(RULES)