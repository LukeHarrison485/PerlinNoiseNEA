
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glfw/glfw3.h>
#include <vector>
#include "shader.h"

enum TextureID
{
    DIRT,
    GRASS,
    STONE
};

float vertices[] = {
    // positions          // normals             // texture coords
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // Top-right
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f, // Top-right
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f, // Top-left
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f, // Bottom-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // Top-right
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f, // Top-right
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f, // Bottom-left
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f, // Top-left
    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Top-right
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Bottom-left
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // Top-left
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Bottom-left
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Top-right
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // Bottom-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Top-left
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f, // Top-right
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Bottom-right
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f, // Bottom-right
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f, // Top-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // Top-right
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // Bottom-left
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f, // Top-left
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f, // Bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f, // Top-right
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f, // Bottom-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f, // Top-left
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f, // Top-right
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f, // Bottom-right
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f, // Bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f  // Top-left
};

unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0,  // front
    4, 5, 6, 6, 7, 4,  // back
    8, 9, 10, 10, 11, 8,  // left
    12, 13, 14, 14, 15, 12,  // right
    16, 17, 18, 18, 19, 16,  // top
    20, 21, 22, 22, 23, 20   // bottom
};

unsigned int textures[3];
std::vector<glm::vec3> instancePositions;

class cube {
public:
    glm::vec3 position;
    TextureID textureID;

    unsigned int* VAO;
    unsigned int* VBO;
    Shader* cubeShader;
    unsigned int* instanceVBO;

    cube(glm::vec3 pos, TextureID texID, Shader* shader, unsigned int* vao, unsigned int* vbo, std::vector<cube*>cubes) {
        position = pos;
        textureID = texID;

        VAO = vao;
        VBO = vbo;

        instancePositions.push_back(pos);
        cubes.push_back(this);
    }

    void updateInstanceData() {
        glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_DYNAMIC_DRAW);
    }

    glm::vec3 getMinBounds() {
        return position - glm::vec3(0.5f, 0.5f, 0.5f);
    }

    glm::vec3 getMaxBounds() {
        return position + glm::vec3(0.5f, 0.5f, 0.5f);
    }
};
std::vector<cube*> cubes;

void drawWorld(unsigned int VAO, Shader* shader) {
    if (!shader) {
        std::cerr << "Error: Shader not set for Cube.\n";
        return;
    }

    shader->use();
    glBindVertexArray(VAO);

    // Bind a default texture (or modify shader to handle multiple textures per instance)
    glBindTexture(GL_TEXTURE_2D, textures[DIRT]); // Example texture

    // Draw all instances
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instancePositions.size());

    glBindVertexArray(0);
}


