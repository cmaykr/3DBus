#pragma once

#include "object.hpp"
#include <vector>
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"

class Terrain : public Object
{
    public:
        //using Object::Object;
        Terrain(TextureData *tex, GLuint shader, GLuint texture, Object* parent = nullptr);

        double calculatePointHeight(double x, double y);
        Model* GenerateTerrain(TextureData* tex);

        int maxWidth() const;

    private:
        double xScale = 0.5;
        double zScale = xScale;
        double yScale = 75;
        int width;

        Model* terr;
};