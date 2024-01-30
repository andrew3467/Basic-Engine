//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_APPLICATION_H
#define BASIC_ENGINE_APPLICATION_H


#include "Window.h"
#include <Entity.h>
#include <Renderer.h>
#include <Descriptor.h>

#include <memory>

namespace BE {
    class Application {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application &operator=(const Application&) = delete;

        void Run();

    private:
        void loadGameObjects();

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    public:
        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;

        Window mWindow{WIDTH, HEIGHT, "Hello Vulkan"};


        float mFOV = 60.0f;
        float pitch = 0.0f;
        float yaw = 0.0f;

    private:
        Device mDevice{mWindow};
        Renderer mRenderer{mWindow, mDevice};

        //Declaration order matters
        std::unique_ptr<DescriptorPool> mGlobalPool{};
        Entity::Map mEntities;
    };
}


#endif //BASIC_ENGINE_APPLICATION_H
