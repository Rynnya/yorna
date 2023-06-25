#ifndef YORNA_ENTITIES_TRANSFORM_COMPONENT
#define YORNA_ENTITIES_TRANSFORM_COMPONENT

#include <coffee/utils/non_copyable.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace yorna {

    struct TransformComponent {
        glm::vec3 translation {};
        glm::vec3 scale { 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation {};

        glm::mat4 mat4() const noexcept;
        glm::mat3 normal() const noexcept;
    };

}

#endif