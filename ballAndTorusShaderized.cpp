#include <cmath>
#include <iostream>
#include <fstream>

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader.h"
#include "hemisphere.h"
#include "bat.h"
#include "material.h"
#include "light.h"
#include "brick.h"
#include "boundingBox.h"
#include "disc.h"
#include "getbmp.h"


using namespace std;
using namespace glm;

static enum object { HEMISPHERE, BAT, BRICK, DISC }; // VAO ids.
static enum buffer { HEM_VERTICES, HEM_INDICES, BAT_VERTICES, BAT_INDICES, BAT_TRANSFORM_MATS, BRICK_VERTICES, BRICK_INDICES, BRICK_TRANSFORM_MATS, DISC_VERTICES, BRICK_COLOR }; // VBO ids.

#define BRICK_NO 16
#define BRICK_RAD 12
#define BRICK_WEIGHT 5

// Globals.
static float latAngle = 0.0; // Latitudinal angle.
static float longAngle = 0.0; // Longitudinal angle.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0; // Animated?
static int animationPeriod = 33; // Time interval between frames.
static const vec4 globAmb = vec4(0.2, 0.2, 0.2, 1.0);
static mat3 normalMat = mat3(1.0);
static float batRot = 0.0f;
static mat4 batTransformMats[4];
static mat4 brickTransformMats[BRICK_NO];
vec3 ballDir(0.25, 0.0, 0.0);
vec3 ballLoc(15.0, 0.0, 1.0);
int score = 0;
int lives = 3;

vec3 baseBallDir(0.25, 0.0, 0.0);
vec3 baseBallLoc(15.0, 0.0, 1.0);
int lastHit = 1000;

bool brick1 = true;
bool brick2 = true;
bool brick3 = true;
bool brick4 = true;
bool brick5 = true;
bool brick6 = true;
bool brick7 = true;
bool brick8 = true;
bool brick9 = true;
bool brick10 = true;
bool brick11 = true;
bool brick12 = true;
bool brick13 = true;
bool brick14 = true;
bool brick15 = true;
bool brick16 = true;

float brick9Z = 2.0f;
float brick10Z = 2.0f;
float brick11Z = 2.0f;
float brick12Z = 2.0f;
float brick13Z = 2.0f;
float brick14Z = 2.0f;
float brick15Z = 2.0f;
float brick16Z = 2.0f;

// Hemisphere data.
static Vertex hemVertices[(HEM_LONGS + 1) * (HEM_LATS + 1)]; 
static unsigned int hemIndices[HEM_LATS][2*(HEM_LONGS+1)]; 
static int hemCounts[HEM_LATS];
static void* hemOffsets[HEM_LATS]; 
static vec4 hemColors = vec4(HEM_COLORS); 

// Bat data.
static Vertex batVertices[BAT_VERTEX_COUNT];
static unsigned int batIndices[BAT_FACES][BAT_VERTICES_PER_FACE];
static int batCounts[BAT_FACES];
static void* batOffsets[BAT_FACES];
static vec4 batColors = vec4(BAT_COLORS);

// Brick data.
static Vertex brickVertices[BRICK_VERTEX_COUNT];
static unsigned int brickIndices[BRICK_FACES][BRICK_VERTICES_PER_FACE];
static int brickCounts[BRICK_FACES];
static void* brickOffsets[BRICK_FACES];
//static vec4 brickColors = vec4(BRICK_COLORS);
static vec4 brickColors[16] = {
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2),
	vec4(BRICK_COLORS),
	vec4(BRICK_COLORS_2)
};


static Vertex discVertices[DISC_SEGS];

// Bat material properties.
static const Material batMaterial =
{
	vec4(0.0, 0.8, 0.0, 1.0),
	vec4(0.0, 0.8, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 30.0, 1.0),
	50.0
};

static const Material brickMaterial =
{
	vec4(0.0, 0.8, 0.0, 1.0),
	vec4(0.0, 0.8, 0.8, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 30.0, 1.0),
	50.0
};

static const Material ballMaterial =
{
	vec4(0.0, 0.8, 0.0, 1.0),
	vec4(0.0, 0.8, 0.8, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 0.0, 30.0, 1.0),
	50.0
};

// Light properties.
static const Light light0 =
{
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 1.5, 3.0, 0.0)
};

//Bat Bounds

BoundingBox bat1Bounds1(
	vec3(23, 0, 0),
	vec3(23 * glm::cos(radians(20.0)), 23 * glm::sin(radians(20.0)), 0),
	4.0
	);

BoundingBox bat1Bounds2(
	vec3(23 * glm::cos(radians(20.0)), 23 * glm::sin(radians(20.0)), 0),
	vec3(23 * glm::cos(radians(40.0)), 23 * glm::sin(radians(40.0)), 0),
	4.0
	);

BoundingBox bat1Bounds3(
	vec3(23 * glm::cos(radians(40.0)), 23 * glm::sin(radians(40.0)), 0),
	vec3(23 * glm::cos(radians(60.0)), 23 * glm::sin(radians(60.0)), 0),
	4.0
	);

