risk : main.o console.o map.o territory.o
	g++ main.o console.o map.o territory.o -o risk -lsfml-graphics -lsfml-window -lsfml-system

main.o : main.cpp console.h map.h territory.h
	g++ -c -std=c++11 main.cpp

console.o : console.cpp console.h
	g++ -c -std=c++11 console.cpp

map.o : map.cpp map.h territory.h
	g++ -c -std=c++11 map.cpp

territory.o : territory.cpp territory.h
	g++ -c -std=c++11 territory.cpp
