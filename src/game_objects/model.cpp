#include "model.hpp"

#include <glm/glm.hpp>

namespace game {

    Model::Model(
        const std::string& filename,
        TransformComponent transform,
        const coffee::Sampler& textureSampler
    )
        : transform { transform }
        , model { coffee::Engine::importModel(filename) }
    {
        initialize(textureSampler);
    }

    Model::Model(
        const std::string& modelFile,
        const std::string& materialsFile,
        TransformComponent transform,
        const coffee::Sampler& textureSampler
    )
        : transform { transform }
        , model { coffee::Engine::importModel(modelFile, { materialsFile }) }
    {
        initialize(textureSampler);
    }

    void Model::updateMeshesInformation() {
        for (size_t i = 0; i < meshesInformationBuffers.size(); i++) {
            const auto& buffer = meshesInformationBuffers[i];
            buffer->write(meshesInformation[i]);
        }
    }

    void Model::initialize(const coffee::Sampler& textureSampler) {
        const auto& meshes = model->meshes;
        meshesInformation.resize(meshes.size());

        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};
        coffee::DescriptorBindingInfo binding{};

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[0] = binding;

        binding.type = coffee::DescriptorType::ImageAndSampler;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[1] = binding;
        bindings[2] = binding;
        bindings[3] = binding;
        bindings[4] = binding;
        bindings[5] = binding;

        layout = coffee::Factory::createDescriptorLayout(bindings);

        coffee::DescriptorWriter writer = coffee::DescriptorWriter{ layout };

        for (size_t i = 0; i < meshes.size(); i++) {
            coffee::BufferConfiguration bufferConfiguration{};
            bufferConfiguration.usage = coffee::BufferUsage::Uniform;
            bufferConfiguration.properties = coffee::MemoryProperty::HostVisible;
            bufferConfiguration.instanceCount = 1U;
            bufferConfiguration.instanceSize = static_cast<uint32_t>(sizeof(MeshInformation));
            meshesInformationBuffers.push_back(coffee::Factory::createBuffer(bufferConfiguration));

            const auto& meshMaterials = meshes[i]->materials;
            auto& currentMeshInfo = meshesInformation[i];

            currentMeshInfo.diffuseColor = meshMaterials.modifiers.diffuseColor;
            currentMeshInfo.specularColor = meshMaterials.modifiers.specularColor;
            currentMeshInfo.metallicFactor = meshMaterials.modifiers.metallicFactor;
            currentMeshInfo.roughnessFactor = meshMaterials.modifiers.roughnessFactor;

            currentMeshInfo.textureFlags = meshMaterials.getTextureFlags();

            writer.addBuffer(0, meshesInformationBuffers[i]);
            writer.addTexture(1, coffee::ResourceState::ShaderResource, meshMaterials.read(coffee::TextureType::Diffuse), textureSampler);
            writer.addTexture(2, coffee::ResourceState::ShaderResource, meshMaterials.read(coffee::TextureType::Specular), textureSampler);
            writer.addTexture(3, coffee::ResourceState::ShaderResource, meshMaterials.read(coffee::TextureType::Normals), textureSampler);
            writer.addTexture(4, coffee::ResourceState::ShaderResource, meshMaterials.read(coffee::TextureType::Metallic), textureSampler);
            writer.addTexture(5, coffee::ResourceState::ShaderResource, meshMaterials.read(coffee::TextureType::Roughness), textureSampler);

            descriptors.push_back(coffee::Factory::createDescriptorSet(writer));
        }

        updateMeshesInformation();
    }

}