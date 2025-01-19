#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <iostream>
#include "shader.h"
#include "camera.h"
#include "perlin.h"
#include "timeCycle.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void createPointLight(int index, Shader& lightingShader);
void renderScene(GLFWwindow* window, Shader shader);
void configureMatricesAndShaders();
void initializeBuffers(unsigned int* VAO, unsigned int* instanceVBO, unsigned int* VBO);
void updateInstanceData();

Shader lightingShader;
Shader simpleDepthShader;

unsigned int VBO, cubeVAO, instanceVBO;

int seed;

void generateWorldFromHeightmap(const char* heightmapPath, std::vector<cube*>& cubes, int MAX_HEIGHT, unsigned int* vao) {
    createPerlinNoise(32, 256, 256, seed);
    int width, height, nrChannels;
    unsigned char* heightmapData = stbi_load(heightmapPath, &width, &height, &nrChannels, 1); // Load as grayscale
    if (!heightmapData) {
        std::cout << "Failed to load heightmap" << std::endl;
        return;
    }
    std::vector<cube*> temp;
    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            unsigned char pixelValue = heightmapData[z * width + x];
            float normalizedHeight = pixelValue / 255.0f; // Normalize [0, 1]
            int cubeHeight = static_cast<int>(normalizedHeight * MAX_HEIGHT);
            
            // Create cubes stacked vertically
            for (int y = 0; y < cubeHeight; ++y) {
                if(y < 4) {
                    temp.push_back(new cube(glm::vec3(x, y, z), STONE, &lightingShader, &cubeVAO, &VBO, cubes));
                }
                else {
                    temp.push_back(new cube(glm::vec3(x, y, z), DIRT, &lightingShader, &cubeVAO, &VBO, cubes));
                }
                
            }
        }
    }
    cubes = temp;
    updateInstanceData();

    stbi_image_free(heightmapData);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 6.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool bilin = true;
bool gamma = false;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


glm::vec3 pointLightPositions[100];
int pointLightCount = 0;

int width = 20;
int height = 2;
int depth = 20;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    lightingShader.createShader("lighting.vs", "lighting.fs");
    simpleDepthShader.createShader("shaders/depth.vs", "shaders/depth.fs");

    const char* version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;

    glEnable(GL_MULTISAMPLE);  

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    
    initializeBuffers(&cubeVAO, &instanceVBO, &VBO);

    unsigned int floorTexture = loadTexture("wood.png");
    unsigned int floorTextureGammaCorrected = loadTexture("wood.png");

    lightingShader.use(); 
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setFloat("light.constant",  1.0f);
    lightingShader.setFloat("light.linear",    0.09f);
    lightingShader.setFloat("light.quadratic", 0.032f);	
    seed = time(NULL);
    glGenTextures(3, textures); // Generate texture IDs for 3 textures
    textures[DIRT] = loadTexture("dirt.png");
    textures[STONE] = loadTexture("stone.png");
    textures[GRASS] = loadTexture("grass.png");
    generateWorldFromHeightmap("perlin.bmp", cubes, 18, &cubeVAO);
    printf("Amount of blocks in the world: %d\n", cubes.size());
    


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);       // Enable depth testing
    glDepthFunc(GL_LESS);
    
    glEnable(GL_CULL_FACE);        // Enable face culling
    glCullFace(GL_BACK);           // Cull back faces
    glFrontFace(GL_CW);           // Counter-clockwise vertices are front faces
    

    


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(ambientLighting.x, ambientLighting.y, ambientLighting.z + 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        /*
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            renderScene(window, simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // 2. then render scene as normal with shadow mapping (using depth map)
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        */
        camera.updateCameraPosition(deltaTime);
        configureMatricesAndShaders();
        renderScene(window, lightingShader);

        glfwSwapBuffers(window);
        glfwPollEvents();   
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.Jump();
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        seed = time(NULL);
        generateWorldFromHeightmap("perlin.bmp", cubes, 32, &cubeVAO);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        if(!camera.running) {
            camera.MovementSpeed = 5.0f;
            camera.running = true;
            camera.Zoom *= 1.2f;
        }
        else {
            camera.MovementSpeed = 2.5f;
            camera.running = false;
            camera.Zoom /= 1.2f;
        }
    }
    if (key == GLFW_KEY_B && action == GLFW_RELEASE) {
            if(bilin) {
                bilin = false;
                printf("Bilin off!\n");
            } 
            else {
                bilin = true;
            }
        }
    
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // Set texture filtering parameters to avoid blurriness
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Nearest for mipmaps
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Nearest for magnification

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        if (pointLightCount < 100) { // Ensure we don't exceed the array bounds
            // Add a new light position based on the camera position
            pointLightPositions[pointLightCount] = glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z);
            pointLightCount++; // Increment the count
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            glm::vec3 rayOrigin, rayDirection;
            //glm::unProject(glm::vec3(mouseX, mouseY, 0.0f),);
        }
    }
}

void createPointLight(int index, Shader& lightingShader) {
    // Use the index to correctly reference the specific point light in the shader
    std::string baseName = "pointLights[" + std::to_string(index) + "]";
    
    lightingShader.setVec3(baseName + ".position", pointLightPositions[index]); // Keep lamp position
    lightingShader.setVec3(baseName + ".ambient", 0.3f, 0.24f, 0.14f); // Warm ambient light (low intensity)
    lightingShader.setVec3(baseName + ".diffuse", 0.7f, 0.5f, 0.3f); // Warm diffuse light (higher than ambient)
    lightingShader.setVec3(baseName + ".specular", 1.0f, 0.9f, 0.8f); // Slightly warm specular highlights
    lightingShader.setFloat(baseName + ".constant", 1.0f); // Standard constant term
    lightingShader.setFloat(baseName + ".linear", 0.18f); // Higher linear attenuation for small range
    lightingShader.setFloat(baseName + ".quadratic", 0.12f); // Higher quadratic attenuation for rapid falloff
    lightingShader.setBool("bilin", bilin); // Keep Blinn-Phong if applicable
    lightingShader.setBool("gamma", gamma); // Apply gamma correction if enabled

    
}

void configureMatricesAndShaders() {

    lightingShader.use();
    lightingShader.setVec3("viewPos", camera.Position);
    lightingShader.setFloat("material.shininess", 2.0f);
    dayCycle(deltaTime, lightingShader);
    for(int i = 0; i < pointLightCount; i++) {
        createPointLight(i, lightingShader);
    }
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    lightingShader.setInt("pointLightCount", pointLightCount);
}

void renderScene(GLFWwindow* window, Shader shader) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    drawWorld(cubeVAO, &shader);
};

void initializeBuffers(unsigned int* VAO, unsigned int* instanceVBO, unsigned int* VBO) {
        glGenVertexArrays(1, VAO);
        glGenBuffers(1, VBO);
        glGenBuffers(1, instanceVBO);

        glBindVertexArray(*VAO);

        // Set up vertex data (positions, normals, texture coords)
        glBindBuffer(GL_ARRAY_BUFFER, *VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0); // Position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); // Normal
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2); // Texture coords

        // Set up instance data
        glBindBuffer(GL_ARRAY_BUFFER, *instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(3); // Instance position
        glVertexAttribDivisor(3, 1);  // Set attribute divisor for instancing

        glBindVertexArray(0);
}

void updateInstanceData() {
    // Update the instance VBO with new positions
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_DYNAMIC_DRAW);
}