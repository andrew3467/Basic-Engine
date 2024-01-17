//
// Created by apgra on 1/12/2024.
//

#include <array>
#include <iostream>
#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace BE{
    struct SimplePushConstantData{
        glm::mat4 model{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };
}

BE::SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : mDevice(device)
{
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

BE::SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(mDevice.device(), mPipelineLayout, nullptr);
}


void BE::SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

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

void BE::SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(mPipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = mPipelineLayout;

    mPipeline = std::make_unique<Pipeline>(mDevice,
                                           "../Shaders/simple_shader.vert.spv",
                                           "../Shaders/simple_shader.frag.spv",
                                           pipelineConfig);


}

void BE::SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
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

    for(auto& kv : frameInfo.gameObjects){
        auto &obj = kv.second;

        if(obj.model == nullptr) continue;

        SimplePushConstantData push{};
        auto modelMatrix = obj.transform.mat4();
        push.model = obj.transform.mat4();
        push.normalMatrix = obj.transform.normalMatrix();

        vkCmdPushConstants(
                frameInfo.commandBuffer,
                mPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
        );

        obj.model->bind(frameInfo.commandBuffer);
        obj.model->draw(frameInfo.commandBuffer);
    }
}


