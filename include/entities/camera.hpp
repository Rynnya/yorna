#ifndef ENTITIES_CAMERA
#define ENTITIES_CAMERA

#include <glm/glm.hpp>

namespace editor {

    class Camera {
    public:
        void setOrthographicProjection(float left = -1.0f, float right = 1.0f, float top = -1.0f, float bottom = 1.0f, float near = -1.0f, float far = 1.0f);
        void setPerspectiveProjection(float fovy, float aspect, float near = 0.1f, float far = 1000.0f);
        void setReversePerspectiveProjection(float fovy, float aspect, float near = 0.1f);

        void setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

        const glm::mat4& projection() const noexcept;
        const glm::mat4& view() const noexcept;
        const glm::mat4& inverseView() const noexcept;

    private:
        glm::mat4 projectionMatrix_ { 1.0f };
        glm::mat4 viewMatrix_ { 1.0f };
        glm::mat4 inverseViewMatrix_ { 1.0f };
    };

}

#endif