#include "assetmanager.hpp"

#include "LoadTGA.h"

AssetManager::AssetManager(std::string const& texturePath, std::string const& modelPath)
    : texPath(texturePath), mdlPath(modelPath)
{

}

AssetManager::~AssetManager()
{
    // Deallocates loaded models.
    for (std::pair<std::string, Model*> mdl: models)
    {
        DisposeModel(mdl.second);
    }
}

/// Convert to use texturedata
void AssetManager::LoadTGATexture(std::string const& textureName)
{
    GLuint tex{};

    LoadTGATextureSimple((texPath + textureName).c_str(), &tex);

    textures.insert(std::pair<std::string, GLuint>{textureName, tex});
}

void AssetManager::LoadSingleModel(std::string const& modelName)
{
    Model* mdl {LoadModel((mdlPath + modelName).c_str())};

    if (mdl != nullptr)
    {
        models.insert(std::pair<std::string, Model*>{modelName, mdl});
    }
}

GLuint AssetManager::getTexture(std::string const& textureName) const
{
    auto it = textures.find(textureName);

    if (it != textures.end())
    {
        return it->second;
    }

    return -1;
}

Model* AssetManager::getModel(std::string const& modelName) const
{
    auto it = models.find(modelName);

    if (it != models.end())
    {
        return it->second;
    }

    return nullptr; // Is this a good idea?
}