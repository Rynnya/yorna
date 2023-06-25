#ifndef YORNA_EDITOR_TIMESTAMPS
#define YORNA_EDITOR_TIMESTAMPS

#include <coffee/graphics/command_buffer.hpp>

namespace yorna {

    class QueryTimestamps {
    public:
        static constexpr size_t kIndicesPerTimestamp = 2ULL;

        QueryTimestamps(const coffee::graphics::DevicePtr& device, uint32_t amountOfTimestamps);
        ~QueryTimestamps() noexcept;

        void resetQueryPool(const coffee::graphics::CommandBuffer& commandBuffer);
        void writeBeginTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index);
        void writeEndTimestamp(const coffee::graphics::CommandBuffer& commandBuffer, uint32_t index);

        std::vector<float> extractResults();
        float extractResults(uint32_t index);

    private:
        coffee::graphics::DevicePtr device_;

        VkQueryPool pool_ = VK_NULL_HANDLE;
        uint32_t amountOfTimestamps_ {};
        std::vector<uint32_t> writtenBeginIndices_ {};
        std::vector<uint32_t> writtenEndIndices_ {};
    };

} // namespace yorna

#endif