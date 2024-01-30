//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_SIMPLE_RENDER_SYSTEM_H
#define BASIC_ENGINE_SIMPLE_RENDER_SYSTEM_H


#include "Pipeline.h"
#include "Entity.h"

#include <memory>
#include "Camera.h"
#include "FrameInfo.h"

namespace BE {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

        void createPipeline(VkRenderPass renderPass);



    private:
        Device &mDevice;
        std::unique_ptr<Pipeline> mPipeline;
        VkPipelineLayout mPipelineLayout;
    };
}


#endif //BASIC_ENGINE_SIMPLE_RENDER_SYSTEM_H
