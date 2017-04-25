risk : main.o game.o i_agent.o human_agent.o random_agent.o planning_agent.o console.o map.o territory.o
	g++ main.o game.o i_agent.o human_agent.o random_agent.o planning_agent.o console.o map.o territory.o -o risk -lsfml-graphics -lsfml-window -lsfml-system

main.o : main.cpp console.h map.h territory.h
	g++ -c -g -std=c++11 main.cpp

game.o : game.cpp game.h i_agent.h human_agent.h random_agent.h planning_agent.h territory.h map.h console.h
	g++ -c -g -std=c++11 game.cpp

console.o : console.cpp console.h
	g++ -c -g -std=c++11 console.cpp

map.o : map.cpp map.h territory.h
	g++ -c -g -std=c++11 map.cpp

territory.o : territory.cpp territory.h i_agent.h
	g++ -c -g -std=c++11 territory.cpp

i_agent.o : i_agent.cpp i_agent.h map.h territory.h
	g++ -c -g -std=c++11 i_agent.cpp

human_agent.o : human_agent.cpp human_agent.h i_agent.h territory.h map.h console.h
	g++ -c -g -std=c++11 human_agent.cpp

planning_agent.o : planning_agent.cpp planning_agent.h random_agent.h i_agent.h territory.h map.h
	g++ -c -g -std=c++11 planning_agent.cpp

random_agent.o : random_agent.cpp random_agent.h i_agent.h territory.h map.h
	g++ -c -g -std=c++11 random_agent.cpp
