#pragma once

#include <map>
#include <string>
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include "LittleOBJLoader.h"

class AssetManager
{
public:
    AssetManager(std::string const& texPath, std::string const& mdlPath);
    ~AssetManager();

    void LoadTGATexture(std::string const& textureName);
    void LoadSingleModel(std::string const& modelName);

    GLuint getTexture(std::string const& textureName) const;
    Model* getModel(std::string const& modelName) const;

private:
    std::string texPath;
    std::string mdlPath;
    std::map<std::string, GLuint> textures{};

    std::map<std::string, Model*> models{}; // models needs to be deleted when closing game.
};