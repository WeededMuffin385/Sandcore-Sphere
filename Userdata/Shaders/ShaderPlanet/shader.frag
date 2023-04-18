#version 460 core
out vec4 fragColor;

in vec3 fPos;
in vec3 sPos;
layout(binding = 0) uniform samplerCube texture0;
layout(binding = 1) uniform samplerCube texture1;


void main()
{
    vec4 color = texture(texture0, fPos);
    float light = 1.0f - texture(texture1, sPos).a * 0.5;
    fragColor = vec4(color.rgb * light * 0.95, color.a);
} 