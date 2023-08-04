#include <editor/components.hpp>

namespace yorna {

    ModelLoader::ModelLoader(const SharedInstance& instance) : SharedInstance { instance } {}

    ModelLoader::~ModelLoader() { tasks_.wait(); }

    void ModelLoader::submit(const Information& loadingInfo)
    {
        tasks_.run([this, loadingInfo]() {
            coffee::MeshLoadingInfo meshLoadingInfo {
                .filesystem = filesystem,
                .path = loadingInfo.meshPath,
            };

            // TBB specification states that throwing exceptions inside task_group::run is undefined behaviour
            if (filesystem->contains(loadingInfo.meshPath)) {
                coffee::graphics::MeshPtr mesh = assetManager->loadMesh(meshLoadingInfo);

                tbb::queuing_mutex::scoped_lock lock { modelsMutex_ };

                auto it = cache_.find(mesh);
                ModelPtr model = it == cache_.end() ? (cache_[mesh] = std::make_shared<Model>(device, mesh, loadingInfo.textureSampler)) : it->second;
                loadedModels_.push_back(std::move(model));

                return;
            }

            COFFEE_WARNING("Mesh '{}' doesn't exist in this filesystem.", loadingInfo.meshPath);
        });
    }

    std::vector<yorna::ModelPtr> ModelLoader::extract() noexcept
    {
        tbb::queuing_mutex::scoped_lock lock { modelsMutex_ };

        return std::exchange(loadedModels_, {});
    }

} // namespace yorna