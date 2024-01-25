
SOURCE_FILES := src/main.cpp src/texture/texture.cpp src/shaders/program.cpp src/mesh/mesh.cpp src/material/material.cpp src/game_object/game_object.cpp

all:
	g++ -I ./src/ -I ./include/ -Wall -Wextra -lglfw -lGL -g -O2 $(SOURCE_FILES) -o build/main