all:
	g++ -std=c++11 -o snake snakegame.cpp Manager.cpp Snake.cpp -lncurses -Wmultichar