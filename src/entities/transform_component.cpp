#include <entities/transform_component.hpp>

#include <glm/gtc/epsilon.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <utility>

namespace yorna {

    void TransformComponent::decompose(glm::mat4 transformationMatrix) noexcept
    {
        constexpr float kZeroFloat = 0.0f;
        constexpr float kOneFloat = 1.0f;

        if (glm::epsilonEqual(transformationMatrix[3][3], kZeroFloat, glm::epsilon<float>())) {
            return;
        }

        const bool firstRowNonEmpty = glm::epsilonNotEqual(transformationMatrix[0][3], kZeroFloat, glm::epsilon<float>());
        const bool secondRowNonEmpty = glm::epsilonNotEqual(transformationMatrix[1][3], kZeroFloat, glm::epsilon<float>());
        const bool thirdRowNonEmpty = glm::epsilonNotEqual(transformationMatrix[2][3], kZeroFloat, glm::epsilon<float>());

        if (firstRowNonEmpty || secondRowNonEmpty || thirdRowNonEmpty) {
            transformationMatrix[0][3] = kZeroFloat;
            transformationMatrix[1][3] = kZeroFloat;
            transformationMatrix[2][3] = kZeroFloat;
            transformationMatrix[3][3] = kOneFloat;
        }

        translation = glm::vec3 { transformationMatrix[3] };
        transformationMatrix[3] = glm::vec4 { kZeroFloat, kZeroFloat, kZeroFloat, transformationMatrix[3].w };

        glm::vec3 row[3] {};

        // Now get scale and shear.
        for (uint32_t i = 0; i < 3; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                row[i][j] = transformationMatrix[i][j];
            }

            scale[i] = glm::length(row[i]);
            row[i] = row[i] * kOneFloat / glm::length(row[i]);
        }

        glm::mat3 tRow { row[0], row[1], row[2] };
        rotation.x = atan2(-tRow[2][1], glm::sqrt(kOneFloat - tRow[2][1] * tRow[2][1]));
        rotation.y = atan2(tRow[2][0], tRow[2][2]);
        rotation.z = atan2(tRow[0][1], tRow[1][1]);
    }

    glm::mat4 TransformComponent::mat4() const noexcept
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);

        return glm::mat4 {
            scale.x * (c1 * c3 + s1 * s2 * s3),
            scale.x * (c2 * s3),
            scale.x * (c1 * s2 * s3 - c3 * s1),
            0.0f,
            scale.y * (c3 * s1 * s2 - c1 * s3),
            scale.y * (c2 * c3),
            scale.y * (c1 * c3 * s2 + s1 * s3),
            0.0f,
            scale.z * (c2 * s1),
            scale.z * (-s2),
            scale.z * (c1 * c2),
            0.0f,
            translation.x,
            translation.y,
            translation.z,
            1.0f
        };
    }

    glm::mat3 TransformComponent::normal() const noexcept
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3 {
            invScale.x * (c1 * c3 + s1 * s2 * s3),
            invScale.x * (c2 * s3),
            invScale.x * (c1 * s2 * s3 - c3 * s1),
            invScale.y * (c3 * s1 * s2 - c1 * s3),
            invScale.y * (c2 * c3),
            invScale.y * (c1 * c3 * s2 + s1 * s3),
            invScale.z * (c2 * s1),
            invScale.z * (-s2),
            invScale.z * (c1 * c2),
        };
    }

} // namespace yorna