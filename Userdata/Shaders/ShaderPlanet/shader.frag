#version 460 core
out vec4 fragColor;

in vec3 fPos;
layout(binding = 0) uniform samplerCube texture0;

void main()
{
    fragColor = texture(texture0, fPos);
} 