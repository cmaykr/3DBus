#pragma once

#include <VectorUtils4.h>
#include <string>
#include "GL_utilities.h"
#include <vector>
#include "object.hpp"
#include "terrain.hpp"
#include "road.hpp"
#include "renderer.hpp"
#include "player.hpp"
#include "bus.hpp"

class Game
{
public:
    Game(int argc, char *argv[], std::string const& title, vec2 const& screenSize, int FPS); // Init for opengl and game.

    // Main loop for game, draws the game and updates.
    void startGame(); 
    void draw() const;
    void update();
    GLuint getProgram(); // (Axel) Added this when first testing camera controls. Can be removed if no other use.
private:
    void gameInit();
    GLuint program;
    GLuint treeShader;
    std::vector<Object*> objects;

    Renderer renderer;
    Player player;
    Bus bus;


    double deltaTime{};
    double startTime{};
    double endTime{};
};