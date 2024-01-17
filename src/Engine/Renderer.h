//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_RENDERER_H
#define BASIC_ENGINE_RENDERER_H


#include "Window.h"
#include <SwapChain.h>

#include <memory>
#include <cassert>

namespace BE {
    class Renderer {
    public:
        Renderer(Window &window, Device &device);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer &operator=(const Renderer&) = delete;

        int getFrameIndex() const {
            assert(mIsFrameStarted && "Cannot get frmae index when frame not in progress");
            return mCurrentFrameIndex;
        }

        VkRenderPass getSwapChainRenderPass() const {return mSwapChain->getRenderPass();}
        float getAspectRatio() const {return mSwapChain->extentAspectRatio();}

        bool isFrameInProgress() const {return mIsFrameStarted;}
        VkCommandBuffer getCurrentCommandBuffer(){
            assert(mIsFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[mCurrentFrameIndex];
        }


        VkCommandBuffer beginFrame();
        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void createCommandBuffers();
        void freeCommandBuffers();

        void recreateSwapChain();



    private:
        Window &mWindow;
        Device &mDevice;
        std::unique_ptr<SwapChain> mSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t mCurrentImageIndex = 0;
        bool mIsFrameStarted = false;

        int mCurrentFrameIndex = 0;
    };
}


#endif //BASIC_ENGINE_Renderer_H
