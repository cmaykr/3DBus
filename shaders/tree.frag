#version 150

out vec4 out_Color;;
in vec2 outTexCoord;
in vec3 position;
in vec3 normal2;

uniform sampler2D texUnit;

uniform vec3 light1Pos;
uniform vec3 light1Dir;
uniform vec3 light2Pos;
uniform vec3 light2Dir;
uniform float outerCosineLimit;
uniform float innerCosineLimit;
uniform float maxLightDist;
float a = 5;
float b = 0.1;
float c = 0.04;
uniform bool shouldSpotlight = true;
vec4 gShade;

uniform mat4 mdlMatrix;
uniform float shadeOn;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
	vec3 transformedNormal = normalMatrix1 * normal2;
	vec3 light = vec3(0.58, 0.58, 0.58);
	float shade;
	shade = dot(transformedNormal, light);
	shade = max(shade, 0);

	vec4 vertcol;
	if (shadeOn == 1)
		vertcol = vec4(shade + vec4(0.2, 0.12, 0.10, 0.1));
	else
		vertcol = vec4(0.7, 0.55, 0.5, 1.0);
	gShade = vertcol;
	vec4 ambient = texture(texUnit, outTexCoord) * vertcol;


	vec3 vecToPoint = light1Pos - position;
	float distance = length(vecToPoint);
	if (distance < maxLightDist && shouldSpotlight)
	{
		float theta = dot(light1Dir, normalize(-vecToPoint));
		float epsilon = (cos(radians(innerCosineLimit)) - cos(radians(outerCosineLimit)));
		float intens1 = clamp((theta - cos(radians(outerCosineLimit))) / epsilon, 0.0, 1.0);

		vecToPoint = light2Pos - position;
		distance = length(vecToPoint);

		theta = dot(light2Dir, normalize(-vecToPoint));
		float intens2 = clamp((theta - cos(radians(outerCosineLimit))) / epsilon, 0.0, 1.0);

		out_Color = texture(texUnit, outTexCoord) * (intens1 + intens2) + ambient;
	}
	else
		out_Color = ambient;

}