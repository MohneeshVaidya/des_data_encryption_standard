run:	main
		./main

main:	main.cpp util.cpp tables.cpp
		g++ -std=c++23 -o main main.cpp util.cpp tables.cpp