#pragma once
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"
#include "GL/gl.h"

#include "object.hpp"
#include "renderer.hpp"

class Player
{
  public:
    Player(double cameraSpeed, Renderer* renderer);

    void setProgram(GLuint programValue);
    void updateCamera(double deltaTime);
    void mouse(int x, int y);

    void setObjectToFollow(Object* obj);
    void setPosition(vec3 const& pos);

    vec3 getCameraPos();

  private:
    void keyboard(double deltaTime);
    void moveCamera(vec3 const& direction, double deltaTime);

    bool debugCamera;
    double cameraSpeed;

    vec3 cameraPos;
    vec3 cameraLookPos;
    mat4 camera;

    GLuint program;

    Object* objectToFollow{};
    vec3 objectLastUpdateRot{};

    Renderer* renderer;
};

void mouse(int x, int y);