BoundingBox bat2Bounds1(
	vec3(23 * glm::cos(radians(120.0)), 23 * glm::sin(radians(120.0)), 0),
	vec3(23 * glm::cos(radians(140.0)), 23 * glm::sin(radians(140.0)), 0),
	4.0
	);

BoundingBox bat2Bounds2(
	vec3(23 * glm::cos(radians(140.0)), 23 * glm::sin(radians(140.0)), 0),
	vec3(23 * glm::cos(radians(160.0)), 23 * glm::sin(radians(160.0)), 0),
	4.0
	);

BoundingBox bat2Bounds3(
	vec3(23 * glm::cos(radians(160.0)), 23 * glm::sin(radians(160.0)), 0),
	vec3(23 * glm::cos(radians(180.0)), 23 * glm::sin(radians(180.0)), 0),
	4.0
	);

BoundingBox bat3Bounds1(
	vec3(23 * glm::cos(radians(240.0)), 23 * glm::sin(radians(240.0)), 0),
	vec3(23 * glm::cos(radians(260.0)), 23 * glm::sin(radians(260.0)), 0),
	-4.0
	);

BoundingBox bat3Bounds2(
	vec3(23 * glm::cos(radians(260.0)), 23 * glm::sin(radians(260.0)), 0),
	vec3(23 * glm::cos(radians(280.0)), 23 * glm::sin(radians(280.0)), 0),
	4.0
	);


BoundingBox bat3Bounds3(
	vec3(23 * glm::cos(radians(280.0)), 23 * glm::sin(radians(280.0)), 0),
	vec3(23 * glm::cos(radians(300.0)), 23 * glm::sin(radians(300.0)), 0),
	4.0
	);

