#include "game.hpp"
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include <iostream>
#include "LoadTGA.h"
#include <iomanip>
#include <random>

#include "object.hpp"
#include "bus.hpp"
#include "road.hpp"
#include "terrain.hpp"

TextureData ttex;
Game* game;

vec3 treeA[] = {vec3(1,2,1),
				vec3(1,2,-1), 
				vec3(-1,2,-1),
				vec3(-1,2,1)};
vec3 treeB[] = {vec3(1,-2,1),
				vec3(1,-2,-1), 
				vec3(-1,-2,-1),
				vec3(-1,-2,1)};

vec3 BusA[] = {vec3(2.2,0.5,2.2),   // 10% of bus.obj because of bus 0.1 scale
				vec3(2.2,0.5,-2.2), 
				vec3(-2.2,0.5,-2.2),
				vec3(-2.2,0.5,2.2)};

vec3 BusB[] = {vec3(2.2,-0.5,2.2),
				vec3(2.2,-0.5,-2.2), 
				vec3(-2.2,-0.5,-2.2),
				vec3(-2.2,-0.5,2.2)};

void gameLoop()
{
    game->update();
    game->draw();
}

Game::Game(int argc, char *argv[], std::string const& title, vec2 const& screenSize, int FPS)
    : program(), objects(), renderer(objects), player(30, &renderer), bus()
{
    game = this;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 2);
    glutInitWindowSize(screenSize.x, screenSize.y);
    glutCreateWindow(title.c_str());
    glutDisplayFunc(gameLoop);
    gameInit();
    glutRepeatingTimer(1000/FPS);

    mat4 projMat = frustum(-screenSize.x/screenSize.y/10, screenSize.x/screenSize.y/10, -0.1, 0.1, 0.2, 2000.0);
    glutReshapeWindow(screenSize.x, screenSize.y);
    renderer.uploadProjectionMat(projMat);

    //Camera & Mouse
    glutPassiveMotionFunc(mouse);
    glutHideCursor();
    player.setProgram(program);

    // For testing
    Model *mdl = LoadModel("models/bus.obj");
    GLuint tex, tex2, tex3, tex4, texSkybox;
    GLuint tex5, tex6;
    LoadTGATextureSimple("textures/grass.tga", &tex);
    LoadTGATextureSimple("textures/water-texture-2.tga", &tex6);
    LoadTGATextureSimple("textures/terrainSplat.tga", &tex5);
    //glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex);
	LoadTGATextureSimple("textures/BusTexture.tga", &tex2);
    LoadTGATextureSimple("textures/conc.tga", &tex3);
    //glBindTexture(GL_TEXTURE_3D, tex2);
    std::cout << "Tex int: " << tex5 << std::endl;
    printError("init shader");
    LoadTGATextureData("textures/fft-terrain.tga", &ttex);
    Terrain* terr = new Terrain{&ttex, program, tex};
    terr->setSplatmap(tex5, tex6);
    Road* road = new Road{"models/road.txt", 3, terr, treeShader, tex3};

    // Skybox init
	Model *skyboxMdl = LoadModel("models/skybox-full-tweaked.obj");
    LoadTGATextureSimple("textures/atmosphere-cloud.tga", &texSkybox);
	/* glActiveTexture(GL_TEXTURE6);
	LoadTGATextureSimple("labskybox512.tga", &texSkybox);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, texSkybox); */
	
	//Skybox
    Object* skybox = new Object{skyboxMdl, program, texSkybox};
    objects.push_back(skybox);
    objects.at(objects.size()-1)->setDepthTest(false);
    skybox->setRotation(vec3(0, 1.5708, 0));
    skybox->setSpecialLightBool(false);


    bus = Bus{mdl, treeShader, tex2, vec3(0.2, 0.2, 2), 30, 0.95, 2.3, terr, road};
    bus.setScale(vec3(0.1));
    bus.setPosition(vec3(10.2,4.5,10));
    //bus.setRotation(vec3(1, 0, 0));

	Model *wheelmdl = LoadModel("models/BusWheel.obj");
	Object* wheel = new Object{wheelmdl, program, 3, &bus};

	wheel->setScale(vec3(0.1));
	wheel->setPosition(vec3(0.2, 0, 2.4));
	wheel->setRotation(vec3(-1.5708, -1.5708, 0));
    wheel->setSpecialLightBool(false);
	bus.addChild(wheel);
    objects.push_back(&bus);
    //player.setObjectToFollow(objects.at(1));

    objects.push_back(terr);

	objects.push_back(road);

    LoadTGATextureSimple("textures/bush.tga", &tex4);
    std::vector<Model*> foliageMdls{};
    foliageMdls.push_back(LoadModel("models/bush.obj"));
    foliageMdls.push_back(LoadModel("models/tree02.obj"));

    objects.push_back(new Object(foliageMdls.at(1), treeShader, tex4));
    objects.at(objects.size()-1)->setPosition(vec3(10, 1, 40));
    objects.at(objects.size()-1)->setAB(treeA, treeB);
    objects.at(1)->setAB(BusA, BusB);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(2, terr->maxWidth()*2-2);
    for (int i{}; i < 400; ++i)
    {
        GLfloat x = dis(gen);
        GLfloat z = dis(gen);
        while (road->getHeight(x, z) != -999)
        {
            x = dis(gen);
            z = dis(gen);
        }
        if (i == 200)
        {
            double rad = static_cast<int>(dis(gen)) % 50;
            std::uniform_int_distribution<> dis2(0, rad*2);
            for (int j{}; j < 200; ++j)
            {
                GLfloat x1 = (dis2(gen)+static_cast<int>(x)) % terr->maxWidth()*2;
                GLfloat z1 = (dis2(gen)+static_cast<int>(z)) % terr->maxWidth()*2;
                while (road->getHeight(x1, z1) != -999)
                {
                    x1 = dis2(gen);
                    z1 = dis2(gen);
                }

                objects.push_back(new Object(foliageMdls.at(1), treeShader, tex4));
                objects.at(objects.size()-1)->setPosition(vec3(x1, terr->calculatePointHeight(x1,z1), z1));
                objects.at(objects.size()-1)->setAB(treeA, treeB); //AABB
            }
        }
        else if (i % 2 == 0)
        {
            objects.push_back(new Object(foliageMdls.at(1), treeShader, tex4));
            objects.at(objects.size()-1)->setPosition(vec3(x, terr->calculatePointHeight(x,z), z));
            objects.at(objects.size()-1)->setAB(treeA, treeB); //AABB
        }
        else
        {
            objects.push_back(new Object(foliageMdls.at(0), treeShader, tex4));
            objects.at(objects.size()-1)->setPosition(vec3(x, terr->calculatePointHeight(x,z), z));
        }
    }

    // end of testing
}

