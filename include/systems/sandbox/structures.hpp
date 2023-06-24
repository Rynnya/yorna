#ifndef SYSTEMS_SANDBOX_STRUCTURES
#define SYSTEMS_SANDBOX_STRUCTURES

#include <coffee/graphics/descriptors.hpp>

#include <entities/light_objects.hpp>

namespace editor {

    constexpr size_t maxAmountOfPointLights = 64;
    constexpr size_t maxAmountOfSpotLights = 32;

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

    struct PointLightUBO {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct SpotLightUBO {
        glm::vec4 position {};
        glm::vec4 color {};
        glm::vec4 coneDirection {}; // w is coneAngle
    };

    struct DirectionalLightUBO {
        glm::vec4 direction {};
        glm::vec4 color {};
    };

    struct LightUniformBuffer {
        glm::mat4 sunlightSpaceMatrix { 1.0f };
        glm::vec4 sunlightDirection { 1.0f };
        glm::vec4 sunlightColor { 1.0f };
        uint32_t amountOfPointLights {};
        uint32_t amountOfSpotLights {};
        alignas(16) PointLightUBO pointLights[maxAmountOfPointLights] {};
        SpotLightUBO spotLights[maxAmountOfSpotLights] {};
    };

    static_assert(offsetof(LightUniformBuffer, pointLights) == sizeof(glm::mat4) + 3 * sizeof(glm::vec4), "Invalid alignment");

    struct FrameInfo {
        MVPUniformBuffer mvpUbo {};
        LightUniformBuffer lightUbo {};

        coffee::graphics::DescriptorSetPtr descriptorSet;
        coffee::graphics::BufferPtr mvpBuffer;
        coffee::graphics::BufferPtr lightBuffer;
    };

}

#endif
