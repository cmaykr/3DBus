#pragma once

#include "object.hpp"
#include "terrain.hpp"
#include "road.hpp"
#include "VectorUtils4.h"

/// @brief
/// @param friction: Dictates the maximum speed of the bus.
class Bus: public Object
{
public:
    Bus() = default;
    Bus(Model * mdl, GLuint shader, GLuint texture, vec3 const& lightOffset, double acc, double friction, double wheelOffset, Terrain* terr, Road* road, Object* parent = nullptr);

    void uploadSpecificUniforms(GLuint prog) const override;
    void update(double deltaTime) override;

    vec3 getLastPos() const;
    void setSpeed(double newSpeed);
    double getSpeed();

private:
    vec3 light1Pos;
    vec3 light2Pos;
    vec3 lightOffset;
    vec3 light1Dir{};
    vec3 light2Dir{};
    vec3 lastPos{};

    double acc{};
    double speed{};
    double friction{}; // False friction (Only for decel)
    double wheelRotation{};
    double wheelOffset;
    double test{};

    Terrain *terr;
    Road *road;

};