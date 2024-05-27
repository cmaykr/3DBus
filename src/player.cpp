#include "player.hpp"

#include <iostream>

vec2 cameraCoord{};

Player::Player(double cameraSpeed, Renderer* renderer)
    : cameraSpeed(cameraSpeed), renderer(renderer)
{
    debugCamera = true;
    cameraPos = vec3(0,0,0);
	cameraLookPos = vec3(0, 0, 1);
}

void Player::setProgram(GLuint programValue)
{
    program = programValue;
}

void Player::mouse(int x, int y)
{
    int deadX = x - 300;
    int deadY = y - 300;

	if (abs(deadX) > 10 || abs(deadY) > 10)
	{
		cameraCoord.x = x - 300;
		cameraCoord.y = y - 300;
		//printf("%d\n", cameraCoord.x);
		glutWarpPointer(300, 300);
		glutPostRedisplay();
	}
}

void Player::moveCamera(vec3 const& direction, double deltaTime)
{
    if (glutKeyIsDown('f'))
    {
        cameraPos += direction * deltaTime * cameraSpeed*3;
        cameraLookPos += direction * deltaTime * cameraSpeed*3;
    }
    else
    {
        cameraPos += direction * deltaTime * cameraSpeed;
        cameraLookPos += direction * deltaTime * cameraSpeed;
    }
}

void Player::keyboard(double deltaTime)
{
    if (glutKeyIsDown('t'))
    {
        exit(0);
    }
    
    if (debugCamera == true) 
    {
        if (objectToFollow == nullptr)
        {
            if (glutKeyIsDown('w'))
            {
                vec3 camDir = normalize(cameraLookPos - cameraPos);
                moveCamera(camDir, deltaTime);
            }
            if (glutKeyIsDown('s'))
            {
                vec3 camDir = normalize(cameraLookPos - cameraPos);
                moveCamera(-camDir, deltaTime);
                
            }
            if (glutKeyIsDown('a'))
            {
                vec3 leftDir = normalize(cross((cameraLookPos - cameraPos), vec3(0, 1, 0)));
                moveCamera(-leftDir, deltaTime);
            }
            if (glutKeyIsDown('d'))
            {
                vec3 leftDir = normalize(cross((cameraLookPos - cameraPos), vec3(0, 1, 0)));
                moveCamera(leftDir, deltaTime);
            }
            if (glutKeyIsDown('e'))
            {
                moveCamera(vec3(0,1,0), deltaTime);
            }
            if (glutKeyIsDown('q'))
            {
                moveCamera(vec3(0,-1,0), deltaTime);
            }
        }
        else
        {
            vec3 camDir = normalize(cameraLookPos - cameraPos);
            vec3 offset;
            cameraPos = objectToFollow->globalPosition();
            offset.y = 1.8 * objectToFollow->scale().y;
            cameraPos.y += offset.y;
            double x = -1.9 * cos(objectToFollow->globalRotation().y+1.57);
            double y = 1.9 * sin(objectToFollow->globalRotation().y+1.57);
            cameraPos.x += x;
            cameraPos.z += y;

            float objectDeltaYRot = objectLastUpdateRot.y - objectToFollow->globalRotation().y;

            camDir = Ry(-objectDeltaYRot) * camDir;
            //uploadUniformVec3ToShader(objectToFollow->getShader(), "objectDir", camDir);

            cameraLookPos = vec3(cameraPos.x+camDir.x, cameraPos.y+camDir.y, cameraPos.z+camDir.z);
        }
    }    
    
}

void Player::updateCamera(double deltaTime)
{

    keyboard(deltaTime);

    vec3 camDir = normalize(cameraLookPos - cameraPos);

	mat3 rotY;
  //std::cout << "Camera coord: " << cameraCoord.x << " " << cameraCoord.y << std::endl;
	if (cameraCoord.x < -1)
	{
		rotY = Ry(-cameraCoord.x / 100);
	}
	else if (cameraCoord.x > 1)
	{
		rotY = Ry(-cameraCoord.x/100);
	}
	else
		rotY = Ry(0);

	cameraCoord.x = 0;

	mat3 rotX;
	camDir = ArbRotate(cross(camDir, vec3(0.0, 1.0, 0.0)), -cameraCoord.y/100) * camDir;
	cameraCoord.y = 0;
    cameraCoord.x = 0;
	camDir = rotY * camDir;

	cameraLookPos = vec3(cameraPos.x+camDir.x, cameraPos.y+camDir.y, cameraPos.z+camDir.z);
	camera = lookAtv(cameraPos, cameraLookPos, vec3(0, 1, 0));

    renderer->playerCamera = camera;
    glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_TRUE, camera.m);
    //glUniformMatrix4fv(glGetUniformLocation(6, "camera"), 1, GL_TRUE, camera.m);

    if (objectToFollow != nullptr)
        objectLastUpdateRot = objectToFollow->globalRotation();

}

void Player::setObjectToFollow(Object* obj)
{
    objectToFollow = obj;
}

void Player::setPosition(vec3 const& pos)
{
    vec3 camDir = normalize(cameraLookPos - cameraPos);
    cameraPos = pos;
    cameraLookPos = vec3(cameraPos.x+camDir.x, cameraPos.y+camDir.y, cameraPos.z+camDir.z);
}

void mouse(int x, int y)
{
    if (x != 300 || y != 300)
	{
		cameraCoord.x = x - 300;
		cameraCoord.y = y - 300;
		//printf("%d\n", cameraCoord.x);
		glutWarpPointer(300, 300);
		glutPostRedisplay();
	}
}

vec3 Player::getCameraPos() {
    return cameraPos;
}