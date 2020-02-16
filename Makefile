CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wextra
LIBS=-lSDLmain -lSDL2

build:
	g++ src/Main.cpp $(CXXFLAGS) $(LIBS) -o engine