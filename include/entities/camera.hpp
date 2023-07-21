#ifndef YORNA_ENTITIES_CAMERA
#define YORNA_ENTITIES_CAMERA

#include <coffee/objects/aabb.hpp>

#include <glm/glm.hpp>

// Windows again doing it dirty hacks ;-;
#undef near
#undef far

namespace yorna {

    class Camera {
    public:
        static constexpr size_t kAmountOfPlanes = 6;

        void setOrthographicProjection(
            float left = -1.0f,
            float right = 1.0f,
            float top = -1.0f,
            float bottom = 1.0f,
            float near = -1.0f,
            float far = 1.0f
        );
        void setPerspectiveProjection(float fovy, float aspect, float near = 0.1f, float far = 1000.0f);
        void setReversePerspectiveProjection(float fovy, float aspect, float near = 0.1f);

        void setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

        void recalculateFrustumPlanes() noexcept;
        bool isInFrustum(const glm::mat4& modelMatrix, const coffee::AABB& aabb) const noexcept;

        const glm::mat4& projection() const noexcept;
        const glm::mat4& view() const noexcept;
        const glm::mat4& inverseView() const noexcept;

        glm::mat4 computeInverseProjection() const noexcept;

    private:
        glm::mat4 viewProjection_ {};
        glm::vec4 frustumPlanes_[kAmountOfPlanes] {};

        glm::mat4 projectionMatrix_ { 1.0f };
        glm::mat4 viewMatrix_ { 1.0f };
        glm::mat4 inverseViewMatrix_ { 1.0f };
    };

} // namespace yorna

#endif