#ifndef GAME_MODEL
#define GAME_MODEL

#include <coffee/engine.hpp>

#include "game_object.hpp"

namespace game {

    enum class TextureFlags : uint32_t {
        Diffuse = 1 << 0,
        Specular = 1 << 1,
        Normals = 1 << 2
    };

    // use glm::vec4 for automatic padding in c++ and vec3 in shader when you need exactly vec3
    // also put padding vector if you need to use basic types (like int or uint)

    class MeshInformation {
    public:
        uint32_t textureFlags = 0U;
        float shininess = 1.0f;
        glm::vec2 _padding {};
        glm::vec4 diffuseColor {};
        glm::vec4 specularColor {};
        glm::vec4 ambientColor {};

        void setTextureFlag(TextureFlags flag);
        void resetTextureFlag(TextureFlags flag);
    };

    class Model {
    public:
        Model(coffee::Engine& engine, const std::string& filename, TransformComponent transform, const coffee::Sampler& textureSampler);

        void updateMeshesInformation();

        TransformComponent transform;
        coffee::Model model = nullptr;
        std::vector<MeshInformation> meshesInformation {};
        coffee::DescriptorLayout layout = nullptr;
        std::vector<coffee::Buffer> meshesInformationBuffers {};
        std::vector<coffee::DescriptorSet> descriptors {};
    };

    using UModel = std::unique_ptr<Model>;
}

#endif
