CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wextra
LIBS=-lSDLmain -lSDL2 -lSDL2_ttf -lSDL2_image
SRC_FILES=src/Input/Input.cpp src/Main.cpp src/Engine.cpp src/Rendering/Rendering.cpp
TEST_FILES=src/Input/Input_test.cpp src/Rendering/Rendering_test.cpp
TEST_FLAG=WITH_TESTS

build:
	g++ $(SRC_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=false -o engine

tests:
	g++ $(SRC_FILES) $(TEST_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=true -o engine-test
