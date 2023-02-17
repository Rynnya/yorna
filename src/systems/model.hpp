#ifndef GAME_MODEL_CLASS
#define GAME_MODEL_CLASS

#include <coffee/utils/utils.hpp>
#include <coffee/engine.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace game {

    // Forward declaration
    class MainSystem;

    class Model : coffee::NonMoveable {
    public:
        struct Vertex {
            glm::vec3 position {};
            glm::vec3 color {};
            glm::vec3 normal {};
            glm::vec2 uv {};

            static std::vector<coffee::InputElement> getElementDescriptions();

            bool operator==(const Vertex& other) const noexcept;
        };

        Model(coffee::Engine& engine, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices = {});
        Model(coffee::Engine& engine, const std::string& filePath);
        ~Model() noexcept = default;

        coffee::Buffer vertexBuffer = nullptr;
        uint32_t vertexCount = 0;

        coffee::Buffer indexBuffer = nullptr;
        uint32_t indexCount = 0;

    private:
        coffee::Engine& engine_;

        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);
    };

}

namespace std {

    template <>
    struct hash<game::Model::Vertex> {
        size_t operator()(const game::Model::Vertex& vertex) const {
            size_t seed = 0;
            coffee::Utils::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };

}

#endif