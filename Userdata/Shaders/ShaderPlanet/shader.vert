#version 460 core
layout (location = 0) in vec3 vPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

layout(binding = 1) uniform samplerCube height;

out vec3 fPos;

void main()
{
	float h = texture(height, vPos).g - 0.5;
	if (h > 0) h = 1 + h * 0.07; else h = 1;

	gl_Position = proj * view * model * (vec4(vPos * h, 1.0));
	fPos = vPos;
}