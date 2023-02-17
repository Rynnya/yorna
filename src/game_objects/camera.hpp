#ifndef GAME_CAMERA_OBJECT
#define GAME_CAMERA_OBJECT

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace game {

    class Camera {
    public:
        void setOrthographicProjection(float left = -1.0f, float right = 1.0f, float top = -1.0f, float bottom = 1.0f, float near = -1.0f, float far = 1.0f);
        void setPerspectiveProjection(float fovy, float aspect, float near = 0.1f, float far = 10.0f);

        void setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3 { 0.0f, -1.0f, 0.0f });
        void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

        const glm::mat4& getProjection() const noexcept;
        const glm::mat4& getView() const noexcept;
        const glm::mat4& getInverseView() const noexcept;

    private:
        glm::mat4 projectionMatrix_ { 1.0f };
        glm::mat4 viewMatrix_ { 1.0f };
        glm::mat4 inverseViewMatrix_ { 1.0f };
    };

}

#endif