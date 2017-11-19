
popcl : main.o pop3.o
	g++ -std=gnu++11 -Wall -Wextra -o popcl main.o pop3.o -lcrypto -lssl

main.o : main.cpp main.hpp
	g++ -std=gnu++11 -Wall -Wextra -c -o main.o main.cpp

pop3.o : pop3.cpp pop3.hpp
	g++ -std=gnu++11 -Wall -Wextra -c -o pop3.o pop3.cpp

clean:
	rm *.o popcl xkadle34.tar

tar:
	tar -cf xkadle34.tar Makefile main.cpp main.hpp pop3.cpp pop3.hpp README manual.pdf