//
// Created by apgra on 1/12/2024.
//

#include <array>
#include <iostream>
#include "Renderer.h"


BE::Renderer::Renderer(Window &window, Device &device)
    : mWindow(window), mDevice(device)
{
    recreateSwapChain();
    createCommandBuffers();
}

BE::Renderer::~Renderer() {
    freeCommandBuffers();
}

void BE::Renderer::createCommandBuffers() {
    commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(mDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to create command buffers");
    }


}

void BE::Renderer::freeCommandBuffers() {
    vkFreeCommandBuffers(mDevice.device(),
                         mDevice.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
}


void BE::Renderer::recreateSwapChain() {
    auto extent = mWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = mWindow.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(mDevice.device());

    if (mSwapChain == nullptr) {
        mSwapChain = std::make_unique<SwapChain>(mDevice, extent);
    } else {
        std::shared_ptr<SwapChain> oldSwapChain = std::move(mSwapChain);
        mSwapChain = std::make_unique<SwapChain>(mDevice, extent, oldSwapChain);

        if (!oldSwapChain->compareSwapFormats(*mSwapChain.get())) {
            throw std::runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}

VkCommandBuffer BE::Renderer::beginFrame()  {
    assert(!mIsFrameStarted && "Can't call beginFrame while already in progress");


    auto result = mSwapChain->acquireNextImage(&mCurrentImageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR){
        recreateSwapChain();
        return nullptr;
    }


    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("failed to acquire swapchain image");
    }

    mIsFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    return commandBuffer;
}

void BE::Renderer::endFrame() {
    assert(mIsFrameStarted && "Can't call endFrame while frame is not in progress");
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = mSwapChain->submitCommandBuffers(&commandBuffer, &mCurrentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        mWindow.wasWindowResized()) {
        mWindow.resetWindowResizedFlag();
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    mIsFrameStarted = false;
    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}

void BE::Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(mIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mSwapChain->getRenderPass();
    renderPassInfo.framebuffer = mSwapChain->getFrameBuffer(mCurrentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(mSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(mSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, mSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void BE::Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(mIsFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);

}


