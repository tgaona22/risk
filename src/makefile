risk : main.o console.o
	g++ main.o console.o -o risk -lsfml-graphics -lsfml-window -lsfml-system

main.o : main.cpp console.h
	g++ -c main.cpp

console.o : console.cpp console.h
	g++ -c console.cpp
