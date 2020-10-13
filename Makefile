CXXFLAGS=-std=c++17 -Wall -Wpedantic -Wextra
LIBS=-lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image
SRC_FILES=src/Input/Input.cpp src/Main.cpp src/Engine.cpp src/Rendering/Rendering.cpp src/Animation2D/Animation2D.cpp
TEST_FILES=src/Input/Input_test.cpp src/Rendering/Rendering_test.cpp
TEST_FLAG=WITH_TESTS
UNAME_S=$(shell uname -s)

ifeq ($(UNAME_S),Linux)
LINUX_FLAG=-DLINUX
endif

build:
	g++ $(SRC_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=false $(LINUX_FLAG) -o engine

tests:
	g++ $(SRC_FILES) $(TEST_FILES) $(CXXFLAGS) $(LIBS) -D$(TEST_FLAG)=true $(LINUX_FLAG) -o engine-test
