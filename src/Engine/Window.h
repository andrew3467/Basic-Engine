//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_WINDOW_H
#define BASIC_ENGINE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>


namespace BE {
    class Window {
    public:
        Window(int w, int h, std::string t);
        ~Window();

        Window(const Window&) = delete;
        Window &operator=(const Window&) = delete;

        bool shouldClose(){return glfwWindowShouldClose(mWindow);}
        bool wasWindowResized(){return frameBufferResized;}
        void resetWindowResizedFlag(){frameBufferResized = false;}

        VkExtent2D getExtent() {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
        GLFWwindow* getGLFWWindow() const {return mWindow;}

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        void Init();


    public:
        GLFWwindow *mWindow;

        int width;
        int height;
        bool frameBufferResized;

        std::string title;
    };
}


#endif //BASIC_ENGINE_WINDOW_H
