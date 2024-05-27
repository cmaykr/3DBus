#pragma once

#include <vector>
#include "VectorUtils4.h"
#include "LittleOBJLoader.h"

class Object
{
public:
	Object() = default;
	Object(GLuint shader, Object* parent = nullptr);
    Object(Model * mdl, GLuint shader, GLuint texture, Object* parent = nullptr);

    void drawObject(mat4 const& projMat, mat4 const& playCam) const;
	virtual void uploadSpecificUniforms(GLuint prog) const;
	virtual void update(double deltaTime);
	vec3 globalPosition() const;
	vec3 relativePosition() const;
	void setPosition(vec3 const& newPos);
	
	vec3 globalRotation() const;
	vec3 relativeRotation() const;
	void setRotation(vec3 const& newRotation);

	vec3 scale() const;
	void setScale(vec3 const& newScale);

	void addModel(Model* mdl, GLuint texture);
	GLuint getShader() const;
	void setSpecialLightBool(bool light);
	void setSplatmap(GLuint splatMap, GLuint texture);

	void addChild(Object* child);

	std::vector<Object*> const& getChildren() const;

	void setAB(vec3 newA[4], vec3 newB[4]);
	vec3* getGlobalAB(); 
	bool checkCollision(Object* collider, GLfloat cutoffRange);

	void setDepthTest(bool depthBool);

protected:
	std::vector<Model*> models;
	GLuint shader;
	bool specialLight{true};
	std::vector<GLuint> textures;
	GLuint splatMap{};
	GLuint splatSpecTex{};
	vec3 _position{};
	vec3 _rotation; // Rotation for x, y, z
	vec3 _scale;

	// AABB
	vec3 A[4] {	vec3(0,0,0),
				vec3(0,0,0), 
				vec3(0,0,0),
				vec3(0,0,0)};
	vec3 B[4] {	vec3(0,0,0),
				vec3(0,0,0), 
				vec3(0,0,0),
				vec3(0,0,0)};
	vec3 AB[8] {vec3(0,0,0),
				vec3(0,0,0), 
				vec3(0,0,0),
				vec3(0,0,0),
				vec3(0,0,0),
				vec3(0,0,0), 
				vec3(0,0,0),
				vec3(0,0,0)};
	vec3 globalAB[8];

	bool depthTest{true};


	Object* parent;

    std::vector<Object*> children;

private:
    bool checkDefaultAB();
};

