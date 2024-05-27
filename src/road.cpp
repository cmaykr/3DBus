#include "road.hpp"
#include <fstream>
#include <iostream>
#include <exception>
#include "VectorUtils4.h"

Road::Road(std::string const& roadFile, double witdh, Terrain* terrain, GLuint shader, GLuint texture, Object* parent)
    : Object(shader, parent), roadPoints(), terr(terrain)
{
    addModel(createRoad(roadFile, witdh, terr), texture);
}

vec3 findIntersection(vec3 p1, vec3 dir1, vec3 p2, vec3 dir2)
{
    double t = (-dir2.z * (p2.x-p1.x) + dir2.x * (p2.z-p1.z))/(dir2.x*dir1.z-dir1.x*dir2.z);
    return vec3(p1.x+t*dir1.x, 0, p1.z+t*dir1.z);
}

double Road::getHeight(double x, double z) const
{
    return getHeight(vec3(x, 0, z));
}

double Road::getHeight(vec3 const& pos) const
{
    size_t i{};

    vec3 shortest{roadPoints.at(0) - pos};
    for (size_t j{1}; j < roadPoints.size(); ++j)
    {
        vec3 dist = roadPoints.at(j) - pos;
        if (Norm(dist) < Norm(shortest))
        {
            shortest = dist;
            i = j;
        }
    }

    vec3 normal{};
    if (i == roadPoints.size()-1)
    {
        normal = normalize(roadPoints.at(i) - roadPoints.at(i-1));
    }
    else
    {
        normal = normalize(roadPoints.at(i+1) - roadPoints.at(i));
    }

    double d = -dot(normal, roadPoints.at(i));

    double side = dot(pos, normal) + d;

    vec3 normalLeft = vec3(normal.z, normal.y,-normal.x);

    if (side > 0 && i == roadPoints.size()-1)
    {
        // Outside road.
        return -999;
    }
    if (side < 0 && i == 0)
    {
        return -999;
    }
    
    if (side < 0)
    {
        i -= 1;
        normal = normalize(roadPoints.at(i+1) - roadPoints.at(i));

        normalLeft = vec3(normal.z, normal.y, -normal.x);
    }

    double leftOuter = -dot(normalLeft, vertexArray[i*4]);
    double leftInner = -dot(normalLeft, vertexArray[i*4+1]);
    double rightInner = -dot(normalLeft, vertexArray[i*4+2]);
    double rightOuter = -dot(normalLeft, vertexArray[i*4+3]);

    if (dot(pos, normalLeft)+leftOuter > 0)
    {
        return -999;
    }
    if (dot(pos, normalLeft) + rightOuter < 0)
    {
        return -999;
    }

    if (dot(pos, normalLeft)+leftInner > 0) // Check if in left most face
    {
        vec3 LLI = vertexArray[i*4];
        vec3 ULI = vertexArray[i*4+4];
        vec3 URI = vertexArray[i*4+5];
        vec3 LRI = vertexArray[i*4+1];
        vec3 polyNorm = normalize(ULI-LRI);
        polyNorm = vec3(polyNorm.z, polyNorm.y, -polyNorm.x);

        double d = -dot(ULI, polyNorm);
        double prod = dot(pos, polyNorm) + d;
        double w1,w2,w3;
        vec3 v1 = ULI;
        vec3 v2 = LRI;
        vec3 v3 = (prod > 0) ? LLI : URI;
        double div =((v2.z-v3.z)*(v1.x-v3.x)+(v3.x-v2.x)*(v1.z-v3.z));
        w1 = ((v2.z-v3.z)*(pos.x-v3.x)+(v3.x-v2.x)*(pos.z-v3.z)) / div;
        w2 = ((v3.z-v1.z)*(pos.x-v3.x)+(v1.x-v3.x)*(pos.z-v3.z)) / div;
        w3 = 1 - w1 - w2;

        return w1*v1.y+w2*v2.y+w3*v3.y;
        
    }
    else if (dot(pos, normalLeft)+rightInner > 0) // In center face
    {
        vec3 LLI = vertexArray[i*4+1];
        vec3 ULI = vertexArray[i*4+5];
        vec3 URI = vertexArray[i*4+6];
        vec3 LRI = vertexArray[i*4+2];

        vec3 polyNorm = normalize(ULI-LRI);
        polyNorm = vec3(polyNorm.z, polyNorm.y, -polyNorm.x);

        double d = -dot(ULI, polyNorm);
        double prod = dot(pos, polyNorm) + d;
        double w1,w2,w3;
        vec3 v1 = ULI;
        vec3 v2 = LRI;
        vec3 v3 = (prod > 0) ? LLI : URI;
        double div =((v2.z-v3.z)*(v1.x-v3.x)+(v3.x-v2.x)*(v1.z-v3.z));
        w1 = ((v2.z-v3.z)*(pos.x-v3.x)+(v3.x-v2.x)*(pos.z-v3.z)) / div;
        w2 = ((v3.z-v1.z)*(pos.x-v3.x)+(v1.x-v3.x)*(pos.z-v3.z)) / div;
        w3 = 1 - w1 - w2;

        return w1*v1.y+w2*v2.y+w3*v3.y;

    }
    else if (dot(pos, normalLeft) + rightOuter > 0) // In right face
    {
        vec3 LLI = vertexArray[i*4+2];
        vec3 ULI = vertexArray[i*4+6];
        vec3 URI = vertexArray[i*4+7];
        vec3 LRI = vertexArray[i*4+3];
        vec3 polyNorm = normalize(ULI-LRI);
        polyNorm = vec3(polyNorm.z, polyNorm.y, -polyNorm.x);

        double d = -dot(ULI, polyNorm);
        double prod = dot(pos, polyNorm) + d;
        double w1,w2,w3;
        vec3 v1 = ULI;
        vec3 v2 = LRI;
        vec3 v3 = (prod > 0) ? LLI : URI;
        double div =((v2.z-v3.z)*(v1.x-v3.x)+(v3.x-v2.x)*(v1.z-v3.z));
        w1 = ((v2.z-v3.z)*(pos.x-v3.x)+(v3.x-v2.x)*(pos.z-v3.z)) / div;
        w2 = ((v3.z-v1.z)*(pos.x-v3.x)+(v1.x-v3.x)*(pos.z-v3.z)) / div;
        w3 = 1 - w1 - w2;

        return w1*v1.y+w2*v2.y+w3*v3.y;
    }
    else
    {
        // Outside road.
        return -999;
    }

    return -999;
}

