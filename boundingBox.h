#include <glm/glm.hpp>

using namespace glm;

class BoundingBox
{
public:
	vec3 vert0;
	vec3 vert1;
	vec3 vert2;
	vec3 vert3;

	vec3 midpoint;
	bool active = true;

	BoundingBox(vec3, vec3, float);
	void RotateBounds(float);
	void Display();
};
