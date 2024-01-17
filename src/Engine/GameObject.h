//
// Created by apgra on 1/14/2024.
//

#ifndef BASIC_ENGINE_GAMEOBJECT_H
#define BASIC_ENGINE_GAMEOBJECT_H

#include <Model.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>


namespace BE {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        GameObject(const GameObject&) = default;
        GameObject &operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject &operator=(GameObject&&) = delete;

        static GameObject createGameObject(){
            static id_t curID = 0;
            return  GameObject(curID++);
        }

        static GameObject createPointLight(float intensity = 5.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

        id_t getID() const{
            return id;
        }

        glm::vec3 color{};
        TransformComponent transform{};

        //Optional Components
        std::shared_ptr<Model> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        GameObject(id_t objID) : id(objID) {}

    private:
        id_t id;

    };
}


#endif //BASIC_ENGINE_GAMEOBJECT_H