//brick bounds///////////////////////////////////////////////////////////////////////////////////////////////
BoundingBox brick1bounds(
	vec3(BRICK_RAD * glm::cos(radians(45.0)), BRICK_RAD * glm::sin(radians(45.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(0.0)), BRICK_RAD * glm::sin(radians(0.0)), 0),
	4.0
	);

BoundingBox brick2bounds(
	vec3(BRICK_RAD * glm::cos(radians(90.0)), BRICK_RAD * glm::sin(radians(90.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(45.0)), BRICK_RAD * glm::sin(radians(45.0)), 0),
	-4.0
	);

BoundingBox brick3bounds(
	vec3(BRICK_RAD * glm::cos(radians(135.0)), BRICK_RAD * glm::sin(radians(135.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(90.0)), BRICK_RAD * glm::sin(radians(90.0)), 0),
	4.0
	);

BoundingBox brick4bounds(
	vec3(BRICK_RAD * glm::cos(radians(180.0)), BRICK_RAD * glm::sin(radians(180.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(135.0)), BRICK_RAD * glm::sin(radians(135.0)), 0),
	4.0
	);

BoundingBox brick5bounds(
	vec3(BRICK_RAD * glm::cos(radians(225.0)), BRICK_RAD * glm::sin(radians(225.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(180.0)), BRICK_RAD * glm::sin(radians(180.0)), 0),
	4.0
	);

BoundingBox brick6bounds(
	vec3(BRICK_RAD * glm::cos(radians(270.0)), BRICK_RAD * glm::sin(radians(270.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(225.0)), BRICK_RAD * glm::sin(radians(225.0)), 0),
	4.0
	);

BoundingBox brick7bounds(
	vec3(BRICK_RAD * glm::cos(radians(315.0)), BRICK_RAD * glm::sin(radians(315.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(270.0)), BRICK_RAD * glm::sin(radians(270.0)), 0),
	4.0
	);

BoundingBox brick8bounds(
	vec3(BRICK_RAD * glm::cos(radians(360.0)), BRICK_RAD * glm::sin(radians(360.0)), 0),
	vec3(BRICK_RAD * glm::cos(radians(315.0)), BRICK_RAD * glm::sin(radians(315.0)), 0),
	4.0
	);

int findClosestBounds()
{
	BoundingBox batBounds[17] = { bat1Bounds1, bat1Bounds2, bat1Bounds3, bat2Bounds1, bat2Bounds2, bat2Bounds3, bat3Bounds1, bat3Bounds2 , bat3Bounds3, brick1bounds, brick2bounds, brick3bounds, brick4bounds, brick5bounds, brick6bounds, brick7bounds, brick8bounds };

	float currentDist;
	float distance;
	float shortestDist = 1000000000;
	int closestInst;

	//Find the closest bounding box
	for (int i = 0; i < 17; i++)
	{
		currentDist = sqrt(((batBounds[i].midpoint.x - ballLoc.x) * (batBounds[i].midpoint.x - ballLoc.x)) + ((batBounds[i].midpoint.y - ballLoc.y) * (batBounds[i].midpoint.y - ballLoc.y)));
		if (currentDist < shortestDist)
		{
			shortestDist = currentDist;
			closestInst = i;
		}
	}

	return closestInst;
}

void ReenableCollisions()
{
	brick1bounds.active = true;
	brick2bounds.active = true;
	brick3bounds.active = true;
	brick4bounds.active = true;
	brick5bounds.active = true;
	brick6bounds.active = true;
	brick7bounds.active = true;
	brick8bounds.active = true;
}

void SaveScore()
{
	ofstream saveFile;
	saveFile.open(("scoreFile.txt"));
	saveFile << "Last Score: " << score;
	saveFile.close();
}

bool CheckAllBlocks()
{
	bool bricks[8] = { brick9, brick10, brick11, brick12, brick13, brick14, brick15, brick16 };
	bool stillBricks = false;

	for (int i = 0; i < 8; i++)
	{
		if (bricks[i] == true)
		{
			stillBricks = true;
		}
	}
	return stillBricks;
}

void checkBricks(int boxHit)
{

	if (boxHit == 9)
	{
		if (brick1 == true)
		{
			brick1 = false;
		}
		else if (brick9 == true)
		{
			brick9 = false;
			brick1bounds.active = false;
		}
	}
	if (boxHit == 10)
	{
		if (brick2 == true)
		{
			brick2 = false;
		}
		else if (brick10 == true)
		{
			brick10 = false;
			brick2bounds.active = false;
		}
	}
	if (boxHit == 11)
	{
		if (brick3 == true)
		{
			brick3 = false;
		}
		else if (brick11 == true)
		{
			brick11 = false;
			brick3bounds.active = false;
		}
	}
	if (boxHit == 12)
	{
		if (brick4 == true)
		{
			brick4 = false;
		}
		else if (brick12 == true)
		{
			brick12 = false;
			brick4bounds.active = false;
		}
	}
	if (boxHit == 13)
	{
		if (brick5 == true)
		{
			brick5 = false;
		}
		else if (brick13 == true)
		{
			brick13 = false;
			brick5bounds.active = false;
		}
	}
	if (boxHit == 14)
	{
		if (brick6 == true)
		{
			brick6 = false;
		}
		else if (brick14 == true)
		{
			brick14 = false;
			brick6bounds.active = false;
		}
	}
	if (boxHit == 15)
	{
		if (brick7 == true)
		{
			brick7 = false;
		}
		else if (brick15 == true)
		{
			brick15 = false;
			brick7bounds.active = false;
		}
	}
	if (boxHit == 16)
	{
		if (brick8 == true)
		{
			brick8 = false;
		}
		else if (brick16 == true)
		{
			brick16 = false;
			brick8bounds.active = false;
		}
	}
	if (CheckAllBlocks() == false)
	{
		cout << "all gone" << endl;
		brick1 = true;
		brick2 = true;
		brick3 = true;
		brick4 = true;
		brick5 = true;
		brick6 = true;
		brick7 = true;
		brick8 = true;
		brick9 = true;
		brick10 = true;
		brick11 = true;
		brick12 = true;
		brick13 = true;
		brick14 = true;
		brick15 = true;
		brick16 = true;
		SaveScore();
		brick9Z = 2.0f;
		brick10Z = 2.0f;
		brick11Z = 2.0f;
		brick12Z = 2.0f;
		brick13Z = 2.0f;
		brick14Z = 2.0f;
		brick15Z = 2.0f;
		brick16Z = 2.0f;
		ReenableCollisions();
	}
}

void ballBatHitCheck()
{
	BoundingBox batBounds[17] = { bat1Bounds1, bat1Bounds2, bat1Bounds3, bat2Bounds1, bat2Bounds2, bat2Bounds3, bat3Bounds1, bat3Bounds2, bat3Bounds3, brick1bounds, brick2bounds, brick3bounds, brick4bounds, brick5bounds, brick6bounds, brick7bounds, brick8bounds };

	int boxHit = findClosestBounds();
	

	vec3 transformPoint = batBounds[boxHit].midpoint;


	vec3 v0 = batBounds[boxHit].vert0;
	vec3 v1 = batBounds[boxHit].vert1;
	vec3 v2 = batBounds[boxHit].vert2;
	vec3 v3 = batBounds[boxHit].vert3;
	vec3 ballPos = ballLoc;


	//Move Midpoint of box to origin, translate all points along the same vector

	v0.x = v0.x - transformPoint.x;
	v0.y = v0.y - transformPoint.y;

	v1.x = v1.x - transformPoint.x;
	v1.y = v1.y - transformPoint.y;

	v2.x = v2.x - transformPoint.x;
	v2.y = v2.y - transformPoint.y;

	v3.x = v3.x - transformPoint.x;
	v3.y = v3.y - transformPoint.y;

	ballPos.x = ballPos.x - transformPoint.x;
	ballPos.y = ballPos.y - transformPoint.y;



	//Determine angle to rotate to create Axis Aligned Bounding Box
	float dx = v1.x - v0.x;
	float dy = v1.y - v0.y;
	float angle = glm::atan(dx / dy);

	v0 = rotate(v0, angle, vec3(0, 0, 1));
	v1 = rotate(v1, angle, vec3(0, 0, 1));
	v2 = rotate(v2, angle, vec3(0, 0, 1));
	v3 = rotate(v3, angle, vec3(0, 0, 1));
	ballPos = rotate(ballPos, angle, vec3(0, 0, 1));

	vec3 verts[4] = { v0, v1, v2, v3 };



	//Test if ball is within the bounds
	float highX = NULL;
	float lowX = NULL;
	float highY = NULL;
	float lowY = NULL;
	int pointsInX = 0;
	int pointsInY = 0;
	//determine high and low points
	for (int i = 0; i < 4; i++)
	{
		if (highY == NULL) highY = verts[i].y;
		if (lowY == NULL) lowY = verts[i].y;
		if (highX == NULL) highX = verts[i].x;
		if (lowX == NULL) lowX = verts[i].x;
		if (verts[i].y > highY) 
		{
			highY = verts[i].y;
		}
		if (verts[i].y < lowY)
		{
			lowY = verts[i].y;
		}
		if (verts[i].x > highX)
		{
			highX = verts[i].x;
		}
		if (verts[i].x < lowX)
		{
			lowX = verts[i].x;
		}
	}

	float ballRad = 1;

	if (ballPos.x - ballRad > lowX && ballPos.x - ballRad < highX)
	{
		pointsInX++;
	}
	if (ballPos.x + ballRad < highX && ballPos.x + ballRad > lowX)
	{
		pointsInX++;
	}
	if (ballPos.y - ballRad > lowY && ballPos.y - ballRad < highY)
	{
		pointsInY++;
	}
	if (ballPos.y + ballRad < highY && ballPos.y + ballRad > lowY)
	{
		pointsInY++;
	}
	//If at least 2 points are in box, collision = true;

	if ((pointsInX >= 1) && (pointsInY >= 1))
	{
		if (lastHit == boxHit)
		{
			if (boxHit <= 8)
			{
				vec3 normal = normalize(batBounds[boxHit].midpoint);
				lastHit = boxHit;
				ballDir = (ballDir - ((2.0f * dot(ballDir, normal)) * normal));
			}
		}
		else
		{
			if (batBounds[boxHit].active == true)
			{
				vec3 normal = normalize(batBounds[boxHit].midpoint);
				lastHit = boxHit;
				ballDir = (ballDir - ((2.0f * dot(ballDir, normal)) * normal));
				checkBricks(boxHit);
				ballDir = ballDir * 1.05f;
				if (boxHit > 8)
				{
					score += 5;
					cout << "Score: " << score << endl;
				}
			}
			
		}
		

	}
}




void rotateBounds(float angle)
{
	bat1Bounds1.RotateBounds(angle);
	bat1Bounds2.RotateBounds(angle);
	bat1Bounds3.RotateBounds(angle);

	bat2Bounds1.RotateBounds(angle);
	bat2Bounds2.RotateBounds(angle);
	bat2Bounds3.RotateBounds(angle);

	bat3Bounds1.RotateBounds(angle);
	bat3Bounds2.RotateBounds(angle);
	bat3Bounds3.RotateBounds(angle);
}


void applyFriction()
{
	if (length(ballDir) > 0.1)
	{
		ballDir = ballDir - ballDir / 500.0f;
	}
}

void applyGrav()
{
	if (length(ballLoc) > 30)
	{
		ballDir.z = ballDir.z - (length(ballDir)/5);
	}
	if (ballLoc.z < -30)
	{
		ballLoc = baseBallLoc;
		ballDir = baseBallDir;
		score -= 10;
		if (lives > 1)
		{
			lives--;
		}
		else
		{
			SaveScore();
			lives = 3;
			score = 0;
		}
		
		cout << "Score: " << score << endl;
	}
}


static mat4 modelViewMat = mat4(1.0);
static mat4 projMat = mat4(1.0);

static unsigned int
   programId,
   vertexShaderId,
   fragmentShaderId,
   modelViewMatLoc,
   normalMatLoc,
   projMatLoc,
   canTopTexLoc,
   objectLoc,
   hemColorLoc,
   batColorLoc,
   brickColorLoc,
   buffer[14], 
   vao[4],
   texture[1], 
   width,
   height;

static BitMapFile *image[1]; // Local storage for bmp image data.



void fillBatTransformMats(void)
{
	batTransformMats[0] = mat4(1.0);

	batTransformMats[1] = mat4(1.0);
	batTransformMats[1] = rotate(batTransformMats[1], radians(120.0f), vec3(0.0, 0.0, 1.0));

	batTransformMats[2] = mat4(1.0);
	batTransformMats[2] = rotate(batTransformMats[2], radians(240.0f), vec3(0.0, 0.0, 1.0));
}

void fillBrickTransformMats(void)
{
	float zCoord;

	if (brick1 == true)
	{
		brickTransformMats[0] = mat4(1.0);
	}
	else
	{
		brickTransformMats[0] = translate(brickTransformMats[0], vec3(0.0, 0.0, -10000.0));

	}
	if (brick2 == true)
	{
		brickTransformMats[1] = mat4(1.0);
		brickTransformMats[1] = rotate(brickTransformMats[1], radians(45.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[1] = translate(brickTransformMats[1], vec3(0.0, 0.0, -10000.0));

	}
	if (brick3 == true)
	{
		brickTransformMats[2] = mat4(1.0);
		brickTransformMats[2] = rotate(brickTransformMats[2], radians(90.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[2] = translate(brickTransformMats[2], vec3(0.0, 0.0, -10000.0));

	}
	if (brick4 == true)
	{
		brickTransformMats[3] = mat4(1.0);
		brickTransformMats[3] = rotate(brickTransformMats[3], radians(135.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[3] = translate(brickTransformMats[3], vec3(0.0, 0.0, -10000.0));

	}
	if (brick5 == true)
	{
		brickTransformMats[4] = mat4(1.0);
		brickTransformMats[4] = rotate(brickTransformMats[4], radians(180.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[4] = translate(brickTransformMats[4], vec3(0.0, 0.0, -10000.0));

	}
	if (brick6 == true)
	{
		brickTransformMats[5] = mat4(1.0);
		brickTransformMats[5] = rotate(brickTransformMats[5], radians(225.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[5] = translate(brickTransformMats[5], vec3(0.0, 0.0, -10000.0));

	}
	if (brick7 == true)
	{
		brickTransformMats[6] = mat4(1.0);
		brickTransformMats[6] = rotate(brickTransformMats[6], radians(270.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[6] = translate(brickTransformMats[6], vec3(0.0, 0.0, -10000.0));

	}
	if (brick8 == true)
	{
		brickTransformMats[7] = mat4(1.0);
		brickTransformMats[7] = rotate(brickTransformMats[7], radians(315.0f), vec3(0.0, 0.0, 1.0));
	}
	else
	{
		brickTransformMats[7] = translate(brickTransformMats[7], vec3(0.0, 0.0, -10000.0));

	}

	///////next row up/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////next row up/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////next row up/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////next row up/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (brick9 == true)
	{
		brickTransformMats[8] = mat4(1.0);
		brickTransformMats[8] = translate(brickTransformMats[8], vec3(0.0, 0.0, brick9Z));


		if (brick1 != true)
		{
			if (brick9Z > 0.0) brick9Z -= brick9Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[8] = translate(brickTransformMats[8], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick10 == true)
	{
		brickTransformMats[9] = mat4(1.0);
		brickTransformMats[9] = rotate(brickTransformMats[9], radians(45.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[9] = translate(brickTransformMats[9], vec3(0.0, 0.0, brick10Z));


		if (brick2 != true)
		{
			if (brick10Z > 0.0) brick10Z -= brick10Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[9] = translate(brickTransformMats[9], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick11 == true)
	{
		brickTransformMats[10] = mat4(1.0);
		brickTransformMats[10] = rotate(brickTransformMats[10], radians(90.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[10] = translate(brickTransformMats[10], vec3(0.0, 0.0, brick11Z));


		if (brick3 != true)
		{
			if (brick11Z > 0.0) brick11Z -= brick11Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[10] = translate(brickTransformMats[10], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick12 == true)
	{
		brickTransformMats[11] = mat4(1.0);
		brickTransformMats[11] = rotate(brickTransformMats[11], radians(135.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[11] = translate(brickTransformMats[11], vec3(0.0, 0.0, brick12Z));


		if (brick4 != true)
		{
			if (brick12Z > 0.0) brick12Z -= brick12Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[11] = translate(brickTransformMats[11], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick13 == true)
	{
		brickTransformMats[12] = mat4(1.0);
		brickTransformMats[12] = rotate(brickTransformMats[12], radians(180.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[12] = translate(brickTransformMats[12], vec3(0.0, 0.0, brick13Z));


		if (brick5 != true)
		{
			if (brick13Z > 0.0) brick13Z -= brick13Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[12] = translate(brickTransformMats[12], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick14 == true)
	{
		brickTransformMats[13] = mat4(1.0);
		brickTransformMats[13] = rotate(brickTransformMats[13], radians(225.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[13] = translate(brickTransformMats[13], vec3(0.0, 0.0, brick14Z));


		if (brick6 != true)
		{
			if (brick14Z > 0.0) brick14Z -= brick14Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[13] = translate(brickTransformMats[13], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick15 == true)
	{
		brickTransformMats[14] = mat4(1.0);
		brickTransformMats[14] = rotate(brickTransformMats[14], radians(270.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[14] = translate(brickTransformMats[14], vec3(0.0, 0.0, brick15Z));


		if (brick7 != true)
		{
			if (brick15Z > 0.0) brick15Z -= brick15Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[14] = translate(brickTransformMats[14], vec3(0.0, 0.0, -10000.0));

	}
	//----------------------------------------------------------------------------------------------------------------------//
	if (brick16 == true)
	{
		brickTransformMats[15] = mat4(1.0);
		brickTransformMats[15] = rotate(brickTransformMats[15], radians(315.0f), vec3(0.0, 0.0, 1.0));
		brickTransformMats[15] = translate(brickTransformMats[15], vec3(0.0, 0.0, brick16Z));


		if (brick8 != true)
		{
			if (brick16Z > 0.0) brick16Z -= brick16Z * BRICK_WEIGHT / 100;
		}
	}
	else
	{
		brickTransformMats[15] = translate(brickTransformMats[15], vec3(0.0, 0.0, -10000.0));

	}

}


// Initialization routine.
void setup(void) 
{
   glClearColor(1.0, 1.0, 1.0, 0.0); 
   glEnable(GL_DEPTH_TEST);

   

   // Create shader program executable.
   vertexShaderId = setShader("vertex", "vertexShader.glsl");
   fragmentShaderId = setShader("fragment", "fragmentShader.glsl");
   programId = glCreateProgram(); 
   glAttachShader(programId, vertexShaderId); 
   glAttachShader(programId, fragmentShaderId);    
   glLinkProgram(programId); 
   glUseProgram(programId); 

   // Obtain light property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "light0.ambCols"), 1, &light0.ambCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.difCols"), 1, &light0.difCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.specCols"), 1, &light0.specCols[0]);
   glUniform4fv(glGetUniformLocation(programId, "light0.coords"), 1, &light0.coords[0]);

   fillBatTransformMats();
   fillBrickTransformMats();
   fillDiscVertexArray(discVertices);

   //associate amb with vertex shader
   glUniform4fv(glGetUniformLocation(programId, "globAmb"), 1, &globAmb[0]);

   // Initialize hemishpere, bat and brick.
   fillHemisphere(hemVertices, hemIndices, hemCounts, hemOffsets);
   fillBat(batVertices, batIndices, batCounts, batOffsets);
   fillBrick(brickVertices, brickIndices, brickCounts, brickOffsets);



   // Create VAOs and VBOs... 
   glGenVertexArrays(4, vao);
   glGenBuffers(14, buffer); 

   // ...and associate data with vertex shader.
   glBindVertexArray(vao[HEMISPHERE]);  
   glBindBuffer(GL_ARRAY_BUFFER, buffer[HEM_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(hemVertices), hemVertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[HEM_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hemIndices), hemIndices, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(hemVertices[0]), 0);
   glEnableVertexAttribArray(0);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(hemVertices[0]), (void*)sizeof(hemVertices[0].coords));
   glEnableVertexAttribArray(1);

   glUniform4fv(glGetUniformLocation(programId, "ballMaterial.ambRefl"), 1, &ballMaterial.ambRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "ballMaterial.difRefl"), 1, &ballMaterial.difRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "ballMaterial.specRefl"), 1, &ballMaterial.specRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "ballMaterial.emitCols"), 1, &ballMaterial.emitCols[0]);
   glUniform1f(glGetUniformLocation(programId, "ballMaterial.shininess"), ballMaterial.shininess);


   // ...and associate data with vertex shader.
   glBindVertexArray(vao[BAT]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[BAT_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(batVertices), batVertices, GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[BAT_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(batIndices), batIndices, GL_STATIC_DRAW);
   glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(batVertices[0]), 0);
   glEnableVertexAttribArray(2);
   

   // ...and associate data with vertex shader.
   glBindBuffer(GL_ARRAY_BUFFER, buffer[BAT_TRANSFORM_MATS]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(batTransformMats), batTransformMats, GL_STATIC_DRAW);
   for (int i = 0; i < 4; i++)
   {
	   glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
	   glEnableVertexAttribArray(3 + i);
	   glVertexAttribDivisor(3 + i, 1); // Set attribute instancing.
   }

   normalMatLoc = glGetUniformLocation(programId, "normalMat");


   // Obtain bat material property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "batMaterial.ambRefl"), 1, &batMaterial.ambRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "batMaterial.difRefl"), 1, &batMaterial.difRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "batMaterial.specRefl"), 1, &batMaterial.specRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "batMaterial.emitCols"), 1, &batMaterial.emitCols[0]);
   glUniform1f(glGetUniformLocation(programId, "batMaterial.shininess"), batMaterial.shininess);

   glBindVertexArray(vao[BRICK]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[BRICK_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(brickVertices), brickVertices, GL_DYNAMIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[BRICK_INDICES]);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(brickIndices), brickIndices, GL_DYNAMIC_DRAW);
   glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(brickVertices[0]), 0);
   glEnableVertexAttribArray(7);

   //Associate brick colors with vertex shader
   glBindBuffer(GL_ARRAY_BUFFER, buffer[BRICK_COLOR]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(brickColors), brickColors, GL_STATIC_DRAW);
   glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(brickColors[0]), 0);
   glEnableVertexAttribArray(8);
   glVertexAttribDivisor(8, 1);			//Set attribute instancing

   // ...and associate data with vertex shader.
   glBindBuffer(GL_ARRAY_BUFFER, buffer[BRICK_TRANSFORM_MATS]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(brickTransformMats), brickTransformMats, GL_DYNAMIC_DRAW);
   for (int i = 0; i < 4; i++)
   {
	   glVertexAttribPointer(9 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
	   glEnableVertexAttribArray(9 + i);
	   glVertexAttribDivisor(9 + i, 1); // Set attribute instancing.
   }

   // Obtain brick material property uniform locations and set values.
   glUniform4fv(glGetUniformLocation(programId, "brickMaterial.ambRefl"), 1, &brickMaterial.ambRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "brickMaterial.difRefl"), 1, &brickMaterial.difRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "brickMaterial.specRefl"), 1, &brickMaterial.specRefl[0]);
   glUniform4fv(glGetUniformLocation(programId, "brickMaterial.emitCols"), 1, &brickMaterial.emitCols[0]);
   glUniform1f(glGetUniformLocation(programId, "brickMaterial.shininess"), brickMaterial.shininess);

   glBindVertexArray(vao[DISC]);
   glBindBuffer(GL_ARRAY_BUFFER, buffer[DISC_VERTICES]);
   glBufferData(GL_ARRAY_BUFFER, sizeof(discVertices), discVertices, GL_STATIC_DRAW);
   glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(discVertices[0]), 0);
   glEnableVertexAttribArray(13);
   glVertexAttribPointer(14, 2, GL_FLOAT, GL_FALSE, sizeof(discVertices[0]),
	   (void*)(sizeof(discVertices[0].coords) + sizeof(discVertices[0].normal)));
   glEnableVertexAttribArray(14);

   image[0] = getbmp("Textures/canTop.bmp");

   glGenTextures(1, texture);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture[0]);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
	   GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   canTopTexLoc = glGetUniformLocation(programId, "canTopTex");
   glUniform1i(canTopTexLoc, 0);

   // Obtain projection matrix uniform location and set value.
   projMatLoc = glGetUniformLocation(programId,"projMat"); 
   projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); 
   projMat = translate(projMat, vec3(0, 20, -25));
   projMat = rotate(projMat, radians(45.0f), vec3(-1.0, 0.0, 0.0));
   projMat = translate(projMat, vec3(0, 0, 5));
   glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

   // Obtain color uniform locations and set values.
   hemColorLoc = glGetUniformLocation(programId, "hemColor");
   glUniform4fv(hemColorLoc, 1, &hemColors[0]);
   batColorLoc = glGetUniformLocation(programId, "batColor");
   glUniform4fv(batColorLoc, 1, &batColors[0]);
   //brickColorLoc = glGetUniformLocation(programId, "brickColor");
   //glUniform4fv(brickColorLoc, 1, &brickColors[0]);

   // Obtain modelview matrix uniform and object uniform locations.
   modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat"); 
   objectLoc = glGetUniformLocation(programId, "object");


   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Drawing routine.
void drawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (glm::length(ballLoc) > 18 && glm::length(ballLoc) < 25)
   {
	   ballBatHitCheck();
   }

   if (glm::length(ballLoc) > 5 && glm::length(ballLoc) < 13)
   {
	   ballBatHitCheck();
   }

   applyFriction();
   applyGrav();

   // Calculate and update modelview matrix.
   modelViewMat = mat4(1.0);
   modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, -25.00));
   modelViewMat = rotate(modelViewMat, radians(Zangle), vec3(0.0, 0.0, 1.0));
   modelViewMat = rotate(modelViewMat, radians(Yangle), vec3(0.0, 1.0, 0.0));
   modelViewMat = rotate(modelViewMat, radians(Xangle), vec3(1.0, 0.0, 0.0));
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); 



   mat4 baseModelViewMat = modelViewMat; //Push
   modelViewMat = rotate(modelViewMat, radians(batRot), vec3(0.0, 0.0, 1.0));

   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));//pop


   // Calculate and update normal matrix
   normalMat = transpose(inverse(mat3(modelViewMat)));
   glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));
   
   

   // Draw bat.
   glUniform1ui(objectLoc, BAT); // Update object name.
   glBindVertexArray(vao[BAT]);

   // MultiDraw same as for loop below but cannot be used with instancing
   // glMultiDrawElements(GL_TRIANGLE_STRIP, batCounts, GL_UNSIGNED_INT, (const void **)batOffsets, BAT_FACES)'

   for (int i = 0; i < 6; i++)
   {
	   glDrawElementsInstanced(GL_TRIANGLE_STRIP, batCounts[i], GL_UNSIGNED_INT, batOffsets[i], 3);
   }
   
   modelViewMat = baseModelViewMat; //pop???

   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   // Draw brick.
   glUniform1ui(objectLoc, BRICK); // Update object name.
   glBindVertexArray(vao[BRICK]);
   
   for (int i = 0; i < 6; i++)
   {
	   glDrawElementsInstanced(GL_TRIANGLE_STRIP, brickCounts[i], GL_UNSIGNED_INT, brickOffsets[i], BRICK_NO);
   }

   modelViewMat = baseModelViewMat;
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   //Ball

   modelViewMat = translate(modelViewMat, ballLoc); 
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   // Draw ball as two hemispheres.
   glUniform1ui(objectLoc, HEMISPHERE); // Update object name.
   glBindVertexArray(vao[HEMISPHERE]);

  
   
   // Calculate and update normal matrix
   normalMat = transpose(inverse(mat3(modelViewMat)));
   glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));
   glMultiDrawElements(GL_TRIANGLE_STRIP, hemCounts, GL_UNSIGNED_INT, (const void **)hemOffsets, HEM_LATS);
   modelViewMat = scale(modelViewMat, vec3(1.0, -1.0, 1.0)); // Scale to make inverted hemisphere.
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat)); // Update modelview matrix.



   // Calculate and update normal matrix
   normalMat = transpose(inverse(mat3(modelViewMat)));
   glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, value_ptr(normalMat));

   glMultiDrawElements(GL_TRIANGLE_STRIP, hemCounts, GL_UNSIGNED_INT, (const void **)hemOffsets, HEM_LATS);

   modelViewMat = baseModelViewMat;
   modelViewMat = translate(modelViewMat, vec3(0, 0, -0.001));
   glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

   glUniform1ui(objectLoc, DISC);
   glBindVertexArray(vao[DISC]);
   glDrawArrays(GL_TRIANGLE_FAN, 0, DISC_SEGS);


   glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
   glViewport(0, 0, w, h); 
}

// Timer function.
void animate(int value)
{
   if (isAnimate) 
   {
	   ballLoc += ballDir;

	   glutPostRedisplay();
	   glutTimerFunc(animationPeriod, animate, 1);

	   

	   glBindBuffer(GL_ARRAY_BUFFER, buffer[BRICK_TRANSFORM_MATS]);
	   glBufferData(GL_ARRAY_BUFFER, sizeof(brickTransformMats), brickTransformMats, GL_DYNAMIC_DRAW);
	   for (int i = 0; i < 4; i++)
	   {
		   glVertexAttribPointer(9 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
		   glEnableVertexAttribArray(9 + i);
		   glVertexAttribDivisor(9 + i, 1); // Set attribute instancing.
	   }
	   
	   fillBrickTransformMats();
   }
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key) 
   {
      case 27:
         exit(0);
         break;
	  case ' ': 
         if (isAnimate) isAnimate = 0;
		 else 
		 {
	        isAnimate = 1; 
			animate(1);
		 }
		 break;
      case 'x':
         Xangle += 5.0;
		 if (Xangle > 360.0) Xangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'X':
         Xangle -= 5.0;
		 if (Xangle < 0.0) Xangle += 360.0;
         glutPostRedisplay();
         break;
      case 'y':
         Yangle += 5.0;
		 if (Yangle > 360.0) Yangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'Y':
         Yangle -= 5.0;
		 if (Yangle < 0.0) Yangle += 360.0;
         glutPostRedisplay();
         break;
      case 'z':
         Zangle += 5.0;
		 if (Zangle > 360.0) Zangle -= 360.0;
         glutPostRedisplay();
         break;
      case 'Z':
         Zangle -= 5.0;
		 if (Zangle < 0.0) Zangle += 360.0;
         glutPostRedisplay();
         break;
	  case 'd':
		  batRot += 5;
		  if (batRot > 360.0) batRot -= 360;
		  rotateBounds(5.0);
		  break;
	  case 'a':
		  batRot -= 5;
		  if (batRot < 0.0) batRot += 360;
		  rotateBounds(-5.0);
		  break;
	  case 'k':
		  ballLoc -= vec3(1,0,0);
		  glutPostRedisplay();
		  break;
	  case 'l':
		  ballLoc += vec3(1, 0, 0);
		  glutPostRedisplay();
		  break;
	  case 'i':
		  ballLoc += vec3(0,1,0);
		  glutPostRedisplay();
		  break;
	  case 'm':
		  ballLoc -= vec3(0, 1, 0);
		  glutPostRedisplay();
		  break;
	  case 'b':
		  if (score >= 5)
		  {
			  ballDir = ballDir * 5.0f;
			  score -= 5;
		  }
      default:
         break;
   }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
   if (key == GLUT_KEY_DOWN) animationPeriod += 5;
   if( key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
   glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
   cout << "Interaction:" << endl;
   cout << "Press space to begin the game, press again to pause" << endl
	   << "Press the a and d keys to rotate the bats" << endl
	   << "Press the x, X, y, Y, z, Z keys to rotate the scene" << endl
	   << "You have 3 lives, when you run out your score and your lives will be reset" << endl
	   << "Press b to boost the ball at the cost of 5 points" << endl
	   << "Previous score can be found in scoreFile.txt"<< endl;
}

// Main routine.
int main(int argc, char **argv) 
{
   printInteraction();
   glutInit(&argc, argv);

   glutInitContextVersion(4, 2);
   glutInitContextProfile(GLUT_CORE_PROFILE);
   glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); 
   glutInitWindowSize(500, 500);
   glutInitWindowPosition(100, 100); 
   glutCreateWindow("ballAndBatShaderized.cpp");
   glutDisplayFunc(drawScene); 
   glutReshapeFunc(resize);  
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);

   glewExperimental = GL_TRUE;
   glewInit();

   setup(); 
   
   


   glutMainLoop(); 
}

