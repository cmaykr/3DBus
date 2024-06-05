#include "bus.hpp"
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils4.h"

#include <iostream>

Bus::Bus(Model * mdl, GLuint shader, GLuint texture, vec3 const& lightOffset, double acc, double frict, double wheelOffset, Terrain* terr, Road* road, Object* parent)
    : Object(mdl, shader, texture, parent), lightOffset(lightOffset), acc(acc), wheelOffset(wheelOffset), terr(terr), road(road)
{
    if (frict > 0 && frict < 1)
        friction = frict;
    else
        friction = 0.95f;
    
    wheelRotation = 0;
    specialLight = false;
}

void Bus::uploadSpecificUniforms(GLuint prog) const
{
    uploadUniformVec3ToShader(prog, "light1Pos", light1Pos);
    uploadUniformVec3ToShader(prog, "light2Pos", light2Pos);
    uploadUniformVec3ToShader(prog, "light1Dir", light1Dir);
    uploadUniformVec3ToShader(prog, "light2Dir", light2Dir);
    // TODO: Fix to not use constants
    uploadUniformFloatToShader(prog, "outerCosineLimit", 12.0f);
    uploadUniformFloatToShader(prog, "innerCosineLimit", 0.0f);

    Object::uploadSpecificUniforms(prog);
}

void Bus::update(double deltaTime)
{
    double newRot{};
    vec3 newPos = globalPosition();
    lastPos = globalPosition();

    vec3 direction;
    direction.x = -cos(globalRotation().y+1.57);
    direction.z = sin(globalRotation().y+1.57);
    direction.y = 0;

    direction = Rx(relativeRotation().x) * Rz(relativeRotation().z) * direction;

    vec3 rotationPoint;
    if (wheelRotation != 0)
    {
        rotationPoint.x = -2*wheelOffset/sin(wheelRotation);
        rotationPoint.z = -wheelOffset;

        double radii = rotationPoint.x;
        // rotationPoint = T(newPos.x, newPos.y, newPos.z) * Ry(relativeRotation().y) * T(-newPos.x, -newPos.y, -newPos.z) * rotationPoint;
        rotationPoint = Ry(relativeRotation().y) * rotationPoint;
        rotationPoint += globalPosition();

        // Rotation is wrong, angular velocity should be identical to speed.
        vec3 upDir = Rx(relativeRotation().x) * Rz(relativeRotation().z) * vec3(0,1,0);
        vec3 nPos = T(rotationPoint.x, rotationPoint.y, rotationPoint.z) * Ry((speed/(radii))*deltaTime) * T(-rotationPoint.x, -rotationPoint.y, -rotationPoint.z) * globalPosition();
        //vec3 nPos = T(rotationPoint.x, rotationPoint.y, rotationPoint.z) * ArbRotate(upDir, (speed/(radii))*deltaTime) * T(-rotationPoint.x, -rotationPoint.y, -rotationPoint.z) * globalPosition();

        nPos.y = std::max(terr->calculatePointHeight(nPos.x, nPos.z), road->getHeight(nPos))+1;
        //nPos.y = (roadHeight != 0) ? roadHeight :  terr->calculatePointHeight(nPos.x, nPos.z);
        //nPos.y++;
        newPos = nPos;
        setRotation(vec3{globalRotation().x, globalRotation().y + static_cast<GLfloat>(-(speed/(radii))*deltaTime * upDir.y), globalRotation().z});
    }
    else
    {
        newPos += speed*deltaTime*direction;
        //setPosition(newPos + speed*deltaTime*direction);
    }
    newPos.y = std::max(terr->calculatePointHeight(newPos.x, newPos.z), road->getHeight(newPos))+1;
    setPosition(newPos);

    if (wheelRotation != 0)
        wheelRotation += ((wheelRotation > 0) ? -1*deltaTime*speed : 1*deltaTime*speed)/50;

    if (abs(wheelRotation) < 0.001)
        wheelRotation = 0;
    newPos = globalPosition();

    direction.x = -cos(globalRotation().y+1.57);
    direction.z = sin(globalRotation().y+1.57);
    direction.y = 0;
    direction = Rx(relativeRotation().x) * Rz(relativeRotation().z) * direction;

    if (glutKeyIsDown('j'))
    {
        newRot += 1 * deltaTime;
         wheelRotation += 1 * deltaTime; // rotate wheel 1 degree per second.
         if (wheelRotation > 0.523599)
            wheelRotation = 0.523599;
    }
    if (glutKeyIsDown('l'))
    {
        newRot -= 1 * deltaTime;
        wheelRotation -= 1 * deltaTime;

        if (wheelRotation < -0.523599)
            wheelRotation = -0.523599;
    }
    if (glutKeyIsDown('i'))
    {
        newPos.z += cos(newRot) * deltaTime * 10;
        newPos.x += sin(newRot) * deltaTime * 10;

        speed += deltaTime * acc;
    }
    if (glutKeyIsDown('k'))
    {
        newPos.z -= cos(newRot) * deltaTime * 10;
        newPos.x -= sin(newRot) * deltaTime * 10;

        speed -= deltaTime * acc;
    }

    speed *= friction;
    //setPosition(globalPosition() + direction * speed);
    vec3 rot = globalRotation();
    rot.y += newRot * speed;
    //setRotation(rot);

    Object* wheel = children.at(0);
    if (wheel != nullptr)
    {
        vec3 wheelRot{wheel->globalRotation()};
        wheelRot.y = wheelRotation*8-1.57;
        children.at(0)->setRotation(wheelRot);
    }

    // Rotate lights some degree left and right from bus direction.
    light1Dir = Ry(0.1) * direction;
    light1Dir.y = -0.2;
    light2Dir = Ry(-0.1) * direction;
    light2Dir.y = -0.2;
    

    light1Pos = Ry(globalRotation().y) * scale() * (lightOffset) + globalPosition();
    light2Pos = Ry(globalRotation().y) * (vec3(-lightOffset.x, lightOffset.y, lightOffset.z)) * scale() + globalPosition();

    
    Object::update(deltaTime);
}

vec3 Bus::getLastPos() const {
    return lastPos;
}

void Bus::setSpeed(double newSpeed) {
    speed = newSpeed;
}

double Bus::getSpeed() {
    return speed;
}