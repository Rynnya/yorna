#include <editor/timestamps.hpp>

#include <coffee/graphics/command_buffer.hpp>
#include <coffee/utils/log.hpp>

namespace yorna {

    constexpr float timestampToMillseconds = 1000000.0f;

    QueryTimestamps::QueryTimestamps(const coffee::graphics::DevicePtr& device, uint32_t amountOfTimestamps) : device_ { device }
    {
        COFFEE_ASSERT(device_ != nullptr, "Invalid device provided.");

        VkQueryPoolCreateInfo createInfo { VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO };
        createInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        createInfo.queryCount = kIndicesPerTimestamp * amountOfTimestamps;

        if (vkCreateQueryPool(device->logicalDevice(), &createInfo, nullptr, &pool_) != VK_SUCCESS) {
            pool_ = VK_NULL_HANDLE;
            return;
        }

        amountOfTimestamps_ = amountOfTimestamps;
    }

    QueryTimestamps::~QueryTimestamps() noexcept
    {
        if (pool_ == VK_NULL_HANDLE) {
            return;
        }

        device_->waitForRelease();

        vkDestroyQueryPool(device_->logicalDevice(), pool_, nullptr);
    }

    void QueryTimestamps::resetQueryPool(const coffee::graphics::CommandBuffer& commandBuffer)
    {
        if (pool_ == VK_NULL_HANDLE || !writtenBeginIndices_.empty() || !writtenEndIndices_.empty()) {
            return;
        }

        vkCmdResetQueryPool(commandBuffer, pool_, 0, kIndicesPerTimestamp * amountOfTimestamps_);
    }

    void QueryTimestamps::writeBeginTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index)
    {
        if (pool_ == VK_NULL_HANDLE) {
            return;
        }

        for (auto& beginWrite : writtenBeginIndices_) {
            if (beginWrite == index) {
                return;
            }
        }

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, pool_, kIndicesPerTimestamp * index);
        writtenBeginIndices_.push_back(index);
    }

    void QueryTimestamps::writeEndTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index)
    {
        if (pool_ == VK_NULL_HANDLE) {
            return;
        }

        for (auto& endWrite : writtenEndIndices_) {
            if (endWrite == index) {
                return;
            }
        }

        vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, pool_, kIndicesPerTimestamp * index + 1);
        writtenEndIndices_.push_back(index);
    }

    std::vector<float> QueryTimestamps::extractResults()
    {
        std::vector<float> timestampResults {};
        timestampResults.resize(amountOfTimestamps_, 0.0f);

        if (pool_ == VK_NULL_HANDLE) {
            return timestampResults;
        }

        std::vector<uint64_t> queryResults {};
        queryResults.resize(kIndicesPerTimestamp * amountOfTimestamps_, 0);

        VkResult result = vkGetQueryPoolResults(
            device_->logicalDevice(),
            pool_,
            0,
            kIndicesPerTimestamp * amountOfTimestamps_,
            queryResults.size() * sizeof(uint64_t),
            queryResults.data(),
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT
        );

        if (result == VK_SUCCESS) {
            float timestampPeriod = device_->properties().limits.timestampPeriod;

            for (size_t index = 0; index < amountOfTimestamps_; index++) {
                size_t queryIndex = kIndicesPerTimestamp * index;

                timestampResults[index] = (queryResults[queryIndex + 1] - queryResults[queryIndex]) * timestampPeriod / timestampToMillseconds;
            }

            writtenBeginIndices_.clear();
            writtenEndIndices_.clear();
        }

        return timestampResults;
    }

    float QueryTimestamps::extractResults(uint32_t index)
    {
        if (pool_ == VK_NULL_HANDLE) {
            return 0.0f;
        }

        uint64_t queryResults[kIndicesPerTimestamp] {};
        VkResult result = vkGetQueryPoolResults(
            device_->logicalDevice(),
            pool_,
            kIndicesPerTimestamp * index,
            kIndicesPerTimestamp,
            sizeof(queryResults),
            queryResults,
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT
        );

        if (result == VK_SUCCESS) {
            float timestampPeriod = device_->properties().limits.timestampPeriod;

            writtenBeginIndices_.erase(std::remove(writtenBeginIndices_.begin(), writtenBeginIndices_.end(), index));
            writtenEndIndices_.erase(std::remove(writtenEndIndices_.begin(), writtenEndIndices_.end(), index));

            return (queryResults[1] - queryResults[0]) * timestampPeriod / timestampToMillseconds;
        }

        return 0.0f;
    }

} // namespace yorna