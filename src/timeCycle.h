#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

float timeElapsed = 0.0f;
glm::vec3 ambientLighting;

void dayCycle(float deltaTime, Shader lightingShader) {
    timeElapsed += deltaTime;

    // Day cycle configuration
    float dayLength = 60.0f; // Length of a full day in seconds
    float timeScale = 1.0f; // Speed multiplier for day/night cycle
    float currentTime = fmod(glfwGetTime() * timeScale, dayLength); // Loop time within the day

    // Calculate angle of the sun across the sky
    float angle = currentTime / dayLength * 2.0f * 3.14159f; // Angle in radians (0 to 2π)

    // Direction of light based on angle
    float dirX = cos(angle);
    float dirY = sin(angle) * 0.5f; // Scaled for slight elevation change
    float dirZ = sin(angle);

    glm::vec3 lightDirection = glm::normalize(glm::vec3(dirX, dirY, dirZ));

    // Calculate ambient lighting
    float rawTimeOfDay = sin(currentTime / dayLength * 2.0f * 3.14159f);
    float timeOfDay = (rawTimeOfDay + 1.0f) / 2.0f; // Normalize to [0, 1]

    float nightBaseIntensity = 0.1f; // Minimum light at night
    float dayIntensityScale = 0.75f;  // Maximum additional intensity during the day
    float lightIntensity = nightBaseIntensity + timeOfDay * dayIntensityScale;

    glm::vec3 ambientLighting = glm::vec3(lightIntensity, lightIntensity, lightIntensity);

    // Set lighting parameters in the shader
    printf("Angle: %f, Light Direction: (%f, %f, %f)\n", angle, lightDirection.x, lightDirection.y, lightDirection.z);
    lightingShader.setVec3("dirLight.direction", lightDirection);
    lightingShader.setVec3("dirLight.ambient", ambientLighting);
    lightingShader.setVec3("dirLight.diffuse", glm::vec3(lightIntensity * 0.5f));
    lightingShader.setVec3("dirLight.specular", glm::vec3(lightIntensity * 0.2f));
}
