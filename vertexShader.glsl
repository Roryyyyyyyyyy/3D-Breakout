#version 420 core

#define HEMISPHERE 0
#define BAT 1
#define BRICK 2
#define DISC 3

layout(location=0) in vec4 hemCoords;
layout(location=1) in vec3 hemNormal;
layout(location=2) in vec4 batCoords;
layout(location = 3) in mat4 batTransformMats; // 3,4,5,6
layout(location = 7) in vec4 brickCoords;
layout(location = 8) in vec4 brickColors;
layout(location = 9) in mat4 brickTransformMats; // 9,10,11,12
layout(location = 13) in vec4 discCoords;
layout(location = 14) in vec2 discTexCoords;


uniform mat4 projMat;
uniform mat4 modelViewMat;
uniform uint object;
uniform vec4 globAmb;

vec3 normal;
uniform mat3 normalMat;

vec3 lightDirection;

vec3 eyeDirection, halfway;

vec4 batAmb, batDiff, batSpec, brickAmb, brickDiff, brickSpec, ballAmb, ballDiff, ballSpec;

vec4 coords;

out vec4 batColsExport;
out vec4 brickColsExport;
out vec4 ballColsExport;
out vec4 colorsExport;
out vec2 texCoordsExport;


struct Material
{
 vec4 ambRefl;
 vec4 difRefl;
 vec4 specRefl;
 vec4 emitCols;
 float shininess;
};
uniform Material batMaterial;
uniform Material brickMaterial;
uniform Material ballMaterial;

struct Light
{
 vec4 ambCols;
 vec4 difCols;
 vec4 specCols;
 vec4 coords;
};
uniform Light light0;


void main(void)
{
	lightDirection = normalize(vec3(light0.coords));
	if (object == HEMISPHERE)
	{
		coords = hemCoords; 
		eyeDirection = -1.0 * normalize(vec3(modelViewMat * coords));
		halfway = (length(lightDirection + eyeDirection) == 0.0) ? vec3(0.0) :
			(lightDirection + eyeDirection) / length(lightDirection + eyeDirection);


		normal = hemNormal;
		normal = normalize(normalMat * normal);

		ballAmb = globAmb * ballMaterial.ambRefl;

		ballDiff = max(dot(normal, lightDirection), 0.0) * light0.difCols * ballMaterial.difRefl;

		ballSpec = pow(max(dot(normal, halfway), 0.0), ballMaterial.shininess) * light0.specCols * ballMaterial.specRefl;
		ballColsExport = vec4(vec3(min(ballAmb + ballDiff, vec4(1.0))), 1.0);


		gl_Position = projMat * modelViewMat * coords;
	}
   if (object == BAT)
   {
	   coords = batCoords;

	   batColsExport = vec4(0.0, 0.0, 1, 1.0);

	   gl_Position = projMat * modelViewMat * batTransformMats * coords;
   }
   if (object == BRICK)
   {
	   coords = brickCoords;


	   colorsExport = brickColors;

	   gl_Position = projMat * modelViewMat * brickTransformMats * coords;
   }
   if (object == DISC)
   {
	   coords = discCoords;
	   //normal = discNormal;
	   texCoordsExport = discTexCoords;
	   gl_Position = projMat * modelViewMat * coords;
   }

}