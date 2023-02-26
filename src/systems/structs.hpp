#ifndef GAME_STRUCTS
#define GAME_STRUCTS

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace game {

    static constexpr size_t amountOfLightPoints = 10;

    struct MVPUniformBuffer {
        glm::mat4 projection {};
        glm::mat4 view {};
        glm::mat4 inverseView {};
        glm::vec4 ambientLightColor { 1.0f, 1.0f, 1.0f, 0.02f };
    };

    struct LightPoint {
        glm::vec4 position {};
        glm::vec4 color {};
    };

    struct LightUniformBuffer {
        LightPoint lightPoint {};
    };

    class StaticObjects {
    public:
        static inline std::vector<LightUniformBuffer> lightUniformBuffers {};
        static inline std::vector<MVPUniformBuffer> mvpUniformBuffers {};
    };

}

#endif