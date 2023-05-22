#include <entities/point_light.hpp>

#include <utility>

namespace editor {

    PointLight::PointLight(float intensity, float radius, glm::vec3 color) noexcept
        : intensity { intensity }
        , color { color }
    {
        transform.scale.r = radius;
    }

}