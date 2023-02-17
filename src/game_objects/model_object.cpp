#include "model_object.hpp"

namespace game {

    ModelObject::ModelObject() noexcept : GameObject { createEmptyObject() } {};

    ModelObject ModelObject::createModelObject(const std::shared_ptr<Model>& model) {
        return ModelObject { model };
    }

    ModelObject::ModelObject(ModelObject&& other) noexcept
        : GameObject { std::move(other) }
        , model_ { std::move(other.model_) }
    {}

    ModelObject& ModelObject::operator=(ModelObject&& other) noexcept {
        this->model_ = std::move(other.model_);
        GameObject::operator=(std::move(other));

        return *this;
    }

    const std::shared_ptr<Model>& ModelObject::getModel() const noexcept {
        return model_;
    }

    ModelObject::ModelObject(const std::shared_ptr<Model>& model) : GameObject { createGameObject() }, model_ { model } {}
}