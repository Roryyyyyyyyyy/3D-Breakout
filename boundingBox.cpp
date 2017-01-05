#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#ifdef __APPLE__
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib") 
#endif

#include "boundingBox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define PI 3.14159265359

using namespace std;

BoundingBox::BoundingBox(vec3 v0, vec3 v1, float thickness)
{
	float thick = thickness;
	float S = 1 / sqrt(3);
	vert0 = v0;
	vert1 = v1;

	float dx = v1.x - v0.x;
	float dy = v1.y - v0.y;
	float angle = glm::atan(dx / dy);
	float wcb;
	float wcbtoperp;
	float PAngle;
	float dxp;
	float dyp;

	//Check which quadrant
	if (dx <= 0 && dy <= 0)
	{
		wcb = PI + angle;
	}
	else if (dx <= 0 && dy >= 0)
	{
		wcb = (2 * PI) + angle;
	}
	else if (dx >= 0 && dy <= 0)
	{
		wcb = PI - angle;
	}
	else if (dx >= 0 && dy >= 0)
	{
		wcb = angle;
	}

	if (wcb > (2 * PI))
	{
		wcb -= 2 * PI;
	}

	wcbtoperp = wcb + (PI / 2);

	if (wcbtoperp > (2 * PI))
	{
		wcbtoperp -= (2 * PI);
	}

	if (wcbtoperp > PI)
	{
		PAngle = (-2 * PI) + wcbtoperp;
	}
	else
	{
		PAngle = wcbtoperp;
	}

	dxp = thick * glm::sin(PAngle);
	dyp = thick * glm::cos(PAngle);


	vert2.x = v0.x + dxp;
	vert2.y = v0.y + dyp;
	vert2.z = 0.0;
	
	vert3.x = v1.x + dxp;
	vert3.y = v1.y + dyp;
	vert3.z = 0.0;
	

	midpoint.x = (vert1.x + vert2.x) / 2;
	midpoint.y = (vert1.y + vert2.y) / 2;
	midpoint.z = 0.0;

}

void BoundingBox::RotateBounds(float angle)
{
	float rot = 0;
	rot += angle;
	vert0 = rotate(vert0, radians(rot), vec3(0, 0, 1));
	vert1 = rotate(vert1, radians(rot), vec3(0, 0, 1));
	vert2 = rotate(vert2, radians(rot), vec3(0, 0, 1));
	vert3 = rotate(vert3, radians(rot), vec3(0, 0, 1));
	midpoint = rotate(midpoint, radians(rot), vec3(0, 0, 1));
}

void BoundingBox::Display()
{
	cout << vert0.x << ", " << vert0.y << endl;
	cout << " " << endl;
	cout << vert1.x << ", " << vert1.y << endl;
	cout << " " << endl;
	cout << vert2.x << ", " << vert2.y << endl;
	cout << " " << endl;
	cout << vert3.x << ", " << vert3.y << endl;
	cout << " " << endl;
	cout << "Midpoint: " << midpoint.x << ", " << midpoint.y << endl;
	cout << " " << endl;
}

