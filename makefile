CC=clang
CXX=clang++
CFLAGS= 
CXXFLAGS=-std=c++11 -stdlib=libc++
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o,$(SOURCES))
main: $(OBJS) 
	@echo "source files:" $(SOURCES)
	@echo "object files:" $(OBJS)
	clang++ $(OBJS) -o main
clean:
	rm -rf $(OBJS)
	rm -f main