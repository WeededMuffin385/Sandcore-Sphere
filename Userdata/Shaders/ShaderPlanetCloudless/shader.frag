#version 460 core
out vec4 fragColor;

in vec3 fPos;
layout(binding = 0) uniform samplerCube texture0;

void main()
{
    vec4 color = texture(texture0, fPos);
    fragColor = vec4(color.rgb * 0.95, color.a);
} 