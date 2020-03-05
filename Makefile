CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wextra
LIBS=-lSDLmain -lSDL2 -lSDL2_ttf
SRC_FILES=src/Main.cpp src/GameState.cpp src/Input/Input.cpp
TEST_FILES=src/Input/Input_test.cpp
TEST_FLAG=WITH_TESTS

build:
	g++ $(SRC_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=false -o engine

tests:
	g++ $(SRC_FILES) $(TEST_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=true -o engine-test