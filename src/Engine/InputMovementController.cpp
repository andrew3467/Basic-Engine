//
// Created by apgra on 1/14/2024.
//

#include <iostream>
#include "InputMovementController.h"

void BE::InputMovementController::moveInPlaneXZ(GLFWwindow *window, float dt, BE::Camera &camera) {
    if (glfwGetKey(window, keys.closeGame) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    auto& gameObject = camera.cameraEntity;

    glm::vec3 moveDir{0.f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += camera.front;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= camera.front;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += camera.right;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= camera.right;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += camera.up;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= camera.up;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject->transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
}

void BE::InputMovementController::rotateCameraXZ(GLFWwindow *window, float dt, BE::Camera &camera) {
    auto& gameObject = camera.cameraEntity;
    glm::vec3 rotate{0};

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if(camera.firstMouse){
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = false;
    }

    float xoffset = xpos - camera.lastX;
    float yoffset = camera.lastY - ypos;
    camera.lastX = xpos;
    camera.lastY = ypos;

    float yaw = xoffset;
    float pitch = yoffset;

    if(pitch > 89.0f){
        pitch = 89.0f;
    }
    if(pitch < -89.0f){
        pitch = -89.0f;
    }

    rotate.x = pitch;
    rotate.y = yaw;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject->transform.rotation += mouseSensitivity * dt * glm::normalize(rotate);
    }

    // limit pitch values between about +/- 85ish degrees
    gameObject->transform.rotation.x = glm::clamp(gameObject->transform.rotation.x, -1.5f, 1.5f);
    gameObject->transform.rotation.y = glm::mod(gameObject->transform.rotation.y, glm::two_pi<float>());

    float camYaw = camera.cameraEntity->transform.rotation.y;
    float camPitch = camera.cameraEntity->transform.rotation.x;


    camera.front = {sin(camYaw),
                    -sin(camPitch) * cos(camPitch),
                    cos(camYaw)};
    camera.right = {camera.front.z, 0.f, -camera.front.x};
    camera.up = {0.f, -1.f, 0.f};
}
