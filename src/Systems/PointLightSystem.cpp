//
// Created by apgra on 1/12/2024.
//

#include <array>
#include <iostream>
#include <PointLightSystem.h>
#include <map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace BE {
    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };
}

BE::PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : mDevice(device)
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

BE::PointLightSystem::~PointLightSystem() {
    vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}


void BE::PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(PointLightPushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};



    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if(vkCreatePipelineLayout(mDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void BE::PointLightSystem::createPipeline(VkRenderPass renderPass) {
    assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    Pipeline::enableAlphaBlending(pipelineConfig);
    pipelineConfig.attributeDescriptions.clear();
    pipelineConfig.bindingDescriptions.clear();
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_unique<Pipeline>(mDevice,
                                           "../Shaders/point_light.vert.spv",
                                           "../Shaders/point_light.frag.spv",
                                           pipelineConfig);


}

void BE::PointLightSystem::render(FrameInfo &frameInfo) {
    std::map<float, GameObject::id_t> sorted;
    for(auto& kv : frameInfo.gameObjects) {
        auto &obj = kv.second;
        if (obj.pointLight == nullptr) continue;

        //calculate distance
        auto offset = frameInfo.camera.getPosition() - obj.transform.translation;
        float distSquared = glm::dot(offset, offset);
        sorted[distSquared] = obj.getID();
    }

    mPipeline->bind(frameInfo.commandBuffer);

    vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
            );

    //Render lights back to front
    for(auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        auto &obj = frameInfo.gameObjects.at(it->second);

        PointLightPushConstants pushConstants{
            glm::vec4(obj.transform.translation, 1.0f),
            glm::vec4(obj.color, obj.pointLight->lightIntensity),
            obj.transform.scale.x
        };

        vkCmdPushConstants(
                frameInfo.commandBuffer,
                mPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &pushConstants
                );

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}

void BE::PointLightSystem::update(BE::FrameInfo &frameInfo, GlobalUBO &ubo) {
    auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            frameInfo.frameTime,
            {0.f, -1.f, 0.f});

    int lightIndex = 0;
    for(auto& kv : frameInfo.gameObjects){
        auto& obj = kv.second;
        if(obj.pointLight == nullptr) continue;

        assert(lightIndex < MAX_LIGHTS && "Point Lights exceeded max limit");

        ///update light position
        obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

        //copy light data to ubo
        ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
        ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

        lightIndex++;
    }

    ubo.numLights = lightIndex;
}


