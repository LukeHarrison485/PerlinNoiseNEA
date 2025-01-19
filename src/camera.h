#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cube.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  4.3f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  75.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float maxFov;
    float minFov;
    float size = 0.6f;
    glm::vec3 velocity;
    bool inAir = false;
    bool isJumping = false;

    bool running = 0;

    Camera(glm::vec3 position = glm::vec3(0.0f, 10.5f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), minFov(1.0f), maxFov(90.0f) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), minFov(1.0f), maxFov(90.0f)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    glm::vec3 getMinBounds() {
        return Position - glm::vec3(size, size + 0.7f, size);
    }

    glm::vec3 getMaxBounds() {
        return Position + glm::vec3(size, size + 0.7f, size);
    }

    void Jump() {
    if (!isJumping) {
        velocity.y = 6.0f; // Set a strong upward velocity for the jump
        isJumping = true;
        inAir = true;
        }
    }

    cube* checkCameraCollision(std::vector<cube*> listOfCubes) {
    for (cube* c : listOfCubes) {
        glm::vec3 minA = getMinBounds();
        glm::vec3 maxA = getMaxBounds();
        glm::vec3 minB = c->getMinBounds();
        glm::vec3 maxB = c->getMaxBounds();

        if (!(maxA.x < minB.x || minA.x > maxB.x || 
              maxA.y < minB.y || minA.y > maxB.y || 
              maxA.z < minB.z || minA.z > maxB.z)) {
            // Collision detected
            inAir = false;
            return c;
        }
        inAir = true;

    }
    return nullptr;  // No collision
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void updateCameraPosition(float deltaTime) {
    // Apply gravity
    velocity.y += -15.8f * deltaTime;
    Position += velocity * deltaTime;

    // Check for collisions with cubes
    cube* collidedCube = checkCameraCollision(cubes);
    if (collidedCube) {
        glm::vec3 cubeMaxBounds = collidedCube->getMaxBounds();

        // Place the camera on top of the cube and reset jump state
        if (Position.y < cubeMaxBounds.y + size + 0.7f) {
            Position.y = cubeMaxBounds.y + size + 0.7f;
            velocity.y = 0.0f; // Stop downward motion
            isJumping = false; // Allow jumping again
        }
    } else if (Position.y <= 0.0f) {
        // Prevent the camera from falling below ground level
        Position.y = 0.0f;
        velocity.y = 0.0f;
        isJumping = false; // Allow jumping again
    }
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;

        // Create a modified Front vector with the y component set to 0
        glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));

        if(direction == FORWARD) {
            Position += flatFront * velocity;
        }
        if(direction == BACKWARD) {
            Position -= flatFront * velocity;
        }
        if(direction == LEFT) {
            Position -= Right * velocity;
        }
        if(direction == RIGHT) {
            Position += Right * velocity;
        }
        

        // Optional: Keep Position.y at a fixed value if needed
        
    }


    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean contrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if(contrainPitch) {
            if (Pitch > 89.0f) {Pitch = 89.0f;};
            if (Pitch < -89.0f) {Pitch = -89.0f;};
        }
        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < minFov)
            Zoom = minFov;
        if (Zoom > maxFov)
            Zoom = maxFov;
    }
private:
    void updateCameraVectors() {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

#endif