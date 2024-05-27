#include "object.hpp"
#include "GL/gl.h"
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include <iostream>
#include <cmath>

Object::Object(GLuint shader, Object* parent)
	: models(), shader(shader), textures(), _position(vec3(0,0,0)), _rotation(vec3(0,0,0)), _scale(vec3(1.0)), parent(parent), children()
{}

Object::Object(Model *mdl, GLuint shader, GLuint texture, Object* parent)
    : Object(shader, parent)
{
	if (mdl != nullptr)
	{
		models.push_back(mdl);
		textures.push_back(texture);
	}
	//std::cout << "texture int: " << texture << std::endl;
}

void Object::drawObject(mat4 const& projMat, mat4 const& playCam) const
{
	vec3 scale = this->scale();
	mat4 trans, rot, scaling, total;
	trans = T(globalPosition().x, globalPosition().y, globalPosition().z);
	rot = Rx(relativeRotation().x) * Ry(relativeRotation().y) * Rz(relativeRotation().z);
	scaling = S(scale.x, scale.y, scale.z);
	if (parent != nullptr)
	{
		vec3 pRot = parent->globalRotation();
		vec3 pPos = parent->globalPosition();
		mat4 pTrans = T(pPos.x, pPos.y, pPos.z);
		mat4 pTrans2 = T(-pPos.x, -pPos.y, -pPos.z);
		total = pTrans * Ry(pRot.y) * pTrans2 * trans * rot * scaling;
	}
	else
		total = trans * rot * scaling;
	glUseProgram(shader);
	if (splatMap != 0)
	{
		GLint splatIndex{3};
		GLint tex2Index{5};
		glUniform1i(glGetUniformLocation(shader, "splatMap"), splatIndex);
		glActiveTexture(GL_TEXTURE0 + splatIndex);
		glBindTexture(GL_TEXTURE_2D, splatMap);
		glUniform1i(glGetUniformLocation(shader, "texUnit2"), tex2Index);
		glActiveTexture(GL_TEXTURE0 + tex2Index);
		glBindTexture(GL_TEXTURE_2D, splatSpecTex);
	}
	glUniformMatrix4fv(glGetUniformLocation(shader, "camera"), 1, GL_TRUE, playCam.m);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projMatrix"), 1, GL_TRUE, projMat.m);
	glUniformMatrix4fv(glGetUniformLocation(shader, "mdlMatrix"), 1, GL_TRUE, total.m);
	uploadUniformFloatToShader(shader, "shouldSpotlight", specialLight);

	glActiveTexture(GL_TEXTURE0);
	for (size_t i{}; i < models.size(); ++i)
	{
		//glUniform1i(glGetUniformLocation(shader, "texUnit2"), textures.at(i));
		glBindTexture(GL_TEXTURE_2D, textures.at(i));
		if (!depthTest)  {
			glDisable(GL_DEPTH_TEST);
			glUniform1f(glGetUniformLocation(shader, "shadeOn"), 0);
		}
		DrawModel(models.at(i), shader, "in_Position", "normal", "inTexCoord");
		glEnable(GL_DEPTH_TEST);
		glUniform1f(glGetUniformLocation(shader, "shadeOn"), 1);
	}

	for (Object* child: children)
	{
		child->drawObject(projMat, playCam);
	}
}

void Object::uploadSpecificUniforms(GLuint prog) const
{
	for (Object* child: children)
	{
		child->uploadSpecificUniforms(prog);
	}
}

void Object::update(double deltaTime)
{
	for (Object* child: children)
	{
		child->update(deltaTime);
	}
}

vec3 Object::globalPosition() const
{
	if (parent == nullptr)
		return relativePosition();

	return _position + parent->globalPosition();
}

vec3 Object::relativePosition() const
{
	return _position;
}

void Object::setPosition(vec3 const& newPos)
{
	_position = newPos;
}

vec3 Object::globalRotation() const
{
	if (parent == nullptr)
		return relativeRotation();

	return _rotation + parent->globalRotation();
}

vec3 Object::relativeRotation() const
{
	return _rotation;
}

void Object::setRotation(vec3 const& newRotation)
{
	_rotation = newRotation;
}

vec3 Object::scale() const
{
	return _scale;
}

void Object::setScale(vec3 const& newScale)
{
	_scale = newScale;
}

void Object::addModel(Model* mdl, GLuint texture)
{
	models.push_back(mdl);
	textures.push_back(texture);
}

GLuint Object::getShader() const
{
	return shader;
}

void Object::setSpecialLightBool(bool light)
{
	specialLight = light;
}

void Object::addChild(Object* child)
{
	child->parent = this;
	children.push_back(child);
}

std::vector<Object*> const& Object::getChildren() const
{
	return children;
}

void Object::setAB(vec3 newA[4], vec3 newB[4]) {
	A[0] = newA[0];
	A[1] = newA[1];
	A[2] = newA[2];
	A[3] = newA[3];
	B[0] = newB[0];
	B[1] = newB[1];
	B[2] = newB[2];
	B[3] = newB[3];

	AB[0] = newA[0];
	AB[1] = newA[1];
	AB[2] = newA[2];
	AB[3] = newA[3];
	AB[4] = newB[0];
	AB[5] = newB[1];
	AB[6] = newB[2];
	AB[7] = newB[3];
}

vec3* Object::getGlobalAB() {
	globalAB[0] =		AB[0] + globalPosition();
	globalAB[1] =		AB[1] + globalPosition();
	globalAB[2] =		AB[2] + globalPosition();
	globalAB[3] =		AB[3] + globalPosition();
	globalAB[4] =		AB[4] + globalPosition();
	globalAB[5] =		AB[5] + globalPosition();
	globalAB[6] =		AB[6] + globalPosition();
	globalAB[7] =		AB[7] + globalPosition();

	return globalAB;
}

bool Object::checkCollision(Object* collider, GLfloat cutoffRange) {

	if (this->checkDefaultAB() || collider->checkDefaultAB()) {
		return false;
	}

	vec3 delta = this->globalPosition() - collider->globalPosition();
	GLfloat distance = sqrt((delta.x*delta.x) + (delta.y*delta.y) + (delta.z*delta.z));

	if (distance >= cutoffRange)
	{
		return false;
	}
	else 
	{
		return(this->getGlobalAB()[0].x > collider->getGlobalAB()[6].x &&
		this->getGlobalAB()[6].x < collider->getGlobalAB()[0].x &&
		this->getGlobalAB()[0].y > collider->getGlobalAB()[6].y &&
		this->getGlobalAB()[6].y < collider->getGlobalAB()[0].y &&
		this->getGlobalAB()[0].z > collider->getGlobalAB()[6].z &&
		this->getGlobalAB()[6].z < collider->getGlobalAB()[0].z);
	}
}

void Object::setDepthTest(bool depthBool) {
	depthTest = depthBool;
}

bool Object::checkDefaultAB() {
	for (int i = 0; i < 8; i++) {
		if (AB[i].x != 0 || AB[i].y != 0 || AB[i].y != 0) {
			return false;
		}
	}
	return true;
}

void Object::setSplatmap(GLuint splat, GLuint texture)
{
	this->splatMap = splat;
	
	splatSpecTex = texture;
}