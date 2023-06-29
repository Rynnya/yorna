#ifndef YORNA_FORWARD_PLUS
#define YORNA_FORWARD_PLUS

#include <coffee/graphics/buffer.hpp>
#include <coffee/graphics/image.hpp>

namespace yorna {

    class ForwardPlus {
    public:
        static constexpr uint32_t kFrustumBlockSize = 16;

        ForwardPlus(const coffee::graphics::DevicePtr& device);
        ~ForwardPlus() noexcept;

    private:
        coffee::graphics::BufferPtr gridFrustums = nullptr;
    };

} // namespace yorna

#endif