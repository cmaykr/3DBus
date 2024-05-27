#include "renderer.hpp"

#include <iostream>

Renderer::Renderer(std::vector<Object*> &objects)
    : objects(objects)
{

}

void Renderer::draw() const
{
    for (Object* obj: objects)
    {
        glUseProgram(obj->getShader());
        for (Object* a: objects)
        {
            a->uploadSpecificUniforms(obj->getShader());
        }
        obj->drawObject(projectionMatrix, playerCamera);
    }
}

void Renderer::uploadProjectionMat(mat4 const& projMat)
{
    projectionMatrix = projMat;
}