Model* Road::createRoad(std::string const& file, double width, Terrain* terrain)
{
    std::ifstream ofs{file};

    std::string text{};
    vec3 singlePoint{};
    int delimPos{};
    while (std::getline(ofs, text, '\n'))
    {
        if (text.front() == '#')
            continue;
        else if (text.front() == '(' && text.back() == ')')
        {
            delimPos = text.find(',');
            if (static_cast<size_t>(delimPos) == std::string::npos)
                throw std::logic_error("Delimiter ',' not found in file");
            
            singlePoint.x = stod(text.substr(1, delimPos));
            singlePoint.z = stod(text.substr(delimPos + 1, text.back() - 1));
            singlePoint.y = 0;
            roadPoints.push_back(singlePoint);
        }
        else
        {
            throw std::logic_error("Format of road file is faulty");
        }
    }
    std::cout << "Length: " << roadPoints.size();
    int vertexCount = roadPoints.size()*4;
    int triangleCount = (roadPoints.size()-1)*6;

    std::cout << " Vertex count: " << vertexCount << " triangleCount: " << triangleCount << std::endl;

    vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *) malloc(sizeof(GLuint) * triangleCount*3);

    vec3 roadDir{};
    vec3 ortDir{};
    vec3 lastDir{};
    for (size_t i{0}; i < roadPoints.size(); ++i)
    {
        double w = width;

        if (i < roadPoints.size() - 1)
        {
            roadDir = normalize(roadPoints.at(i+1) - roadPoints.at(i));
            vec3 comp = roadDir - lastDir;
            if ((comp.x != 0 || comp.z != 0) && i != 0)
            {
                comp = (roadDir + lastDir) / 2.0;
                ortDir = vec3(-comp.z, comp.y, comp.x);
                vec3 ortDir1 = vec3(-lastDir.z, lastDir.y, lastDir.x);
                vec3 ortDir2 = vec3(-roadDir.z, roadDir.y, roadDir.x);
                // Weird stuff
                vertexArray[i*4] = findIntersection((-w*3 * ortDir1 + roadPoints.at(i-1)), lastDir, (-w*3 * ortDir2 + roadPoints.at(i)), roadDir);
                vertexArray[i*4+1] = findIntersection((-w * ortDir1 + roadPoints.at(i-1)), lastDir, (-w * ortDir2 + roadPoints.at(i)), roadDir);
                vertexArray[i*4+2] = findIntersection((w * ortDir1 + roadPoints.at(i-1)), lastDir, (w * ortDir2 + roadPoints.at(i)), roadDir);
                vertexArray[i*4+3] = findIntersection((w*3 * ortDir1 + roadPoints.at(i-1)), lastDir, (w*3 * ortDir2 + roadPoints.at(i)), roadDir);
                
            }
            else
            {
                ortDir = vec3(-roadDir.z, roadDir.y, roadDir.x);
                vertexArray[i*4] = -w*3 * ortDir + roadPoints.at(i);
                vertexArray[i*4+1] = -w * ortDir + roadPoints.at(i);
                vertexArray[i*4+2] = w * ortDir + roadPoints.at(i);
                vertexArray[i*4+3] = w*3 * ortDir + roadPoints.at(i);
            }
            indexArray[i*18] = i*4;
            indexArray[i*18+1] = i*4+1;
            indexArray[i*18+2] = i*4+4;

            indexArray[i*18+3] = i*4+5;
            indexArray[i*18+4] = i*4+4;
            indexArray[i*18+5] = i*4+1;

            indexArray[i*18+6] = i*4+1;
            indexArray[i*18+7] = i*4+2;
            indexArray[i*18+8] = i*4+5;

            indexArray[i*18+9] = i*4+6;
            indexArray[i*18+10] = i*4+5;
            indexArray[i*18+11] = i*4+2;

            indexArray[i*18+12] = i*4+2;
            indexArray[i*18+13] = i*4+3;
            indexArray[i*18+14] = i*4+6;

            indexArray[i*18+15] = i*4+7;
            indexArray[i*18+16] = i*4+6;
            indexArray[i*18+17] = i*4+3;

            lastDir = roadDir;
        }
        else
        {
            ortDir = vec3(-lastDir.z, lastDir.y, lastDir.x);
            vertexArray[i*4] = -w*3 * ortDir + roadPoints.at(i);
            vertexArray[i*4+1] = -w * ortDir + roadPoints.at(i);
            vertexArray[i*4+2] = w * ortDir + roadPoints.at(i);
            vertexArray[i*4+3] = w*3 * ortDir + roadPoints.at(i);
        }
        vertexArray[i*4].y = terrain->calculatePointHeight(vertexArray[i*4].x, vertexArray[i*4].z)-5;
        vertexArray[i*4+1].y = terrain->calculatePointHeight(roadPoints.at(i).x, roadPoints.at(i).z)+1;
        vertexArray[i*4+2].y = vertexArray[i*4+1].y;
        vertexArray[i*4+3].y = vertexArray[i*4].y;
        
        if (i != 0)
        {
            vec3 distance = vertexArray[i*4] - vertexArray[0];
            texCoordArray[i*4] = vec2(distance.z/1.0, distance.x/1.0);
 
            distance = vertexArray[i*4+1] - vertexArray[0];
            texCoordArray[i*4+1] = vec2(distance.z/1.0, distance.x/1.0);
            distance = vertexArray[i*4+2] - vertexArray[0];
            texCoordArray[i*4+2] = vec2(distance.z/1.0, distance.x/1.0);
            distance = vertexArray[i*4+3] - vertexArray[0];
            texCoordArray[i*4+3] = vec2(distance.z/1.0, distance.x/1.0);
        }
        else
        {
            texCoordArray[i*4].x = 0;
            texCoordArray[i*4].y = 0; 

            texCoordArray[i*4+1].x = w / 10;
            texCoordArray[i*4+1].y = 0;
            texCoordArray[i*4+2].x = w * 3 / 1;
            texCoordArray[i*4+2].y = 0;
            texCoordArray[i*4+3].x = w * 4 / 1;
            texCoordArray[i*4+3].y = 0;
        }
    }

    for (size_t i{}; i < roadPoints.size(); ++i)
    {
        if(i == 0)
        {
            normalArray[0] = normalize(cross(vertexArray[4] - vertexArray[0], vertexArray[1] - vertexArray[0]) / 2.0);
            vec3 mid {vertexArray[1]};
            vec3 l {vertexArray[0] - mid};
            vec3 lU {vertexArray[4] - mid};
            vec3 u {vertexArray[5] - mid};
            vec3 r {vertexArray[2] - mid};
            vec3 n1 {cross(r, u)};
            vec3 n2 {cross(l, lU)};
            vec3 n3 {cross(lU, l)};
            normalArray[1] = normalize((n1+n2+n3)/3.0);
            mid = vertexArray[2];
            l = vertexArray[1] - mid;
            lU = vertexArray[5] - mid;
            u = vertexArray[6] - mid;
            r = vertexArray[3] - mid;
            n1 = cross(r, u);
            n2 = cross(u, lU);
            n3 = cross(lU, l);
            normalArray[2] = normalize((n1+n2+n3)/3.0);
            mid = vertexArray[3];
            l = vertexArray[2] - mid;
            lU = vertexArray[6] - mid;
            u = vertexArray[7] - mid;
            n1 = cross(u, lU);
            n2 = cross(lU, l);
            normalArray[3] = normalize((n1+n2)/2.0);
        }
        else if (i == roadPoints.size()-1)
        {
            vec3 mid {vertexArray[i*4]};
            vec3 d {vertexArray[(i-1)*4] - mid};
            vec3 dR {vertexArray[(i-1)*4+1] - mid};
            vec3 r = {vertexArray[i*4+1] - mid};
            vec3 n1 = cross(d, dR);
            vec3 n2 = cross(dR, r);
            normalArray[i*4] = normalize((n1+n2)/2.0);
            
            mid = vertexArray[i*4+1];
            d = vertexArray[(i-1)*4+1] - mid;
            dR = vertexArray[(i-1)*4+2] - mid;
            r = vertexArray[i*4+2] - mid;
            vec3 l {vertexArray[i*4] - mid};
            n1 = cross(d, dR);
            n2 = cross(dR, r);
            vec3 n3 {cross(l, d)};
            normalArray[i*4+1] = normalize((n1+n2+n3)/3.0);

            mid = vertexArray[i*4+2];
            d = vertexArray[(i-1)*4+2] - mid;
            dR = vertexArray[(i-1)*4+3] - mid;
            r = vertexArray[i*4+3] - mid;
            l = vertexArray[i*4+1] - mid;
            n1 = cross(d, dR);
            n2 = cross(dR, r);
            n3 = cross(l, d);
            normalArray[i*4+2] = normalize((n1+n2+n3)/3.0);

            mid = vertexArray[i*4+3];
            l = vertexArray[i*4+2] - mid;
            d = vertexArray[(i-1)*4+3] - mid;
            n1 = cross(l, d);
            normalArray[i*4+3] = normalize(n1);
        }
        else
        {
            vec3 mid {vertexArray[i*4]};
            vec3 d {vertexArray[(i-1)*4]-mid};
            vec3 dR {vertexArray[(i-1)*4+1]-mid};
            vec3 r {vertexArray[i*4+1]-mid};
            vec3 u {vertexArray[(i+1)*4]-mid};
            vec3 n1 {cross(d, dR)};
            vec3 n2 {cross(dR, r)};
            vec3 n3 {cross(r, u)};
            normalArray[i*4] = normalize((n1+n2+n3)/3.0);

            mid = vertexArray[i*4+1];
            d = vertexArray[(i-1)*4+1] - mid;
            vec3 l = vertexArray[i*4] - mid;
            vec3 lU = vertexArray[(i+1)*4] - mid;
            u = vertexArray[(i+1)*4+1] - mid;
            r = vertexArray[i*4+2] - mid;
            dR = vertexArray[(i-1)*4+2] - mid;
            n1 = cross(d, dR);
            n2 = cross(dR, r);
            n3 = cross(r, u);
            vec3 n4 = cross(u, lU);
            vec3 n5 = cross(lU, l);
            vec3 n6 = cross(l, d);
            normalArray[i*4+1] = normalize((n1+n2+n3+n4+n5+n6)/6.0);

            mid = vertexArray[i*4+2];
            d = vertexArray[(i-1)*4+2] - mid;
            l = vertexArray[i*4+1] - mid;
            lU = vertexArray[(i+1)*4+1] - mid;
            u = vertexArray[(i+1)*4+2] - mid;
            r = vertexArray[i*4+3] - mid;
            dR = vertexArray[(i-1)*4+3] - mid;
            n1 = cross(lU, l);
            n2 = cross(u, lU);
            n3 = cross(r, u);
            n4 = cross(dR, r);
            n5 = cross(d, dR);
            n6 = cross(l, d);
            normalArray[i*4+2] = normalize((n1+n2+n3+n4+n5+n6)/6.0);

            mid = vertexArray[i*4+3];
            d = vertexArray[(i-1)*4+3]-mid;
            l = vertexArray[i*4+2]-mid;
            u = vertexArray[(i+1)*4+3]-mid;
            lU = vertexArray[(i+1)*4+2] - mid;
            n1 = cross(lU, l);
            n2 = cross(u, lU);
            n3 = cross(l, d);
            normalArray[i*4+3] = normalize((n1+n2+n3)/3.0);
        }
    }

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