#include <entities/model.hpp>

#include <glm/glm.hpp>

namespace yorna {

    // clang-format off

    Model::Model(const coffee::graphics::DevicePtr& device, const coffee::graphics::MeshPtr& mesh, const coffee::graphics::SamplerPtr& textureSampler)
        : mesh { mesh }
        , device { device }
    {
        const auto& subMeshes = mesh->subMeshes;
        subMeshesInformation.resize(subMeshes.size());

        layout = coffee::graphics::DescriptorLayout::create(device, {
            { 0, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT          } },
            { 1, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT  } },
            { 2, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT  } },
            { 3, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT  } },
            { 4, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT  } },
            { 5, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT  } },
        });

        coffee::graphics::DescriptorWriter writer = coffee::graphics::DescriptorWriter { layout };

        for (size_t i = 0; i < subMeshes.size(); i++) {
            subMeshesInformationBuffers.push_back(coffee::graphics::Buffer::create(device, {
                .instanceSize = static_cast<uint32_t>(sizeof(SubMeshInformation)),
                .instanceCount = 1U,
                .usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                .allocationFlags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            }));

            auto& meshMaterials = subMeshes[i].materials;
            auto& currentMeshInfo = subMeshesInformation[i];

            currentMeshInfo.diffuseColor = meshMaterials.modifiers.diffuseColor;
            currentMeshInfo.specularColor = meshMaterials.modifiers.specularColor;
            currentMeshInfo.metallicFactor = meshMaterials.modifiers.metallicFactor;
            currentMeshInfo.roughnessFactor = meshMaterials.modifiers.roughnessFactor;
            currentMeshInfo.textureFlags = meshMaterials.textureFlags();

            writer.addBuffer(0, subMeshesInformationBuffers[i]);
            writer.addImage(1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Diffuse), textureSampler);
            writer.addImage(2, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Specular), textureSampler);
            writer.addImage(3, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Normals), textureSampler);
            writer.addImage(4, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Metallic), textureSampler);
            writer.addImage(5, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, meshMaterials.read(coffee::TextureType::Roughness), textureSampler);

            descriptors.push_back(coffee::graphics::DescriptorSet::create(device, writer));
        }

        updateMeshesInformation();
    }

    // clang-format on

    void Model::updateMeshesInformation()
    {
        for (size_t i = 0; i < subMeshesInformationBuffers.size(); i++) {
            std::memcpy(subMeshesInformationBuffers[i]->memory(), &subMeshesInformation[i], sizeof(SubMeshInformation));
        }
    }

} // namespace yorna