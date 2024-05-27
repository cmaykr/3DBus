#pragma once

#include "object.hpp"
#include <vector>

/// Handles drawing/rendering of the game.
class Renderer
{
public:
    Renderer(std::vector<Object*> &objects);

    void draw() const;

    void uploadProjectionMat(mat4 const& projectionMatrix);
    mat4 playerCamera;
private:
    std::vector<Object*> &objects;

    mat4 projectionMatrix;
};