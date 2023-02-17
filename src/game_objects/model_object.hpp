#ifndef GAME_MODEL_OBJECT
#define GAME_MODEL_OBJECT

#include "game_object.hpp"
#include "../systems/model.hpp"

namespace game {

    class ModelObject : public GameObject {
        friend class MainSystem;
    public:

        // Empty model object
        ModelObject() noexcept;

        static ModelObject createModelObject(const std::shared_ptr<Model>& model);

        ModelObject(ModelObject&& other) noexcept;
        ModelObject& operator=(ModelObject&& other) noexcept;

        const std::shared_ptr<Model>& getModel() const noexcept;

    private:
        ModelObject(const std::shared_ptr<Model>& model);

        std::shared_ptr<Model> model_ = nullptr;
    };

}

#endif