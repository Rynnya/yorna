#ifndef YORNA_COMPONENTS
#define YORNA_COMPONENTS

#include <entities/model.hpp>
#include <yorna/shared_instance.hpp>

#include <tbb/queuing_mutex.h>
#include <tbb/task_group.h>

#include <unordered_map>

namespace yorna {

    class ModelLoader : SharedInstance {
    public:
        struct Information {
            std::string meshPath {};
            TransformComponent transform {};
            coffee::graphics::SamplerPtr textureSampler = nullptr;
        };

        ModelLoader(const SharedInstance& instance);
        ~ModelLoader();

        // Submits new loading as asynchronous task
        void submit(const Information& loadingInfo);
        // Extracts all already loaded models in thread-safe manner
        std::vector<ModelPtr> extract() noexcept;

    private:
        tbb::queuing_mutex modelsMutex_ {};

        tbb::task_group tasks_ {};
        std::unordered_map<coffee::graphics::MeshPtr, ModelPtr> cache_ {};
        std::vector<ModelPtr> loadedModels_ {};
    };

} // namespace yorna

#endif
