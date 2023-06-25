#ifndef YORNA_ENTITIES_MODEL
#define YORNA_ENTITIES_MODEL

#include <coffee/coffee.hpp>

#include <entities/transform_component.hpp>

namespace yorna {

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
        Model(const coffee::graphics::DevicePtr& device, const coffee::ModelPtr& model, const coffee::graphics::SamplerPtr& textureSampler);

        void updateMeshesInformation();

        const coffee::graphics::DevicePtr& device;
        coffee::ModelPtr model = nullptr;
        TransformComponent transform {};
        std::vector<size_t> visibleMeshes {};
        std::vector<MeshInformation> meshesInformation {};
        std::vector<coffee::graphics::BufferPtr> meshesInformationBuffers {};
        coffee::graphics::DescriptorLayoutPtr layout = nullptr;
        std::vector<coffee::graphics::DescriptorSetPtr> descriptors {};

    private:
        void initialize(const coffee::graphics::SamplerPtr& textureSampler);
    };

}

#endif
