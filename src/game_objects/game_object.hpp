#ifndef GAME_OBJECT
#define GAME_OBJECT

#include "../systems/model.hpp"

#include <coffee/utils/non_copyable.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace game {

    struct TransformComponent {
        glm::vec3 translation {};
        glm::vec3 scale { 1.0f, 1.0f, 1.0f };
        glm::vec3 rotation;

        glm::mat4 mat4() const noexcept;
        glm::mat3 normal() const noexcept;
    };

    // Base class to every game object
    // In a good way your game must not convert GameObject into something different and vise versa
    class GameObject : coffee::NonCopyable {
    public:
        
        static GameObject createGameObject() noexcept;
        static GameObject createEmptyObject() noexcept;

        GameObject(GameObject&& other) noexcept;
        GameObject& operator=(GameObject&& other) noexcept;

        std::size_t getId() const noexcept;
        bool isValidObject() const noexcept;

        TransformComponent transform {};

    private:
        GameObject(std::size_t objectID) noexcept;

        std::size_t id_ = 0;
    };

}

#endif