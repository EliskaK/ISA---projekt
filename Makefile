
main : main.o
	g++ -std=gnu++11 -Wall -Wextra -o main main.o

main.o : main.cpp main.hpp
	g++ -std=gnu++11 -Wall -Wextra -c -o main.o main.cpp

clean:
	rm *.o main xkadle34.tar

tar:
	tar -cf xkadle34.tar Makefile main.cpp main.hpp README manual.pdf