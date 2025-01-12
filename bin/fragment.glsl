#version 330 core
out vec4 FragColor;
in vec3 ourColor;

uniform float time;

void main()
{
    float red = sin(time * 2.0f) * 0.5f + 0.5f;   // Red shifts from 0 to 1
    float green = sin(time * 2.0f + 2.0f) * 0.5f + 0.5f; // Green shifts from 0 to 1
    float blue = sin(time * 2.0f + 4.0f) * 0.5f + 0.5f; // Blue shifts from 0 to 1
    FragColor = vec4(red, green, blue, 1.0f);
} 