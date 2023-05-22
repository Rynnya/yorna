#ifndef ENTITIES_MODEL
#define ENTITIES_MODEL

#include <coffee/coffee.hpp>

#include <entities/transform_component.hpp>

namespace editor {

    // TODO: Made it compatable with entt

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
        Model(const coffee::GPUDevicePtr& device, const coffee::ModelPtr& model, const coffee::SamplerPtr& textureSampler);

        void updateMeshesInformation();

        const coffee::GPUDevicePtr& device;
        TransformComponent transform {};
        coffee::ModelPtr model = nullptr;
        std::vector<MeshInformation> meshesInformation {};
        coffee::DescriptorLayoutPtr layout = nullptr;
        std::vector<coffee::BufferPtr> meshesInformationBuffers {};
        std::vector<coffee::DescriptorSetPtr> descriptors {};

    private:
        void initialize(const coffee::SamplerPtr& textureSampler);
    };

    using UModel = std::unique_ptr<Model>;
}

#endif
