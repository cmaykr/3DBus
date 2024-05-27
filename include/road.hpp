#pragma once

#include "object.hpp"
#include "terrain.hpp"
#include <string>
#include <vector>

class Road: public Object
{
public:
    Road(std::string const& roadFile, double width, Terrain* terrain, GLuint shader, GLuint texture, Object* parent = nullptr);

    double getHeight(double x, double z) const;
    double getHeight(vec3 const& pos) const;
private:
    std::vector<vec3> roadPoints{};
    vec3* vertexArray{};
    Terrain* terr;
    Model* createRoad(std::string const& file, double width, Terrain* terrain);

};