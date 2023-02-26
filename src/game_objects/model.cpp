#include "model.hpp"

#include <coffee/utils/math.hpp>

namespace game {

    void MeshInformation::setTextureFlag(TextureFlags flag) {
        textureFlags |= static_cast<uint32_t>(flag);
    }

    void MeshInformation::resetTextureFlag(TextureFlags flag) {
        textureFlags &= ~static_cast<uint32_t>(flag);
    }

    Model::Model(coffee::Engine& engine, const std::string& filename, TransformComponent transform, const coffee::Sampler& textureSampler)
        : transform { transform }
        , model { engine.importModel(filename) }
    {
        const auto& meshes = model->meshes;
        meshesInformation.resize(meshes.size());

        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};
        coffee::DescriptorBindingInfo binding {};

        binding.type = coffee::DescriptorType::UniformBuffer;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[0] = binding;

        binding.type = coffee::DescriptorType::ImageAndSampler;
        binding.stages = coffee::ShaderStage::Fragment;
        bindings[1] = binding;
        bindings[2] = binding;
        bindings[3] = binding;

        layout = engine.createDescriptorLayout(bindings);

        coffee::DescriptorWriter writer = coffee::DescriptorWriter { layout };

        for (size_t i = 0; i < meshes.size(); i++) {
            coffee::BufferConfiguration bufferConfiguration {};
            bufferConfiguration.usage = coffee::BufferUsage::Uniform;
            bufferConfiguration.properties = coffee::MemoryProperty::HostVisible;
            bufferConfiguration.instanceCount = 1U;
            bufferConfiguration.instanceSize = static_cast<uint32_t>(sizeof(MeshInformation));
            meshesInformationBuffers.push_back(engine.createBuffer(bufferConfiguration));

            const auto& meshMaterials = meshes[i]->materials;
            auto& currentMeshInfo = meshesInformation[i];

            const auto& diffuse = meshMaterials.read(coffee::TextureType::Diffuse);
            const auto& specular = meshMaterials.read(coffee::TextureType::Specular);
            const auto& normals = meshMaterials.read(coffee::TextureType::Normals);

            auto checkAndSet = [this, &meshMaterials, &currentMeshInfo](const coffee::Texture& texture, TextureFlags flag) {
                if (texture != meshMaterials.defaultTexture) {
                    currentMeshInfo.setTextureFlag(flag);
                }
            };

            auto copyVector = [](glm::vec4& dst, const coffee::Vec3& src) {
                dst.r = src.r;
                dst.g = src.g;
                dst.b = src.b;
            };

            checkAndSet(diffuse, TextureFlags::Diffuse);
            checkAndSet(specular, TextureFlags::Specular);
            checkAndSet(normals, TextureFlags::Normals);

            copyVector(currentMeshInfo.diffuseColor, meshMaterials.modifiers.diffuseColor);
            copyVector(currentMeshInfo.specularColor, meshMaterials.modifiers.specularColor);

            currentMeshInfo.ambientColor.r = meshMaterials.modifiers.ambientColor.r;
            currentMeshInfo.ambientColor.g = meshMaterials.modifiers.ambientColor.g;
            currentMeshInfo.ambientColor.b = meshMaterials.modifiers.ambientColor.b;
            currentMeshInfo.ambientColor.a = meshMaterials.modifiers.ambientColor.a;

            writer.addBuffer(0, meshesInformationBuffers[i]);
            writer.addTexture(1, coffee::ResourceState::ShaderResource, diffuse, textureSampler);
            writer.addTexture(2, coffee::ResourceState::ShaderResource, specular, textureSampler);
            writer.addTexture(3, coffee::ResourceState::ShaderResource, normals, textureSampler);

            descriptors.push_back(engine.createDescriptorSet(writer));
        }

        updateMeshesInformation();
    }

    void Model::updateMeshesInformation() {
        for (size_t i = 0; i < meshesInformationBuffers.size(); i++) {
            const auto& buffer = meshesInformationBuffers[i];
            buffer->write(meshesInformation[i]);
        }
    }

}