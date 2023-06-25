#include <entities/light_objects.hpp>

#include <utility>

namespace yorna {

    PointLight::PointLight(float intensity, glm::vec3 color) noexcept
        : intensity { intensity }
        , color { color }
    {}

    SpotLight::SpotLight(float coneAngle, glm::vec3 direction, float intensity, glm::vec3 color) noexcept
        : coneAngle { glm::radians(coneAngle) }
        , coneDirection { direction }
        , intensity { intensity }
        , color { color }
    {}

    DirectionalLight::DirectionalLight(glm::vec3 direction, float intensity, glm::vec3 color) noexcept
        : direction { direction }
        , intensity { intensity }
        , color { color }
    {}

}