void Game::startGame()
{
    glutMainLoop();
    std::cout << "End Loop" << std::endl;
}

void Game::gameInit()
{
    glClearColor(0.2,0.2,0.5,0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    program = loadShaders("shaders/base.vert", "shaders/base.frag");
    treeShader = loadShaders("shaders/tree.vert", "shaders/tree.frag");
    std::cout << treeShader << std::endl;

}

void Game::draw() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(game->program);
    renderer.draw();

	
    glutSwapBuffers();
}



void Game::update() // (Axel) New function like draw, only difference practically is that it is not const so allowed to change stuff.
{
	startTime = glutGet(GLUT_ELAPSED_TIME);

    for (Object* obj: objects)
    {
        obj->update(deltaTime);
    }
    
    for (unsigned i = 2; i < objects.size(); i++) {
        bool hit = objects.at(1)->checkCollision(objects.at(i), 5);
        if (hit) 
        {
        bus.setSpeed(-bus.getSpeed());
        bus.setPosition(bus.getLastPos());
        }
    }

    //player.setPosition(objects.at(0)->globalPosition());
    player.updateCamera(deltaTime);

    /* bool hit = objects.at(0)->checkCollision(objects.at(objects.size()-1), 30);
    if (hit) {
        bus.setSpeed(0);
        bus.setPosition(bus.getLastPos());
    }

    std::cout << "HIT: " << hit << std::endl;
    std::cout << "BusGlobasPos: X: " << objects.at(0)->globalPosition().x << ", Y: " << objects.at(0)->globalPosition().y << ", Z: " << objects.at(0)->globalPosition().z << std::endl;
    std::cout << "BusglobalAB[0]: X: " << objects.at(0)->getGlobalAB()[0].x << ", Y: " << objects.at(0)->getGlobalAB()[0].y << ", Z: " << objects.at(0)->getGlobalAB()[0].z << std::endl;
    std::cout << "BusglobalAB[7]: X: " << objects.at(0)->getGlobalAB()[6].x << ", Y: " << objects.at(0)->getGlobalAB()[6].y << ", Z: " << objects.at(0)->getGlobalAB()[6].z << std::endl;
    std::cout << "TreeGlobasPos: X: " << objects.at(3)->globalPosition().x << ", Y: " << objects.at(3)->globalPosition().y << ", Z: " << objects.at(3)->globalPosition().z << std::endl;
    std::cout << "TreeglobalAB[0]: X: " << objects.at(3)->getGlobalAB()[0].x << ", Y: " << objects.at(3)->getGlobalAB()[0].y << ", Z: " << objects.at(3)->getGlobalAB()[0].z << std::endl;
    std::cout << "TreeglobalAB[7]: X: " << objects.at(3)->getGlobalAB()[6].x << ", Y: " << objects.at(3)->getGlobalAB()[6].y << ", Z: " << objects.at(3)->getGlobalAB()[6].z << std::endl;
    */

	deltaTime = startTime - endTime;
	deltaTime /= 1000;
	endTime = startTime;
	//std::cout << std::setprecision(4) << "Delta: " << deltaTime << std::endl;

	// Skybox
    objects.at(0)->setPosition(player.getCameraPos());
	/* glUniform1f(glGetUniformLocation(program, "shadeOn"), 0);
	glDisable(GL_DEPTH_TEST);
	skybox.drawObject(projectionmatrix, playercamera);
	glEnable(GL_DEPTH_TEST);
	glUniform1f(glGetUniformLocation(program, "shadeOn"), 1); */
}