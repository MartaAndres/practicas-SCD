default: all

all: extra_calculoPi

extra_calculoPi: extra_calculoPi.cpp
	g++ -o $@ $^ -pthread

clean:
	rm -f *.o extra_calculoPi
