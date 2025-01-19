#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aInstancePos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Add the instance position to the vertex position before transforming
    vec3 worldPosition = vec3(model * vec4(aPos, 1.0)) + aInstancePos;
    
    FragPos = worldPosition;
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;

    // Apply the view and projection transformations
    gl_Position = projection * view * vec4(worldPosition, 1.0);
}
