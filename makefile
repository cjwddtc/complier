CC=clang
CXX=clang++
CFLAGS= 
CXXFLAGS=-std=c++11 -stdlib=libc++ -g
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
RULES = $(patsubst %.cpp, %.d,$(SOURCES))
main: $(OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(OBJS)
	$(CXX) $(OBJS) -o main $(CXXFLAGS)
include $(RULES)
%d : %cpp
	gcc -MM -MD $<
clean:
	rm -rf $(OBJS)
	rm -f main
	rm -rf $(RULES)