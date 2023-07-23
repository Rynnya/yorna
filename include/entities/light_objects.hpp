#ifndef YORNA_ENTITIES_LIGHT_OBJECTS
#define YORNA_ENTITIES_LIGHT_OBJECTS

#include <entities/transform_component.hpp>

namespace yorna {

    struct PointLight {
        // World-space position of light
        glm::vec3 position { 0.0f };
        // Radius of light
        float radius { 0.0f };
        // RGB color of light
        alignas(16) glm::vec3 color { 1.0f };
    };

    struct SpotLight {
        // World-space position of light
        glm::vec3 position { 0.0f };
        // Radius of light
        float radius { 0.0f };
        // RGB color of light
        glm::vec3 color { 1.0f };
        alignas(16) glm::vec3 coneDirection { 0.0f };
        // Cone angle in degress
        float coneAngle { 30.0f };
    };

    struct DirectionalLight {
        alignas(16) glm::vec3 direction {};
        alignas(16) glm::vec3 color {};
    };

    // Sanity checks

    static_assert(sizeof(PointLight) == 32);
    static_assert(sizeof(SpotLight) == 48);
    static_assert(sizeof(DirectionalLight) == 32);

} // namespace yorna

#endif