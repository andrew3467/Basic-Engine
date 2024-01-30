//
// Created by apgra on 1/14/2024.
//

#ifndef BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H
#define BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H

#include <Entity.h>
#include <Window.h>
#include "Camera.h"


namespace BE {
    class InputMovementController {
    public:
        struct KeyMappings{
            int closeGame = GLFW_KEY_ESCAPE;


            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, BE::Camera &camera);
        void rotateCameraXZ(GLFWwindow *window, float dt, BE::Camera &camera);

    private:
        KeyMappings keys{};
        float moveSpeed = 3.0f;
        float mouseSensitivity = 5.0f;

    };
}


#endif //BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H
