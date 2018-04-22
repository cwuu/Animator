
//	resource:http://www.paulsprojects.net/NewBSDLicense.txt

#ifndef METABALL_H
#define METABALL_H

#include "Marching_Cube.h"
#include "vec.h"
#include <vector>

struct Ball 
{
	Vec3f position;
	float squaredRadius;

	Ball(Vec3f newPosition, float newSquaredRadius) {
		position = newPosition;
		squaredRadius = newSquaredRadius;
	}
};

class MetaBall 
{

public:
	MetaBall(float mul = 1.0);
	void draw(float threshold);
	void addBallAbs(Vec3f center, float radius);
	void addBallRel(float x, float y, float z, float radius);
	void cal();
	~MetaBall();
private:
	std::vector<Ball> m_metaBalls;
	MarchingCube* m_marchingCube;
	bool initialized;

};

#endif