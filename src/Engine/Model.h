#pragma once

#include <Device.h>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>
#include "Buffer.h"

namespace BE {
    class Model {
    public:
        struct Vertex {
            bool operator==(const Vertex& other) const {
                return position == other.position &&
                color == other.color &&
                normal == other.normal &&
                uv == other.uv;
            }

            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Data{
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &filePath);
        };

        Model(Device &device, const Data &data);
        ~Model();

        Model(const Model &) = delete;
        Model &operator=(const Model &) = delete;

        static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filePath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t > &indices);

        Device &mDevice;
        bool mHasIndexBuffer = false;

        std::unique_ptr<Buffer> mVertexBuffer;
        uint32_t mVertexCount;

        std::unique_ptr<Buffer> mIndexBuffer;
        uint32_t mIndexCount;
    };
}  // namespace lve