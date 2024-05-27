#define MAIN

#include <iostream>
#include "game.hpp"
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include <math.h>
#include <GL/gl.h>

int main(int argc, char *argv[])
{
    Game game{argc, argv, "Bussing Game", vec2(1024, 768), 60};
    game.startGame();
}