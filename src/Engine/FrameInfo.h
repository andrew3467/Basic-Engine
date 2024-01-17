//
// Created by apgra on 1/15/2024.
//

#ifndef BASIC_ENGINE_FRAMEINFO_H
#define BASIC_ENGINE_FRAMEINFO_H

#include <Camera.h>

#include <vulkan/vulkan.h>
#include <GameObject.h>

namespace BE {
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map &gameObjects;
    };

#define MAX_LIGHTS 10
    struct PointLight{
        glm::vec4 position{}; //Ignore w
        glm::vec4 color{};
    };

    struct GlobalUBO{
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};

        glm::mat4 inverseView{1.0f};

        glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f};
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
    };
}

#endif //BASIC_ENGINE_FRAMEINFO_H
