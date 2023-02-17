#include "model.hpp"

#include "main_behaviour.hpp"

#include <coffee/abstract/buffer.hpp>
#include <coffee/utils/log.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#undef TINYOBJLOADER_USE_DOUBLE
#include <tiny_obj_loader.h>

#include <unordered_map>

namespace game {

    std::vector<coffee::InputElement> Model::Vertex::getElementDescriptions() {
        std::vector<coffee::InputElement> elements {};

        elements.push_back({ "", 0U, 0U, coffee::Format::R32G32B32SFloat, offsetof(Vertex, position) });
        elements.push_back({ "", 0U, 1U, coffee::Format::R32G32B32SFloat, offsetof(Vertex, color) });
        elements.push_back({ "", 0U, 2U, coffee::Format::R32G32B32SFloat, offsetof(Vertex, normal) });
        elements.push_back({ "", 0U, 3U, coffee::Format::R32G32SFloat, offsetof(Vertex, uv) });

        return elements;
    }

    bool Model::Vertex::operator==(const Vertex& other) const noexcept {
        return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
    }

    Model::Model(coffee::Engine& engine, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : engine_ { engine } {
        createVertexBuffers(vertices);
        createIndexBuffers(indices);

        COFFEE_INFO("Successfully initialized model: {} vertices, {} indices", vertices.size(), indices.size());
    }

    Model::Model(coffee::Engine& engine, const std::string& filePath) : engine_ { engine } {
        tinyobj::attrib_t attrib {};
        std::vector<tinyobj::shape_t> shapes {};
        std::vector<tinyobj::material_t> materials {};
        std::string warn {}, err {};

        COFFEE_THROW_IF(
            !tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str()),
            "Unable to load object file with warning: '{}' and error: '{}'!", warn, err);

        std::vector<Vertex> vertices {};
        std::vector<uint32_t> indices {};
        std::unordered_map<Vertex, uint32_t> uniqueVertices {};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex {};

                if (index.vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };
                }

                if (index.texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                
                indices.push_back(uniqueVertices[vertex]);
            }
        }

        createVertexBuffers(vertices);
        createIndexBuffers(indices);

        COFFEE_INFO("Successfully initialized model: {} vertices, {} indices", vertices.size(), indices.size());
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
        COFFEE_ASSERT(vertices.size() >= 3, "Vertex count must be at least 3.");

        vertexCount = static_cast<uint32_t>(vertices.size());

        coffee::BufferConfiguration stagingConfiguration {};
        stagingConfiguration.usage = coffee::BufferUsage::TransferSource;
        stagingConfiguration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
        stagingConfiguration.instanceCount = vertexCount;
        stagingConfiguration.instanceSize = sizeof(vertices[0]);
        coffee::Buffer stagingBuffer = engine_.createBuffer(stagingConfiguration);

        std::memcpy(stagingBuffer->map(), vertices.data(), vertices.size() * sizeof(vertices[0]));

        coffee::BufferConfiguration vertexConfiguration {};
        vertexConfiguration.usage = coffee::BufferUsage::Vertex | coffee::BufferUsage::TransferDestination;
        vertexConfiguration.properties = coffee::MemoryProperty::DeviceLocal;
        vertexConfiguration.instanceCount = vertexCount;
        vertexConfiguration.instanceSize = sizeof(vertices[0]);
        vertexBuffer = engine_.createBuffer(vertexConfiguration);

        engine_.copyBuffer(vertexBuffer, stagingBuffer);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());

        if (indexCount == 0) {
            return;
        }

        coffee::BufferConfiguration stagingConfiguration {};
        stagingConfiguration.usage = coffee::BufferUsage::TransferSource;
        stagingConfiguration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
        stagingConfiguration.instanceCount = indexCount;
        stagingConfiguration.instanceSize = sizeof(indices[0]);
        coffee::Buffer stagingBuffer = engine_.createBuffer(stagingConfiguration);

        std::memcpy(stagingBuffer->map(), indices.data(), indices.size() * sizeof(indices[0]));

        coffee::BufferConfiguration indicesConfiguration {};
        indicesConfiguration.usage = coffee::BufferUsage::Index | coffee::BufferUsage::TransferDestination;
        indicesConfiguration.properties = coffee::MemoryProperty::DeviceLocal;
        indicesConfiguration.instanceCount = indexCount;
        indicesConfiguration.instanceSize = sizeof(indices[0]);
        indexBuffer = engine_.createBuffer(indicesConfiguration);

        engine_.copyBuffer(indexBuffer, stagingBuffer);
    }

}