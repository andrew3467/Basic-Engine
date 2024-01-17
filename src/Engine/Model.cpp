#include <Model.h>

// std
#include <cassert>
#include <cstring>
#include <Utils.h>
#include <unordered_map>


#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


namespace std {
    template<>
    struct hash<BE::Model::Vertex>{
        size_t operator()(BE::Model::Vertex const &vertex) const {
            size_t seed = 0;
            BE::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}


namespace BE {

    Model::Model(Device &device, const Data &data) : mDevice{device} {
        createVertexBuffers(data.vertices);
        createIndexBuffers(data.indices);
    }

    Model::~Model() {

    }

    void Model::draw(VkCommandBuffer commandBuffer) {
        if(mHasIndexBuffer){
            vkCmdDrawIndexed(commandBuffer, mIndexCount, 1, 0, 0, 0);
        }else{
            vkCmdDraw(commandBuffer, mVertexCount, 1, 0, 0);
        }
    }

    void Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {mVertexBuffer->getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if(mHasIndexBuffer){
            vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        mVertexCount = static_cast<uint32_t>(vertices.size());
        assert(mVertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * mVertexCount;
        uint32_t vertexSize = sizeof(vertices[0]);

        Buffer stagingBuffer{
            mDevice,
            vertexSize,
            mVertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        };

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)vertices.data());

        mVertexBuffer = std::make_unique<Buffer>(
                mDevice,
                vertexSize,
                mVertexCount,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                );

        mDevice.copyBuffer(stagingBuffer.getBuffer(), mVertexBuffer->getBuffer(), bufferSize);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        mIndexCount = static_cast<uint32_t>(indices.size());
        mHasIndexBuffer = mIndexCount > 0;

        if(!mHasIndexBuffer){
            return;
        }

        VkDeviceSize bufferSize = sizeof(indices[0]) * mIndexCount;
        uint32_t indexSize = sizeof(indices[0]);

        Buffer stagingBuffer(
                mDevice,
                indexSize,
                mIndexCount,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                );

        stagingBuffer.map();
        stagingBuffer.writeToBuffer((void*)indices.data());

        mIndexBuffer = std::make_unique<Buffer>(
                mDevice,
                indexSize,
                mIndexCount,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

        mDevice.copyBuffer(stagingBuffer.getBuffer(), mIndexBuffer->getBuffer(), bufferSize);
    }

    std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filePath) {
        Data builder{};
        builder.loadModel(filePath);

        return std::make_unique<Model>(device, builder);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.emplace_back(0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, position));
        attributeDescriptions.emplace_back(1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color));
        attributeDescriptions.emplace_back(2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, normal));
        attributeDescriptions.emplace_back(3,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, uv));




        return attributeDescriptions;
    }

    void Model::Data::loadModel(const std::string &filePath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string error;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filePath.c_str())){
            throw std::runtime_error(warn + error);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices;
        for(const auto& shape : shapes){
            for(const auto& index : shape.mesh.indices){
                Vertex vertex{};

                if(index.vertex_index >= 0){
                    vertex.position = {
                            attrib.vertices[3 * index.vertex_index + 0],
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = {
                            attrib.colors[3 * index.vertex_index + 0],
                            attrib.colors[3 * index.vertex_index + 1],
                            attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if(index.normal_index >= 0){
                    vertex.normal = {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if(index.texcoord_index >= 0){
                    vertex.uv = {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if(uniqueVertices.count(vertex) == 0){
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

    }


}  // namespace be