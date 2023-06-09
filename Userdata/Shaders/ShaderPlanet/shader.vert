#version 460 core
layout (location = 0) in vec3 vPos;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform mat4 shadowModel;

out vec3 fPos;
out vec3 sPos;

void main()
{
    sPos = (shadowModel * vec4(vPos, 1.0)).xyz;
	fPos = vPos;
    gl_Position = proj * view * model * vec4(vPos, 1.0);
}