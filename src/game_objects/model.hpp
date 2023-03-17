#ifndef GAME_MODEL
#define GAME_MODEL

#include <coffee/engine.hpp>

#include "game_object.hpp"

namespace game {

    // use glm::vec4 for automatic padding in c++ and vec3 in shader when you need exactly vec3
    // also put padding vector if you need to use basic types (like int or uint)

    class MeshInformation {
    public:
        glm::vec3 diffuseColor {};
        float metallicFactor = 0.0f;
        glm::vec3 specularColor {};
        float roughnessFactor = 0.0f;
        coffee::TextureType textureFlags = coffee::TextureType::None;
    };

    class Model {
    public:
        Model(
            const std::string& filename,
            TransformComponent transform,
            const coffee::Sampler& textureSampler);
        Model(
            const std::string& modelFile,
            const std::string& materialsFile,
            TransformComponent transform,
            const coffee::Sampler& textureSampler);

        void updateMeshesInformation();

        TransformComponent transform;
        coffee::Model model = nullptr;
        std::vector<MeshInformation> meshesInformation {};
        coffee::DescriptorLayout layout = nullptr;
        std::vector<coffee::Buffer> meshesInformationBuffers {};
        std::vector<coffee::DescriptorSet> descriptors {};

    private:
        void initialize(const coffee::Sampler& textureSampler);
    };

    using UModel = std::unique_ptr<Model>;
}

#endif
