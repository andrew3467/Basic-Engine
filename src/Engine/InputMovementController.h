//
// Created by apgra on 1/14/2024.
//

#ifndef BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H
#define BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H

#include <GameObject.h>
#include <Window.h>


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

            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, GameObject &gameObject);

    private:
        KeyMappings keys{};
        float moveSpeed = 3.0f;
        float turnSpeed = 1.5f;

    };
}


#endif //BASIC_ENGINE_INPUTMOVEMENTCONTROLLER_H
