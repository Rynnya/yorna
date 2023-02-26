#include "camera.hpp"

#include <coffee/utils/log.hpp>

namespace game {

    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix_ = glm::mat4 { 1.0f };
        projectionMatrix_[0][0] = 2.0f / (right - left);
        projectionMatrix_[1][1] = -2.0f / (bottom - top);
        projectionMatrix_[2][2] = 1.0f / (far - near);
        projectionMatrix_[3][0] = -(right + left) / (right - left);
        projectionMatrix_[3][1] = -(bottom + top) / (bottom - top);
        projectionMatrix_[3][2] = -near / (far - near);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        if (glm::abs(aspect - std::numeric_limits<float>::epsilon()) <= 0.0f) {
            return;
        }

        const float tanHalfFovy = tan(fovy / 2.0f);
        projectionMatrix_ = glm::mat4 { 0.0f };
        projectionMatrix_[0][0] = 1.0f / (aspect * tanHalfFovy);
        projectionMatrix_[1][1] = -1.0f / (tanHalfFovy);
        projectionMatrix_[2][2] = far / (far - near);
        projectionMatrix_[2][3] = 1.0f;
        projectionMatrix_[3][2] = -(far * near) / (far - near);
    }

    void Camera::setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
        const glm::vec3 w { glm::normalize(direction) };
        const glm::vec3 u { glm::normalize(glm::cross(w, up)) };
        const glm::vec3 v { glm::cross(w, u) };

        viewMatrix_ = glm::mat4 { 1.0f };
        viewMatrix_[0][0] = u.x;
        viewMatrix_[1][0] = u.y;
        viewMatrix_[2][0] = u.z;
        viewMatrix_[0][1] = v.x;
        viewMatrix_[1][1] = v.y;
        viewMatrix_[2][1] = v.z;
        viewMatrix_[0][2] = w.x;
        viewMatrix_[1][2] = w.y;
        viewMatrix_[2][2] = w.z;
        viewMatrix_[3][0] = -glm::dot(u, position);
        viewMatrix_[3][1] = -glm::dot(v, position);
        viewMatrix_[3][2] = -glm::dot(w, position);

        inverseViewMatrix_ = glm::mat4 { 1.f };
        inverseViewMatrix_[0][0] = u.x;
        inverseViewMatrix_[0][1] = u.y;
        inverseViewMatrix_[0][2] = u.z;
        inverseViewMatrix_[1][0] = v.x;
        inverseViewMatrix_[1][1] = v.y;
        inverseViewMatrix_[1][2] = v.z;
        inverseViewMatrix_[2][0] = w.x;
        inverseViewMatrix_[2][1] = w.y;
        inverseViewMatrix_[2][2] = w.z;
        inverseViewMatrix_[3][0] = position.x;
        inverseViewMatrix_[3][1] = position.y;
        inverseViewMatrix_[3][2] = position.z;
    }

    void Camera::setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
        COFFEE_ASSERT(
            glm::distance(target, position) - std::numeric_limits<float>::epsilon() > 0, 
            "Distance between target and position must be more than 0."
        );

        setViewDirection(position, target - position, up);
    }

    void Camera::setViewYXZ(const glm::vec3& position, const glm::vec3& rotation) {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u { (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
        const glm::vec3 v { (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
        const glm::vec3 w { (c2 * s1), (-s2), (c1 * c2) };

        viewMatrix_ = glm::mat4 { 1.0f };
        viewMatrix_[0][0] = u.x;
        viewMatrix_[1][0] = u.y;
        viewMatrix_[2][0] = u.z;
        viewMatrix_[0][1] = v.x;
        viewMatrix_[1][1] = v.y;
        viewMatrix_[2][1] = v.z;
        viewMatrix_[0][2] = w.x;
        viewMatrix_[1][2] = w.y;
        viewMatrix_[2][2] = w.z;
        viewMatrix_[3][0] = -glm::dot(u, position);
        viewMatrix_[3][1] = -glm::dot(v, position);
        viewMatrix_[3][2] = -glm::dot(w, position);

        inverseViewMatrix_ = glm::mat4 { 1.f };
        inverseViewMatrix_[0][0] = u.x;
        inverseViewMatrix_[0][1] = u.y;
        inverseViewMatrix_[0][2] = u.z;
        inverseViewMatrix_[1][0] = v.x;
        inverseViewMatrix_[1][1] = v.y;
        inverseViewMatrix_[1][2] = v.z;
        inverseViewMatrix_[2][0] = w.x;
        inverseViewMatrix_[2][1] = w.y;
        inverseViewMatrix_[2][2] = w.z;
        inverseViewMatrix_[3][0] = position.x;
        inverseViewMatrix_[3][1] = position.y;
        inverseViewMatrix_[3][2] = position.z;
    }

    const glm::mat4& Camera::getProjection() const noexcept {
        return projectionMatrix_;
    }

    const glm::mat4& Camera::getView() const noexcept {
        return viewMatrix_;
    }

    const glm::mat4& Camera::getInverseView() const noexcept {
        return inverseViewMatrix_;
    }

}