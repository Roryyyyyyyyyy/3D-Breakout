#version 420 core

#define HEMISPHERE 0
#define BAT 1
#define BRICK 2
#define DISC 3


uniform uint object;
uniform vec4 hemColor, batColor, brickColor;
in vec2 texCoordsExport;

in vec4 batColsExport;
in vec4 brickColsExport;
in vec4 ballColsExport;
in vec4 colorsExport;

uniform sampler2D canTopTex;

vec4 texColor;
out vec4 colorsOut;

void main(void)
{
   if (object == HEMISPHERE) colorsOut = ballColsExport; 
   if (object == BAT) colorsOut = batColsExport;
   if (object == BRICK) colorsOut = colorsExport;
   if (object == DISC) colorsOut = texture(canTopTex, texCoordsExport);

}