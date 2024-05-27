#version 150

in  vec3 in_Position;
in  vec3 normal;
in  vec2 inTexCoord;
out vec3 position;
out vec3 normal2;
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform sampler2D texUnit;
uniform sampler2D texUnit2;
uniform mat4 camMatrix;
uniform float shadeOn;

in vec4 in_vertcol;
out vec4 vertcol;
out vec2 outTexCoord;;
uniform mat4 colorMatrix;

uniform vec3 lightSourceDirection;
uniform vec3 lightSourceColor;

uniform mat4 camera;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
	outTexCoord = inTexCoord;

	normal2 = normal;
	gl_Position = projMatrix * camera * mdlMatrix * vec4(in_Position, 1.0);


	position = vec3(mdlMatrix * vec4(in_Position, 1.0));
}