#include <entities/model.hpp>

#include <glm/glm.hpp>

namespace editor {

    Model::Model(const coffee::graphics::DevicePtr& device, const coffee::ModelPtr& model, const coffee::graphics::SamplerPtr& textureSampler)
        : device { device }
        , model { model }
    {
        initialize(textureSampler);
    }

    void Model::updateMeshesInformation() {
        for (size_t i = 0; i < meshesInformationBuffers.size(); i++) {
            const auto& buffer = meshesInformationBuffers[i];

            buffer->map();
            std::memcpy(buffer->memory(), &meshesInformation[i], sizeof(MeshInformation));
            buffer->flush();
            buffer->unmap();
        }
    }

    void Model::initialize(const coffee::graphics::SamplerPtr& textureSampler) {
        const auto& meshes = model->meshes;
        meshesInformation.resize(meshes.size());

        std::map<uint32_t, coffee::graphics::DescriptorBindingInfo> bindings {};
        coffee::graphics::DescriptorBindingInfo binding {};

        binding.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[0] = binding;

        binding.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.shaderStages = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[1] = binding;
        bindings[2] = binding;
        bindings[3] = binding;
        bindings[4] = binding;
        bindings[5] = binding;

        layout = coffee::graphics::DescriptorLayout::create(device, bindings);

        coffee::graphics::DescriptorWriter writer = coffee::graphics::DescriptorWriter { layout };

        for (size_t i = 0; i < meshes.size(); i++) {
            coffee::graphics::BufferConfiguration bufferConfiguration {};
            bufferConfiguration.instanceSize = static_cast<uint32_t>(sizeof(MeshInformation));
            bufferConfiguration.instanceCount = 1U;
            bufferConfiguration.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            bufferConfiguration.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            bufferConfiguration.allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            meshesInformationBuffers.push_back(coffee::graphics::Buffer::create(device, bufferConfiguration));

            auto& meshMaterials = meshes[i].materials;
            auto& currentMeshInfo = meshesInformation[i];

            currentMeshInfo.diffuseColor = meshMaterials.modifiers.diffuseColor;
            currentMeshInfo.specularColor = meshMaterials.modifiers.specularColor;
            currentMeshInfo.metallicFactor = meshMaterials.modifiers.metallicFactor;
            currentMeshInfo.roughnessFactor = meshMaterials.modifiers.roughnessFactor;
            currentMeshInfo.textureFlags = meshMaterials.textureFlags();

            writer.addBuffer(0, meshesInformationBuffers[i]);
            writer.addImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Diffuse), textureSampler);
            writer.addImage(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Specular), textureSampler);
            writer.addImage(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Normals), textureSampler);
            writer.addImage(4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Metallic), textureSampler);
            writer.addImage(5, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Roughness), textureSampler);

            descriptors.push_back(coffee::graphics::DescriptorSet::create(device, writer));
        }

        updateMeshesInformation();
    }

}