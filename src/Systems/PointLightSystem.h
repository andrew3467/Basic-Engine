//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_POINT_POINT_SYSTEM_H
#define BASIC_ENGINE_POINT_POINT_SYSTEM_H


#include "Pipeline.h"
#include "Entity.h"

#include <memory>
#include "Camera.h"
#include "FrameInfo.h"



namespace BE {
    class PointLightSystem {
    public:
        PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem &operator=(const PointLightSystem&) = delete;

        void update(FrameInfo &frameInfo, GlobalUBO &ubo);
        void render(FrameInfo &frameInfo);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);

        void createPipeline(VkRenderPass renderPass);



    private:
        Device &mDevice;
        std::unique_ptr<Pipeline> mPipeline;
        VkPipelineLayout mPipelineLayout;
    };
}


#endif //BASIC_ENGINE_POINT_POINT_SYSTEM_H
