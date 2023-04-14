#version 460 core
out vec4 fragColor;

in vec3 fPos;
layout(binding = 0) uniform samplerCube texture0;

void main()
{
    if (texture(texture0, fPos) == vec4(0,0,0,0)) 
        fragColor = vec4(1,1,0,1); 
    else 
        fragColor = texture(texture0, fPos);
} 