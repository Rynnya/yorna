#include "light_object.hpp"

namespace game {

    LightObject::LightObject() noexcept : GameObject { createEmptyObject() } {}

    LightObject LightObject::createLightObject(float intensity, float radius, glm::vec3 color) noexcept {
        return LightObject { intensity, radius, color };
    }

    LightObject::LightObject(LightObject&& other) noexcept 
        : GameObject { std::move(other) }
        , intensity { std::exchange(other.intensity, 0.0f) }
        , color { std::exchange(other.color, glm::vec3 {}) }
    {}

    LightObject& LightObject::operator=(LightObject&& other) noexcept {
        this->intensity = std::exchange(other.intensity, 0.0f);
        this->color = std::exchange(other.color, glm::vec3 {});
        GameObject::operator=(std::move(other));

        return *this;
    }

    LightObject::LightObject(float intensity, float radius, glm::vec3 color) noexcept
        : GameObject { createGameObject() }
        , intensity { intensity }
        , color { color }
    {
        transform.scale.r = radius;
    }
}