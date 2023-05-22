#ifndef SYSTEMS_SANDBOX_STRUCTURES
#define SYSTEMS_SANDBOX_STRUCTURES

#include <coffee/graphics/descriptors.hpp>

#include <entities/point_light.hpp>

namespace editor {

    constexpr size_t maxAmountOfPointLights = 10;

    struct MainPushConstants {
        glm::mat4 transform { 1.0f };
        glm::mat4 normal { 1.0f };
    };

    struct LightPushConstants {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct MVPUniformBuffer {
        glm::mat4 projection {};
        glm::mat4 view {};
        glm::mat4 inverseView {};
        glm::vec4 ambientLightColor { 1.0f, 1.0f, 1.0f, 0.07f };
    };

    struct LightPoint {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct LightUniformBuffer {
        LightPoint lightPoints[maxAmountOfPointLights] {};
        uint32_t size {};
    };

    struct FrameInfo {
        MVPUniformBuffer mvpUbo {};
        LightUniformBuffer lightUbo {};

        coffee::DescriptorSetPtr descriptorSet;
        coffee::BufferPtr mvpBuffer;
        coffee::BufferPtr lightBuffer;
    };

}

#endif
