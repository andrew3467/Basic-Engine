//
// Created by apgra on 1/14/2024.
//

#ifndef BASIC_ENGINE_ENTITY_H
#define BASIC_ENGINE_ENTITY_H

#include <Model.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>
#include "Components/BaseComponent.h"


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

    class Entity {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, Entity>;

        Entity(const Entity&) = default;
        Entity &operator=(const Entity&) = delete;
        Entity(Entity&&) = default;
        Entity &operator=(Entity&&) = delete;

        static Entity createEntity(){
            static id_t curID = 0;
            Entity obj(curID++);

            return obj;
        }

        static Entity createPointLight(float intensity = 5.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));


        id_t getID() const{
            return id;
        }

        std::vector<std::shared_ptr<Entity>> getChildren() const {
            return mChildren;
        }

        std::shared_ptr<Entity> getParent() const {
            return mParent;
        }



        glm::vec3 color{};
        TransformComponent transform{};

        //Optional Components
        std::shared_ptr<Model> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        Entity(id_t objID) : id(objID) {}

    private:
        id_t id;

        std::vector<std::shared_ptr<Entity>> mChildren;
        std::shared_ptr<Entity> mParent;

    };
}


#endif //BASIC_ENGINE_ENTITY_H
