#ifndef YORNA_EDITOR_ENTITIES
#define YORNA_EDITOR_ENTITIES

#include <entt.hpp>

namespace yorna {

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
