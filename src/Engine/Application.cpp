//
// Created by apgra on 1/12/2024.
//

#include <array>
#include "Application.h"
#include <Camera.h>
#include <InputMovementController.h>
#include <Buffer.h>
#include "SimpleRenderSystem.h"
#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>



BE::Application::Application() {
    mGlobalPool = DescriptorPool::Builder(mDevice)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
    loadGameObjects();
}

BE::Application::~Application() {

}

void BE::Application::Run() {
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for(int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
                mDevice,
                sizeof(GlobalUBO),
                SwapChain::MAX_FRAMES_IN_FLIGHT,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                mDevice.properties.limits.minUniformBufferOffsetAlignment
        );
        uboBuffers[i]->map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(mDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
            .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); ++i) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *mGlobalPool)
        .writeBuffer(0, &bufferInfo)
        .build(globalDescriptorSets[i]);
    }



    SimpleRenderSystem simpleRenderSystem(mDevice, mRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
    PointLightSystem pointLightSystem(mDevice, mRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());

    Camera camera{};

    camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

    auto viewerObject = GameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5f;
    InputMovementController camController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while(!mWindow.shouldClose()){
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        camController.moveInPlaneXZ(mWindow.getGLFWWindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = mRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 1000.0f);

        if(auto commandBuffer = mRenderer.beginFrame()){
            int frameIndex = mRenderer.getFrameIndex();

            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                mGameObjects
            };

            //Update
            GlobalUBO ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();


            //Render
            mRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo);    //Must be called before point lights
            pointLightSystem.render(frameInfo);
            mRenderer.endSwapChainRenderPass(commandBuffer);
            mRenderer.endFrame();


        }
    }

    vkDeviceWaitIdle(mDevice.device());
}

void BE::Application::loadGameObjects() {
    std::shared_ptr<Model> vaseModel = Model::createModelFromFile(mDevice, "../models/smooth_vase.obj");
    std::shared_ptr<Model> quadModel = Model::createModelFromFile(mDevice, "../models/quad_model.obj");

    auto vase1 = GameObject::createGameObject();
    vase1.model = vaseModel;
    vase1.transform.translation = {-1.0f, 0.5f, 0.0f};
    vase1.transform.scale = {3.0f, 1.5f, 3.0f};
    mGameObjects.emplace(vase1.getID(), std::move(vase1));

    auto vase2 = GameObject::createGameObject();
    vase2.model = vaseModel;
    vase2.transform.translation = {0.0f, 0.5f, 0.0f};
    vase2.transform.scale = {3.0f, 1.5f, 3.0f};
    mGameObjects.emplace(vase2.getID(), std::move(vase2));

    auto vase3 = GameObject::createGameObject();
    vase3.model = vaseModel;
    vase3.transform.translation = {1.0f, 0.5f, 0.0f};
    vase3.transform.scale = {3.0f, 1.5f, 3.0f};
    mGameObjects.emplace(vase3.getID(), std::move(vase3));

    auto floor = GameObject::createGameObject();
    floor.model = quadModel;
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = {3.0f, 1.0f, 3.0f};
    mGameObjects.emplace(floor.getID(), std::move(floor));

    std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}  //
    };

    for (int i = 0; i < lightColors.size(); i++) {
        auto pointLight = GameObject::createPointLight(0.2f);
        pointLight.color = lightColors[i];
        auto rotateLight = glm::rotate(
                glm::mat4(1.f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.f, -1.f, 0.f});
        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        mGameObjects.emplace(pointLight.getID(), std::move(pointLight));
    }
}

