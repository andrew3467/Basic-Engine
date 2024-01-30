//
// Created by apgra on 1/12/2024.
//

#include <stdexcept>
#include "Window.h"
#include "Application.h"

BE::Window::Window(int w, int h, std::string t) : width(w), height(h), title(t) {
    Init();
}

BE::Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void BE::Window::Init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);   //Disable rendering API
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);     //Vulkan needs special window resizing

    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    //glfwSetWindowUserPointer(mWindow, this);

    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
}

void BE::Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if(glfwCreateWindowSurface(instance, mWindow, nullptr, surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create VK Surface");
    }
}

void BE::Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->mWindow.frameBufferResized = true;
    app->mWindow.width = width;
    app->mWindow.height = height;
}
