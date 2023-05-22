#ifndef SYSTEMS_IMGUI_ENTITIES
#define SYSTEMS_IMGUI_ENTITIES

#include <entt.hpp>

namespace editor {

    // Dummy structure to indicate that this entity is in root
    struct EntityRoot {};

    struct HierarchyComponent {
        entt::entity parent = entt::null;
        entt::entity next = entt::null;
        entt::entity previous = entt::null;
        entt::entity firstChild = entt::null;

        std::string tag {};
    };

}

#endif
