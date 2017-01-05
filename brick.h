#ifndef BRICK_H
#define BRICK_H

#include "vertex.h"

#define PI 3.14159265
#define BRICK_OUTRAD 10.0 // Brick outer radius.
#define BRICK_INRAD 8.0 // Brick inner radius.
#define BRICK_ANGLE 14.8 // Angle between each segment of brick.

#define BRICK_COLORS 0.0, 1.0, 0.0, 1.0 // Brick colors.
#define BRICK_COLORS_2 1.0, 1.0, 0.0, 1.0 // Brick colors.

// TODO: Need to update BRICK_FACES to 6 when other faces complete
#define BRICK_FACES 6 
#define BRICK_VERTEX_COUNT 16
#define BRICK_VERTICES_PER_FACE 8

void fillTorVertexArray(Vertex brickVertices[BRICK_VERTEX_COUNT]);
void fillTorIndices(unsigned int brickIndices[BRICK_FACES][BRICK_VERTICES_PER_FACE]);
void fillTorCounts(int brickCounts[BRICK_FACES]);
void fillTorOffsets(void* brickOffsets[BRICK_FACES]);

void fillBrick(Vertex brickVertices[BRICK_VERTEX_COUNT],
	unsigned int brickIndices[BRICK_FACES][BRICK_VERTICES_PER_FACE],
	int brickCounts[BRICK_FACES],
	void* brickOffsets[BRICK_FACES]);
#endif
