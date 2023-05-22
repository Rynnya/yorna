#ifndef ENTITIES_POINT_LIGHT
#define ENTITIES_POINT_LIGHT

#include <entities/transform_component.hpp>

namespace editor {

    struct PointLight {
        PointLight(float intensity = 3.0f, float radius = 0.1f, glm::vec3 color = glm::vec3 { 1.0f }) noexcept;

        float intensity {};
        glm::vec3 color {};

        TransformComponent transform {};
    };

}

#endif