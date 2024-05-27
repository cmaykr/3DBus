#include "terrain.hpp"
#include "GL/gl.h"
#include "Linux/MicroGlut.h"
#include "GL_utilities.h"
#include <iostream>
#include "LoadTGA.h"

Terrain::Terrain(TextureData *tex, GLuint shader, GLuint texture, Object *parent)
    : Object(shader, parent), width(tex->height)
{
	terr = GenerateTerrain(tex);
	addModel(terr, texture);
}

double Terrain::calculatePointHeight(double x, double z)
{
	if (x < 0 || z < 0)
	{
		return 0;
	}
	int xLLI = x*xScale;
	int zLLI = z*zScale;

	vec3 LLI = terr->vertexArray[xLLI + zLLI * width];
	vec3 ULI =terr->vertexArray[xLLI + (zLLI+1) * width];
	vec3 URI =terr->vertexArray[xLLI+1 + (zLLI+1) * width];
	vec3 LRI =terr->vertexArray[xLLI+1 + (zLLI) * width];


	double w1 = ((URI.x - x)*(URI.z - z))/((URI.x - LLI.x)*(URI.x - LLI.x));
	double w2 = ((URI.x - x)*(z - LLI.z))/((URI.x - LLI.x)*(URI.x - LLI.x));
	double w3 = ((x - LLI.x)*(URI.z - z))/((URI.x - LLI.x)*(URI.x - LLI.x));
	double w4 = ((x - LLI.x)*(z - LLI.z))/((URI.x - LLI.x)*(URI.x - LLI.x));

	return w1*LLI.y+w2*ULI.y+w3*LRI.y+URI.y*w4;
}

Model *Terrain::GenerateTerrain(TextureData *tex)
{
     int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	unsigned int x, z;

	std::cout << tex->width << std::endl;
	
	vec3 *vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *) malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			vertexArray[(x + z * tex->width)].x = x / xScale;
			vertexArray[(x + z * tex->width)].y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / yScale;
			vertexArray[(x + z * tex->width)].z = z / zScale;
			if (x > 0 && z > 0)
			{
				vec3 middle = vec3(x, tex->imageData[(x + (z) * tex->width) * (tex->bpp/8)] / yScale, z);
				vec3 downV = vec3(x, tex->imageData[(x + (z-1) * tex->width) * (tex->bpp/8)] / yScale, z-1) - middle;
				vec3 leftV = vec3(x-1, tex->imageData[(x-1 + (z) * tex->width) * (tex->bpp/8)] / yScale, z) - middle;
				vec3 ldiagV = vec3(x-1, tex->imageData[(x-1 + (z+1) * tex->width) * (tex->bpp/8)] / yScale, z+1) - middle;
				vec3 upV = vec3(x, tex->imageData[(x + (z+1) * tex->width) * (tex->bpp/8)] / yScale, z+1) - middle;
				vec3 rightV = vec3(x+1, tex->imageData[(x+1 + (z) * tex->width) * (tex->bpp/8)] / yScale, z) - middle;
				vec3 rdiagV = vec3(x+1, tex->imageData[(x+1 + (z-1) * tex->width) * (tex->bpp/8)] / yScale, z-1) - middle;
				vec3 t1 = CrossProduct(downV, leftV);
				vec3 t2 = CrossProduct(leftV, ldiagV);
				vec3 t3 = CrossProduct(ldiagV, upV);
				vec3 t4 = CrossProduct(upV, rightV);
				vec3 t5 = CrossProduct(rightV, rdiagV);
				vec3 t6 = CrossProduct(rdiagV, downV);
				vec3 normal = (t1+t2+t3+t4+t5+t6) / 6.0;
				normalArray[(x + z * tex->width)] = normalize(normal);
			}
			else
			{
				normalArray[(x + z * tex->width)] = vec3(0,1,0);
			}
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)].x = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)].y = z; // (float)z / tex->height;
		}

	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 6
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
	
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}

int Terrain::maxWidth() const
{
	return width;
}