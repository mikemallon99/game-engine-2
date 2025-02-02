#version 330 core
out vec4 FragColor;

uniform vec3 wireframeColor;

void main()
{
    FragColor = vec4(wireframeColor, 1.0);
}  