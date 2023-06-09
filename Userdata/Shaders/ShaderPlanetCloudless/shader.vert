#version 460 core
layout (location = 0) in vec3 vPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec3 fPos;

void main()
{
	fPos = vPos;
    gl_Position = proj * view * model * vec4(vPos, 1.0);
}