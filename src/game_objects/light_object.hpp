#ifndef GAME_LIGHT_OBJECT
#define GAME_LIGHT_OBJECT

#include "game_object.hpp"

namespace game {

    class LightObject : public GameObject {
        friend class Behaviour;
    public:

        // Empty light object
        LightObject() noexcept;

        static LightObject createLightObject(float intensity = 3.0f, float radius = 0.1f, glm::vec3 color = glm::vec3 { 1.0f }) noexcept;

        LightObject(LightObject&& other) noexcept;
        LightObject& operator=(LightObject&& other) noexcept;

        float intensity {};
        glm::vec3 color {};

    private:
        LightObject(float intensity, float radius, glm::vec3 color) noexcept;
    };

}

#endif