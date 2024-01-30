//
// Created by apgra on 1/14/2024.
//

#ifndef BASIC_ENGINE_CAMERA_H
#define BASIC_ENGINE_CAMERA_H


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Entity.h"


namespace BE {
    class Camera {
    public:
        Camera();

        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float FOV, float aspect, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);


        const glm::mat4& getProjection() const {return projectionMatrix;}
        const glm::mat4& getView() const {return viewMatrix;}
        const glm::mat4& getInverseView() const {return inverseViewMatrix;}
        const glm::vec3 getPosition() const {return glm::vec3(inverseViewMatrix[3]);}

        std::unique_ptr<Entity> cameraEntity;

        bool firstMouse = true;
        float lastX, lastY;

        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;

    private:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        glm::mat4 inverseViewMatrix{1.0f};
    };
}


#endif //BASIC_ENGINE_CAMERA_H
