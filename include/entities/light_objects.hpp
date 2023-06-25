#ifndef YORNA_ENTITIES_LIGHT_OBJECTS
#define YORNA_ENTITIES_LIGHT_OBJECTS

#include <entities/transform_component.hpp>

namespace yorna {

    struct PointLight {
        PointLight(float intensity = 3.0f, glm::vec3 color = glm::vec3 { 1.0f }) noexcept;

        float intensity {};
        glm::vec3 color {};
        glm::vec3 position {};
    };

    struct SpotLight {
        SpotLight(float coneAngle, glm::vec3 direction, float intensity = 3.0f, glm::vec3 color = glm::vec3 { 1.0f }) noexcept;

        float coneAngle {};
        glm::vec3 coneDirection {};
        float intensity {};
        glm::vec3 color {};
        glm::vec3 position {};
    };

    struct DirectionalLight {
        DirectionalLight(glm::vec3 direction, float intensity = 3.0f, glm::vec3 color = glm::vec3 { 1.0f }) noexcept;

        glm::vec3 direction {};
        float intensity {};
        glm::vec3 color {};
    };

} // namespace yorna

#endif