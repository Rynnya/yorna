#ifndef YORNA_EDITOR_ENTITIES
#define YORNA_EDITOR_ENTITIES

#include <entt.hpp>

namespace yorna {

    enum class HierarchyType : uint32_t {
        Folder = 0,
        Model = 1,
        PointLight = 2,
        SpotLight = 3
    };

    // Dummy structure to indicate that this entity is in root
    struct EntityRoot {};

    struct HierarchyComponent {
        entt::entity parent = entt::null;
        entt::entity next = entt::null;
        entt::entity previous = entt::null;
        entt::entity firstChild = entt::null;

        HierarchyType type {};
        std::string tag {};
    };

    struct PointLightIndex {
        uint32_t index = 0;
    };

    struct SpotLightIndex {
        uint32_t index = 0;
    };

} // namespace yorna

#endif
