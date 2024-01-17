//
// Created by apgra on 1/12/2024.
//

#ifndef BASIC_ENGINE_APPLICATION_H
#define BASIC_ENGINE_APPLICATION_H


#include "Window.h"
#include <GameObject.h>
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

    public:
        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;



    private:
        Window mWindow{WIDTH, HEIGHT, "Hello Vulkan"};
        Device mDevice{mWindow};
        Renderer mRenderer{mWindow, mDevice};

        //Declaration order matters
        std::unique_ptr<DescriptorPool> mGlobalPool{};
        GameObject::Map mGameObjects;
    };
}


#endif //BASIC_ENGINE_APPLICATION_H
