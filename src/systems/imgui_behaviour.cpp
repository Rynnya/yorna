#include "imgui_behaviour.hpp"

#include <coffee/engine.hpp>
#include <coffee/utils/log.hpp>

#include <fstream>

#include "structs.hpp"

namespace game {

    static float slider = 60.0f;
    static ImVec4 clearColor = { 0.45f, 0.45f, 0.02f, 1.0f };

    std::vector<coffee::InputElement> getElementDescriptions() {
        std::vector<coffee::InputElement> elements {};

        elements.push_back({ "", 0U, 0U, coffee::Format::R32G32SFloat, offsetof(ImDrawVert, pos) });
        elements.push_back({ "", 0U, 1U, coffee::Format::R32G32SFloat, offsetof(ImDrawVert, uv) });
        elements.push_back({ "", 0U, 2U, coffee::Format::R8G8B8A8UNorm, offsetof(ImDrawVert, col) });

        return elements;
    }

    // Modified ImGui shaders so they will work with Coffee factory

    /*
        #version 450 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aUV;
        layout(location = 2) in vec4 aColor;
        layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

        out gl_PerVertex { vec4 gl_Position; };
        layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

        void main()
        {
            Out.Color = aColor;
            Out.UV = aUV;
            gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
        }
    */

    static std::vector<uint8_t> imguiVertexShader = {
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x08, 0x00, 0x2e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
        0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x6c, 0x6f,
        0x72, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x55, 0x56, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x4f, 0x75, 0x74, 0x00,
        0x05, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x61, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00,
        0x05, 0x00, 0x03, 0x00, 0x15, 0x00, 0x00, 0x00, 0x61, 0x55, 0x56, 0x00, 0x05, 0x00, 0x06, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x67, 0x6c, 0x5f, 0x50, 0x65, 0x72, 0x56, 0x65, 0x72, 0x74, 0x65, 0x78,
        0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x00, 0x05, 0x00, 0x03, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00,
        0x61, 0x50, 0x6f, 0x73, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x75, 0x50, 0x75, 0x73, 0x68, 0x43, 0x6f, 0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x05, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x53, 0x63, 0x61,
        0x6c, 0x65, 0x00, 0x00, 0x06, 0x00, 0x06, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x75, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x70, 0x63, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x15, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x19, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x1c, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x03, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x0a, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x15, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x0e, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x11, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00,
        0x0c, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
        0x17, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x14, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x04, 0x00, 0x1e, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00,
        0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x12, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00,
        0x15, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x17, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x0b, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x85, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x24, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00,
        0x21, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
        0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00,
        0x81, 0x00, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
        0x26, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00,
        0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00,
        0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x11, 0x00, 0x00, 0x00,
        0x2d, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x2d, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
    };

    /*
        #version 450 core
        layout(location = 0) out vec4 fColor;
        layout(set=0, binding=0) uniform sampler2D sTexture;
        layout(location = 0) in struct { vec4 Color; vec2 UV; } In;

        vec4 correctColor(vec4 color)
        {
            color.r = pow(color.r, 2.2);
            color.g = pow(color.g, 2.2);
            color.b = pow(color.b, 2.2);
            return color;
        }

        void main()
        {
            fColor = correctColor(In.Color) * texture(sTexture, In.UV.st);
        }
    */

    static std::vector<uint8_t> imguiFragmentShader = {
        0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x3a, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
        0xc2, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e,
        0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x72, 0x72,
        0x65, 0x63, 0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x28, 0x76, 0x66, 0x34, 0x3b, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x66, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00,
        0x05, 0x00, 0x03, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x05, 0x00,
        0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x55, 0x56, 0x00, 0x00,
        0x05, 0x00, 0x03, 0x00, 0x27, 0x00, 0x00, 0x00, 0x49, 0x6e, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
        0x2a, 0x00, 0x00, 0x00, 0x70, 0x61, 0x72, 0x61, 0x6d, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00,
        0x32, 0x00, 0x00, 0x00, 0x73, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x04, 0x00, 0x27, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x04, 0x00, 0x32, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x04, 0x00, 0x32, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x06, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x17, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x21, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x15, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0xcd, 0xcc, 0x0c, 0x40,
        0x2b, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x3b, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x17, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00,
        0x3b, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x15, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x19, 0x00, 0x09, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x30, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
        0x3b, 0x00, 0x04, 0x00, 0x31, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x2b, 0x00, 0x04, 0x00, 0x28, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x04, 0x00, 0x35, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00,
        0x2b, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00,
        0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00,
        0x3e, 0x00, 0x03, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x39, 0x00, 0x05, 0x00,
        0x07, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00,
        0x3d, 0x00, 0x04, 0x00, 0x30, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
        0x41, 0x00, 0x05, 0x00, 0x35, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00,
        0x34, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00,
        0x36, 0x00, 0x00, 0x00, 0x57, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
        0x33, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x85, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x39, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x23, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00,
        0x36, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x09, 0x00, 0x00, 0x00, 0x37, 0x00, 0x03, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
        0xf8, 0x00, 0x02, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x07, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x11, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x16, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x07, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x17, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x19, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x1b, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x07, 0x00,
        0x06, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00,
        0x1c, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00,
        0x1e, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00,
        0x1f, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x02, 0x00, 0x1f, 0x00, 0x00, 0x00,
        0x38, 0x00, 0x01, 0x00
    };

    ImGuiKey keyToImGui(coffee::Keys key) {
        switch (key) {
            case coffee::Keys::Tab:             return ImGuiKey_Tab;
            case coffee::Keys::Left:            return ImGuiKey_LeftArrow;
            case coffee::Keys::Right:           return ImGuiKey_RightArrow;
            case coffee::Keys::Up:              return ImGuiKey_UpArrow;
            case coffee::Keys::Down:            return ImGuiKey_DownArrow;
            case coffee::Keys::PageUp:          return ImGuiKey_PageUp;
            case coffee::Keys::PageDown:        return ImGuiKey_PageDown;
            case coffee::Keys::Home:            return ImGuiKey_Home;
            case coffee::Keys::End:             return ImGuiKey_End;
            case coffee::Keys::Insert:          return ImGuiKey_Insert;
            case coffee::Keys::Delete:          return ImGuiKey_Delete;
            case coffee::Keys::Backspace:       return ImGuiKey_Backspace;
            case coffee::Keys::Space:           return ImGuiKey_Space;
            case coffee::Keys::Enter:           return ImGuiKey_Enter;
            case coffee::Keys::Escape:          return ImGuiKey_Escape;
            case coffee::Keys::Apostrophe:      return ImGuiKey_Apostrophe;
            case coffee::Keys::Comma:           return ImGuiKey_Comma;
            case coffee::Keys::Minus:           return ImGuiKey_Minus;
            case coffee::Keys::Period:          return ImGuiKey_Period;
            case coffee::Keys::Slash:           return ImGuiKey_Slash;
            case coffee::Keys::Semicolon:       return ImGuiKey_Semicolon;
            case coffee::Keys::Equal:           return ImGuiKey_Equal;
            case coffee::Keys::LeftBracket:     return ImGuiKey_LeftBracket;
            case coffee::Keys::Backslash:       return ImGuiKey_Backslash;
            case coffee::Keys::RightBracket:    return ImGuiKey_RightBracket;
            case coffee::Keys::GraveAccent:     return ImGuiKey_GraveAccent;
            case coffee::Keys::CapsLock:        return ImGuiKey_CapsLock;
            case coffee::Keys::ScrollLock:      return ImGuiKey_ScrollLock;
            case coffee::Keys::NumLock:         return ImGuiKey_NumLock;
            case coffee::Keys::PrintScreen:     return ImGuiKey_PrintScreen;
            case coffee::Keys::Pause:           return ImGuiKey_Pause;
            case coffee::Keys::KP0:             return ImGuiKey_Keypad0;
            case coffee::Keys::KP1:             return ImGuiKey_Keypad1;
            case coffee::Keys::KP2:             return ImGuiKey_Keypad2;
            case coffee::Keys::KP3:             return ImGuiKey_Keypad3;
            case coffee::Keys::KP4:             return ImGuiKey_Keypad4;
            case coffee::Keys::KP5:             return ImGuiKey_Keypad5;
            case coffee::Keys::KP6:             return ImGuiKey_Keypad6;
            case coffee::Keys::KP7:             return ImGuiKey_Keypad7;
            case coffee::Keys::KP8:             return ImGuiKey_Keypad8;
            case coffee::Keys::KP9:             return ImGuiKey_Keypad9;
            case coffee::Keys::KPDecimal:       return ImGuiKey_KeypadDecimal;
            case coffee::Keys::KPDivide:        return ImGuiKey_KeypadDivide;
            case coffee::Keys::KPMultiply:      return ImGuiKey_KeypadMultiply;
            case coffee::Keys::KPSubtract:      return ImGuiKey_KeypadSubtract;
            case coffee::Keys::KPAdd:           return ImGuiKey_KeypadAdd;
            case coffee::Keys::KPEnter:         return ImGuiKey_KeypadEnter;
            case coffee::Keys::KPEqual:         return ImGuiKey_KeypadEqual;
            case coffee::Keys::LeftShift:       return ImGuiKey_LeftShift;
            case coffee::Keys::LeftControl:     return ImGuiKey_LeftCtrl;
            case coffee::Keys::LeftAlt:         return ImGuiKey_LeftAlt;
            case coffee::Keys::LeftSuper:       return ImGuiKey_LeftSuper;
            case coffee::Keys::RightShift:      return ImGuiKey_RightShift;
            case coffee::Keys::RightControl:    return ImGuiKey_RightCtrl;
            case coffee::Keys::RightAlt:        return ImGuiKey_RightAlt;
            case coffee::Keys::RightSuper:      return ImGuiKey_RightSuper;
            case coffee::Keys::Menu:            return ImGuiKey_Menu;
            case coffee::Keys::D0:              return ImGuiKey_0;
            case coffee::Keys::D1:              return ImGuiKey_1;
            case coffee::Keys::D2:              return ImGuiKey_2;
            case coffee::Keys::D3:              return ImGuiKey_3;
            case coffee::Keys::D4:              return ImGuiKey_4;
            case coffee::Keys::D5:              return ImGuiKey_5;
            case coffee::Keys::D6:              return ImGuiKey_6;
            case coffee::Keys::D7:              return ImGuiKey_7;
            case coffee::Keys::D8:              return ImGuiKey_8;
            case coffee::Keys::D9:              return ImGuiKey_9;
            case coffee::Keys::A:               return ImGuiKey_A;
            case coffee::Keys::B:               return ImGuiKey_B;
            case coffee::Keys::C:               return ImGuiKey_C;
            case coffee::Keys::D:               return ImGuiKey_D;
            case coffee::Keys::E:               return ImGuiKey_E;
            case coffee::Keys::F:               return ImGuiKey_F;
            case coffee::Keys::G:               return ImGuiKey_G;
            case coffee::Keys::H:               return ImGuiKey_H;
            case coffee::Keys::I:               return ImGuiKey_I;
            case coffee::Keys::J:               return ImGuiKey_J;
            case coffee::Keys::K:               return ImGuiKey_K;
            case coffee::Keys::L:               return ImGuiKey_L;
            case coffee::Keys::M:               return ImGuiKey_M;
            case coffee::Keys::N:               return ImGuiKey_N;
            case coffee::Keys::O:               return ImGuiKey_O;
            case coffee::Keys::P:               return ImGuiKey_P;
            case coffee::Keys::Q:               return ImGuiKey_Q;
            case coffee::Keys::R:               return ImGuiKey_R;
            case coffee::Keys::S:               return ImGuiKey_S;
            case coffee::Keys::T:               return ImGuiKey_T;
            case coffee::Keys::U:               return ImGuiKey_U;
            case coffee::Keys::V:               return ImGuiKey_V;
            case coffee::Keys::W:               return ImGuiKey_W;
            case coffee::Keys::X:               return ImGuiKey_X;
            case coffee::Keys::Y:               return ImGuiKey_Y;
            case coffee::Keys::Z:               return ImGuiKey_Z;
            case coffee::Keys::F1:              return ImGuiKey_F1;
            case coffee::Keys::F2:              return ImGuiKey_F2;
            case coffee::Keys::F3:              return ImGuiKey_F3;
            case coffee::Keys::F4:              return ImGuiKey_F4;
            case coffee::Keys::F5:              return ImGuiKey_F5;
            case coffee::Keys::F6:              return ImGuiKey_F6;
            case coffee::Keys::F7:              return ImGuiKey_F7;
            case coffee::Keys::F8:              return ImGuiKey_F8;
            case coffee::Keys::F9:              return ImGuiKey_F9;
            case coffee::Keys::F10:             return ImGuiKey_F10;
            case coffee::Keys::F11:             return ImGuiKey_F11;
            case coffee::Keys::F12:             return ImGuiKey_F12;
            default:                            return ImGuiKey_None;
        }
    }

    ImGuiSystem::ImGuiSystem(coffee::Window& window) : mainWindow { window } {
        initializeImGui();
        initializeBackend();
    }

    ImGuiSystem::~ImGuiSystem() {
        coffee::Engine::removeMonitorConnectedCallback("imgui");
        coffee::Engine::removeMonitorDisconnectedCallback("imgui");

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);

        coffee::Engine::waitDeviceIdle();

        // We released this handle before, so we must explicitly delete it
        delete viewportData;
        viewport->PlatformUserData = nullptr;

        delete static_cast<RendererData*>(viewport->RendererUserData);
        viewport->RendererUserData = nullptr;

        delete static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        delete static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        ImGui::DestroyPlatformWindows();
        ImGui::DestroyContext();
    }

    void ImGuiSystem::update() {
        ImGuiIO& io = ImGui::GetIO();

        coffee::Extent2D windowSize = mainWindow->getWindowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };

        if (windowSize.width > 0 && windowSize.height > 0) {
            coffee::Extent2D framebufferSize = mainWindow->getFramebufferSize();

            io.DisplayFramebufferScale = ImVec2 {
                static_cast<float>(windowSize.width / framebufferSize.width),
                static_cast<float>(windowSize.height / framebufferSize.height)
            };
        }

        if (static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors) {
            updateMonitors();
        }

        io.DeltaTime = coffee::Engine::getDeltaTime();

        updateMouse(mainWindow);
        updateCursor(mainWindow);

        prepareImGui(mainWindow);
    }

    void ImGuiSystem::render() {
        ImGuiIO& io = ImGui::GetIO();

        if (mainWindow->acquireNextImage()) {
            coffee::CommandBuffer commandBuffer = coffee::Factory::createCommandBuffer();
            this->render(ImGui::GetMainViewport(), commandBuffer);
            mainWindow->sendCommandBuffer(std::move(commandBuffer));
            mainWindow->submitPendingWork();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiSystem::render(ImGuiViewport* viewport, const coffee::CommandBuffer& commandBuffer) {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
        ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
        RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData);
        ImDrawData* data = viewport->DrawData;

        const uint32_t frameIndex = viewportData->windowHandle->getCurrentImageIndex();
        float framebufferWidth = data->DisplaySize.x * data->FramebufferScale.x;
        float framebufferHeight = data->DisplaySize.y * data->FramebufferScale.y;

        if (framebufferWidth <= 0.0f || framebufferHeight <= 0.0f) {
            return;
        }

        commandBuffer->beginRenderPass(
            backendData->renderPass, 
            rendererData->framebuffers[frameIndex],
            viewportData->windowHandle->getFramebufferSize());

        if (data->TotalVtxCount > 0) {
            size_t vertexSize = data->TotalVtxCount * sizeof(ImDrawVert);
            size_t indexSize = data->TotalIdxCount * sizeof(ImDrawIdx);

            coffee::Buffer& vertexBuffer = rendererData->vertexBuffers[frameIndex];
            coffee::Buffer& indexBuffer = rendererData->indexBuffers[frameIndex];

            if (vertexBuffer == nullptr || vertexBuffer->getTotalSize() < vertexSize) {
                coffee::BufferConfiguration configuration {};
                configuration.usage = coffee::BufferUsage::Vertex;
                configuration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
                configuration.instanceCount = data->TotalVtxCount;
                configuration.instanceSize = sizeof(ImDrawVert);
                vertexBuffer = coffee::Factory::createBuffer(configuration);
            }

            if (indexBuffer == nullptr || indexBuffer->getTotalSize() < indexSize) {
                coffee::BufferConfiguration configuration {};
                configuration.usage = coffee::BufferUsage::Index;
                configuration.properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent;
                configuration.instanceCount = data->TotalIdxCount;
                configuration.instanceSize = sizeof(ImDrawIdx);
                indexBuffer = coffee::Factory::createBuffer(configuration);
            }

            size_t vertexOffset = 0;
            size_t indexOffset = 0;

            for (size_t i = 0; i < data->CmdListsCount; i++) {
                const ImDrawList* list = data->CmdLists[i];

                vertexBuffer->write(list->VtxBuffer.Data, list->VtxBuffer.Size * sizeof(ImDrawVert), vertexOffset);
                indexBuffer->write(list->IdxBuffer.Data, list->IdxBuffer.Size * sizeof(ImDrawIdx), indexOffset);

                vertexOffset += list->VtxBuffer.Size * sizeof(ImDrawVert);
                indexOffset += list->IdxBuffer.Size * sizeof(ImDrawIdx);
            }

            commandBuffer->bindVertexBuffer(vertexBuffer);
            commandBuffer->bindIndexBuffer(indexBuffer);
        }

        commandBuffer->bindPipeline(backendData->pipeline);
        commandBuffer->bindDescriptorSet(backendData->descriptorSet);

        commandBuffer->setViewport({ static_cast<uint32_t>(framebufferWidth), static_cast<uint32_t>(framebufferHeight) });

        ImGuiPushConstant constants {};
        constants.scale.x = 2.0f / data->DisplaySize.x;
        constants.scale.y = 2.0f / data->DisplaySize.y;
        constants.translate.x = -1.0f - data->DisplayPos.x * constants.scale.x;
        constants.translate.y = -1.0f - data->DisplayPos.y * constants.scale.y;
        commandBuffer->pushConstants(coffee::ShaderStage::Vertex, constants);

        ImVec2 clipOff = data->DisplayPos;
        ImVec2 clipScale = data->FramebufferScale;

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        for (int i = 0; i < data->CmdListsCount; i++) {
            const ImDrawList* list = data->CmdLists[i];

            for (int j = 0; j < list->CmdBuffer.Size; j++) {
                const ImDrawCmd& command = list->CmdBuffer[j];

                if (command.UserCallback != nullptr) {
                    command.UserCallback(list, &command);
                    continue;
                }

                ImVec2 clipMin { (command.ClipRect.x - clipOff.x) * clipScale.x, (command.ClipRect.y - clipOff.y) * clipScale.y };
                ImVec2 clipMax { (command.ClipRect.z - clipOff.x) * clipScale.x, (command.ClipRect.w - clipOff.y) * clipScale.y };

                clipMin.x = std::max(clipMin.x, 0.0f);
                clipMin.y = std::max(clipMin.y, 0.0f);

                clipMax.x = std::min(clipMax.x, framebufferWidth);
                clipMax.y = std::min(clipMax.y, framebufferHeight);

                if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y) {
                    continue;
                }

                if (command.TextureId != nullptr) {
                    commandBuffer->bindDescriptorSet(*reinterpret_cast<coffee::DescriptorSet*>(command.TextureId));
                }

                commandBuffer->setScissor(
                    { static_cast<uint32_t>(clipMax.x - clipMin.x), static_cast<uint32_t>(clipMax.y - clipMin.y) },
                    { static_cast<int32_t>(clipMin.x), static_cast<int32_t>(clipMin.y) });
                commandBuffer->drawIndexed(command.ElemCount, 1U, indexOffset + command.IdxOffset, vertexOffset + command.VtxOffset);

                // We must restore default descriptor set
                if (command.TextureId != nullptr) {
                    commandBuffer->bindDescriptorSet(backendData->descriptorSet);
                }
            }

            vertexOffset += list->VtxBuffer.Size;
            indexOffset += list->IdxBuffer.Size;
        }

        commandBuffer->endRenderPass();
    }

    void ImGuiSystem::initializeImGui() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;

        coffee::Extent2D windowSize = mainWindow->getWindowSize();
        io.DisplaySize = ImVec2 { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) };
        io.BackendPlatformUserData = new BackendPlatformData {};

        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        updateMonitors();

        coffee::Engine::addMonitorConnectedCallback("imgui", [](const coffee::MonitorImpl&) {
            static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        });

        coffee::Engine::addMonitorDisconnectedCallback("imgui", [](const coffee::MonitorImpl&) {
            static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData)->wantUpdateMonitors = true;
        });

        // Platform callbacks
        platformIO.Platform_CreateWindow = [](ImGuiViewport* viewport) {
            BackendPlatformData* backendPlatformData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

            ViewportData* viewportData = new ViewportData {
                .windowHandle = coffee::Factory::createWindow({
                    .extent = { static_cast<uint32_t>(viewport->Size.x), static_cast<uint32_t>(viewport->Size.y) },
                    .hiddenOnStart = true,
                    .borderless = (viewport->Flags & ImGuiViewportFlags_NoDecoration) != 0
                }).release() // We must explicitly delete this
            };

            viewport->PlatformUserData = viewportData;

            coffee::AbstractWindow* window = viewportData->windowHandle;
            window->userData = viewport->ID;
            window->setWindowPosition({ static_cast<int32_t>(viewport->Pos.x), static_cast<int32_t>(viewport->Pos.y) });

            window->addWindowFocusCallback("viewport", focusCallback);
            window->addWindowEnterCallback("viewport", enterCallback);
            window->addMouseClickCallback("viewport", mouseClickCallback);
            window->addMousePositionCallback("viewport", mousePositionCallback);
            window->addMouseWheelCallback("viewport", mouseWheelCallback);
            window->addKeyCallback("viewport", keyCallback);
            window->addCharCallback("viewport", charCallback);

            window->addWindowPositionCallback("viewport", [](const coffee::AbstractWindow* const window, const coffee::WindowPositionEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window->userData))) {
                    if (ImGui::GetFrameCount() <= static_cast<ViewportData*>(viewport->PlatformUserData)->ignoreWindowPositionFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestMove = true;
                }
            });

            window->addWindowResizeCallback("viewport", [](const coffee::AbstractWindow* const window, const coffee::ResizeEvent& e) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window->userData))) {
                    BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
                    ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
                    RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData);

                    const auto& presentImages = viewportData->windowHandle->getPresentImages();
                    rendererData->framebuffers.resize(presentImages.size());

                    for (size_t i = 0; i < presentImages.size(); i++) {
                        rendererData->framebuffers[i] = coffee::Factory::createFramebuffer(backendData->renderPass, {
                            .extent = viewportData->windowHandle->getFramebufferSize(),
                            .colorViews = { presentImages[i] }
                        });
                    }

                    if (ImGui::GetFrameCount() <= static_cast<ViewportData*>(viewport->PlatformUserData)->ignoreWindowSizeFrameCount + 1) {
                        return;
                    }

                    viewport->PlatformRequestResize = true;
                }
            });

            window->addWindowCloseCallback("viewport", [](const coffee::AbstractWindow* const window) {
                if (ImGuiViewport* viewport = ImGui::FindViewportByID(std::any_cast<ImGuiID>(window->userData))) {
                    viewport->PlatformRequestClose = true;
                }
            });

            window->showWindow();
        };
        platformIO.Platform_DestroyWindow = [](ImGuiViewport* viewport) {
            if (ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData)) {
                BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);

                for (int32_t i = 0; i < IM_ARRAYSIZE(backendData->keyOwnerWindows); i++) {
                    if (backendData->keyOwnerWindows[i] == viewportData->windowHandle) {
                        keyCallback(viewportData->windowHandle, coffee::KeyEvent { 
                            coffee::State::Release, 
                            static_cast<coffee::Keys>(i),
                            0, 0
                        });
                    }
                }

                // We released this handle before, so we must explicitly delete it
                delete viewportData->windowHandle;
                delete viewportData;
                viewport->PlatformUserData = nullptr;

                delete static_cast<RendererData*>(viewport->RendererUserData);
                viewport->RendererUserData = nullptr;
            }
        };
        platformIO.Platform_ShowWindow = [](ImGuiViewport* viewport) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->showWindow();
        };
        platformIO.Platform_SetWindowPos = [](ImGuiViewport* viewport, ImVec2 position) {
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowPositionFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowPosition({ static_cast<int32_t>(position.x), static_cast<int32_t>(position.y) });
        };
        platformIO.Platform_GetWindowPos = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::AbstractWindow* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            coffee::Offset2D windowPosition = window->getWindowPosition();
            return { static_cast<float>(windowPosition.x), static_cast<float>(windowPosition.y) };
        };
        platformIO.Platform_SetWindowSize = [](ImGuiViewport* viewport, ImVec2 size) {
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
            viewportData->ignoreWindowSizeFrameCount = ImGui::GetFrameCount();
            viewportData->windowHandle->setWindowSize({ static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y) });
        };
        platformIO.Platform_GetWindowSize = [](ImGuiViewport* viewport) -> ImVec2 {
            coffee::AbstractWindow* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            coffee::Extent2D windowSize = window->getWindowSize();
            return { static_cast<float>(windowSize.width), static_cast<float>(windowSize.height) }; 
        };
        platformIO.Platform_SetWindowFocus = [](ImGuiViewport* viewport) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->focusWindow();
        };
        platformIO.Platform_GetWindowFocus = [](ImGuiViewport* viewport) -> bool {
            return static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->isFocused();
        };
        platformIO.Platform_GetWindowMinimized = [](ImGuiViewport* viewport) -> bool {
            bool result = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->isIconified();
            return result;
        };
        platformIO.Platform_SetWindowTitle = [](ImGuiViewport* viewport, const char* str) {
            static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle->setWindowTitle(str);
        };
        platformIO.Platform_RenderWindow = [](ImGuiViewport*, void*) { /* Already handled by Renderer_RenderWindow */ };
        platformIO.Platform_SwapBuffers = [](ImGuiViewport*, void*) { /* Already handled by Renderer_SwapBuffers */ };

        // Renderer callbacks
        platformIO.Platform_CreateVkSurface = [](ImGuiViewport*, ImU64, const void*, ImU64*) -> int {
            /* Already handled by Platform_CreateWindow */
            return 0; // Same as VK_SUCCESS
        };
        platformIO.Renderer_CreateWindow = [](ImGuiViewport* viewport) {
            BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            RendererData* rendererData = static_cast<RendererData*>(viewport->RendererUserData = new RendererData {});

            coffee::AbstractWindow* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;
            const auto& presentImages = window->getPresentImages();
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->framebuffers[i] = coffee::Factory::createFramebuffer(backendData->renderPass, {
                    .extent = window->getFramebufferSize(),
                    .colorViews = { presentImages[i] }
                });
            }

            rendererData->vertexBuffers.resize(presentImages.size());
            rendererData->indexBuffers.resize(presentImages.size());
        };
        platformIO.Renderer_DestroyWindow = [](ImGuiViewport*) { /* Already handled by Platform_DestroyWindow */ };
        platformIO.Renderer_SetWindowSize = [](ImGuiViewport*, ImVec2) { /* Already handled in window callback */ };
        platformIO.Renderer_RenderWindow = [](ImGuiViewport* viewport, void*) {
            ViewportData* viewportData = static_cast<ViewportData*>(viewport->PlatformUserData);
            if (viewportData->windowHandle->acquireNextImage()) {
                coffee::CommandBuffer commandBuffer = coffee::Factory::createCommandBuffer();
                ImGuiSystem::render(viewport, commandBuffer);
                viewportData->windowHandle->sendCommandBuffer(std::move(commandBuffer));
                viewportData->windowHandle->submitPendingWork();
            }
        };
        platformIO.Renderer_SwapBuffers = [](ImGuiViewport* viewport, void*) { /* Already handled in Renderer_RenderWindow */ };

        mainWindow->addWindowFocusCallback("mainViewport", focusCallback);
        mainWindow->addWindowEnterCallback("mainViewport", enterCallback);
        mainWindow->addMouseClickCallback("mainViewport", mouseClickCallback);
        mainWindow->addMousePositionCallback("mainViewport", mousePositionCallback);
        mainWindow->addMouseWheelCallback("mainViewport", mouseWheelCallback);
        mainWindow->addKeyCallback("mainViewport", keyCallback);
        mainWindow->addCharCallback("mainViewport", charCallback);

        mainWindow->addWindowResizeCallback("viewport", [](const coffee::AbstractWindow* const window, const coffee::ResizeEvent& e) {
            BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);
            ViewportData* viewportData = static_cast<ViewportData*>(ImGui::GetMainViewport()->PlatformUserData);
            RendererData* rendererData = static_cast<RendererData*>(ImGui::GetMainViewport()->RendererUserData);

            const auto& presentImages = viewportData->windowHandle->getPresentImages();
            rendererData->framebuffers.resize(presentImages.size());

            for (size_t i = 0; i < presentImages.size(); i++) {
                rendererData->framebuffers[i] = coffee::Factory::createFramebuffer(backendData->renderPass, {
                    .extent = viewportData->windowHandle->getFramebufferSize(),
                    .colorViews = { presentImages[i] }
                });
            }
        });

        mainWindow->userData = 0;

        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformUserData = new ViewportData { .windowHandle = mainWindow.get() };
    }

    void ImGuiSystem::initializeBackend() {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData = new BackendRendererData {});
        RendererData* rendererData = static_cast<RendererData*>(ImGui::GetMainViewport()->RendererUserData = new RendererData {});

        createFonts();
        createDescriptors();
        createRenderPass();
        createPipeline();

        const auto& presentImages = mainWindow->getPresentImages();
        rendererData->framebuffers.resize(presentImages.size());

        for (size_t i = 0; i < presentImages.size(); i++) {
            rendererData->framebuffers[i] = coffee::Factory::createFramebuffer(backendData->renderPass, {
                .extent = mainWindow->getFramebufferSize(),
                .colorViews = { presentImages[i] }
            });
        }

        rendererData->vertexBuffers.resize(presentImages.size());
        rendererData->indexBuffers.resize(presentImages.size());
    }

    void ImGuiSystem::createFonts() {
        ImGuiIO& io = ImGui::GetIO();
        uint8_t* fontPixels = nullptr;
        int width {}, height {};

        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        io.Fonts->GetTexDataAsAlpha8(&fontPixels, &width, &height);

        BackendRendererData* backendData = static_cast<BackendRendererData*>(io.BackendRendererUserData);

        backendData->fontsSampler = coffee::Factory::createSampler({
            .magFilter = coffee::SamplerFilter::Linear,
            .minFilter = coffee::SamplerFilter::Linear,
            .mipmapMode = coffee::SamplerFilter::Linear,
            .anisotropyEnable = true,
            .maxAnisotropy = 1U,
            .borderColor = coffee::BorderColor::OpaqueBlack,
            .minLod = -1000.0f,
            .maxLod = 1000.0f
        });

        backendData->fonts = coffee::Factory::createImage({
            .type = coffee::ImageType::TwoDimensional,
            .format = coffee::Format::R8UNorm,
            .extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
            .tiling = coffee::ImageTiling::Optimal,
            .usage = coffee::ImageUsage::TransferDestination | coffee::ImageUsage::Sampled,
            .initialState = coffee::ResourceState::Undefined,
            .viewType = coffee::ImageViewType::TwoDimensional,
            .swizzleRed = coffee::SwizzleComponent::One,
            .swizzleGreen = coffee::SwizzleComponent::One,
            .swizzleBlue = coffee::SwizzleComponent::One,
            .swizzleAlpha = coffee::SwizzleComponent::Red,
            .aspects = coffee::ImageAspect::Color
        });

        coffee::Buffer staging = coffee::Factory::createBuffer({
            .usage = coffee::BufferUsage::TransferSource,
            .properties = coffee::MemoryProperty::HostVisible | coffee::MemoryProperty::HostCoherent,
            .instanceCount = 1U,
            .instanceSize = static_cast<uint32_t>(width * height)
        });

        staging->write(fontPixels, static_cast<size_t>(width * height));
        coffee::Engine::copyBufferToImage(backendData->fonts, staging);
    }

    void ImGuiSystem::createDescriptors() {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        std::map<uint32_t, coffee::DescriptorBindingInfo> bindings {};
        coffee::DescriptorBindingInfo binding {};

        bindings[0] = {
            .type = coffee::DescriptorType::ImageAndSampler,
            .stages = coffee::ShaderStage::Fragment
        };

        backendData->layout =
            coffee::Factory::createDescriptorLayout(bindings);
        backendData->descriptorSet =
            coffee::Factory::createDescriptorSet(
                coffee::DescriptorWriter(backendData->layout)
                .addImage(0, coffee::ResourceState::ShaderResource, backendData->fonts, backendData->fontsSampler));
    }

    void ImGuiSystem::createRenderPass() {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        backendData->renderPass = coffee::Factory::createRenderPass({
            .colorAttachments = { coffee::AttachmentConfiguration {
                .sampleCount = 1U,
                .initialState = coffee::ResourceState::Undefined,
                .finalState = coffee::ResourceState::Present,
                .format = mainWindow->getColorFormat(), // All windows share same render pass
                .loadOp = coffee::AttachmentLoadOp::Clear,
                .storeOp = coffee::AttachmentStoreOp::Store,
                .stencilLoadOp = coffee::AttachmentLoadOp::DontCare,
                .stencilStoreOp = coffee::AttachmentStoreOp::DontCare,
                .clearValue = std::array<float, 4> { 0.01f, 0.01f, 0.01f, 1.0f }
            }}
        });
    }

    void ImGuiSystem::createPipeline() {
        BackendRendererData* backendData = static_cast<BackendRendererData*>(ImGui::GetIO().BackendRendererUserData);

        std::vector<coffee::Shader> shaders {};
        shaders.push_back(coffee::Factory::createShader(imguiVertexShader, coffee::ShaderStage::Vertex));
        shaders.push_back(coffee::Factory::createShader(imguiFragmentShader, coffee::ShaderStage::Fragment));

        backendData->pipeline = coffee::Factory::createPipeline(backendData->renderPass, { backendData->layout }, shaders, {
            .rasterizationInfo = {
                .frontFace = coffee::FrontFace::CounterClockwise
            },
            .colorBlendAttachment = {
                .blendEnable = true,
                .srcBlend = coffee::BlendFactor::SrcAlpha,
                .dstBlend = coffee::BlendFactor::OneMinusSrcAlpha,
                .blendOp = coffee::BlendOp::Add,
                .alphaSrcBlend = coffee::BlendFactor::One,
                .alphaDstBlend = coffee::BlendFactor::OneMinusSrcAlpha,
                .alphaBlendOp = coffee::BlendOp::Add
            },
            .depthStencilInfo = {
                .depthTestEnable = false,
                .depthWriteEnable = false,
                .depthCompareOp = coffee::CompareOp::Never
            },
            .inputBindings = { coffee::InputBinding { 
                .binding = 0U, 
                .stride = sizeof(ImDrawVert), 
                .inputRate = coffee::InputRate::PerVertex, 
                .elements = getElementDescriptions() 
            }},
            .ranges = { coffee::PushConstantsRange { 
                .shaderStages = coffee::ShaderStage::Vertex, 
                .size = static_cast<uint32_t>(sizeof(ImGuiPushConstant))
            }}
        });
    }

    void ImGuiSystem::focusCallback(const coffee::AbstractWindow* const window, const coffee::WindowFocusEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(e.isFocusGained());
    }

    void ImGuiSystem::enterCallback(const coffee::AbstractWindow* const window, const coffee::WindowEnterEvent& e) {
        if (window->getCursorState() == coffee::CursorState::Disabled) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        if (e.entered()) {
            backendData->windowPtr = window;

            io.AddMousePosEvent(backendData->lastMousePos.x, backendData->lastMousePos.y);
        }
        else if (backendData->windowPtr == window) {
            backendData->windowPtr = nullptr;
            backendData->lastMousePos = io.MousePos;

            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
    }

    void ImGuiSystem::mouseClickCallback(const coffee::AbstractWindow* const window, const coffee::MouseClickEvent& e) {
        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.withControl());
        io.AddKeyEvent(ImGuiMod_Shift, e.withShift());
        io.AddKeyEvent(ImGuiMod_Alt, e.withAlt());
        io.AddKeyEvent(ImGuiMod_Super, e.withSuper());

        uint8_t button = static_cast<uint8_t>(e.getButton());
        if (button >= 0 && button < ImGuiMouseButton_COUNT) {
            io.AddMouseButtonEvent(button, e.getState() == coffee::State::Press);
        }
    }

    void ImGuiSystem::mousePositionCallback(const coffee::AbstractWindow* const window, const coffee::MouseMoveEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        float xPosition = e.getX();
        float yPosition = e.getY();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            coffee::Offset2D windowPosition = window->getWindowPosition();
            xPosition += windowPosition.x;
            yPosition += windowPosition.y;
        }

        io.AddMousePosEvent(xPosition, yPosition);
        backendData->lastMousePos = { xPosition, yPosition };
    }

    void ImGuiSystem::mouseWheelCallback(const coffee::AbstractWindow* const window, const coffee::MouseWheelEvent& e) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e.getX(), e.getY());
    }

    void ImGuiSystem::keyCallback(const coffee::AbstractWindow* const window, const coffee::KeyEvent& e) {
        if (e.getState() != coffee::State::Press && e.getState() != coffee::State::Release) {
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl, e.withControl());
        io.AddKeyEvent(ImGuiMod_Shift, e.withShift());
        io.AddKeyEvent(ImGuiMod_Alt, e.withAlt());
        io.AddKeyEvent(ImGuiMod_Super, e.withSuper());

        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        backendData->keyOwnerWindows[static_cast<uint32_t>(e.getKey())] = (e.getState() == coffee::State::Press) ? window : nullptr;

        ImGuiKey key = keyToImGui(e.getKey());
        io.AddKeyEvent(key, e.getState() == coffee::State::Press);
        io.SetKeyEventNativeData(key, static_cast<int>(e.getKey()), static_cast<int>(e.getScancode()));
    }

    void ImGuiSystem::charCallback(const coffee::AbstractWindow* const window, char32_t ch) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(ch);
    }

    void ImGuiSystem::updateMonitors() {
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        platformIO.Monitors.resize(0);

        for (const auto& monitor : coffee::Engine::getMonitors()) {
            ImGuiPlatformMonitor imGuiMonitor {};

            coffee::Extent2D position = monitor->getPosition();
            imGuiMonitor.MainPos = imGuiMonitor.WorkPos = ImVec2 { static_cast<float>(position.width), static_cast<float>(position.height) };
            
            coffee::VideoMode videoMode = monitor->getCurrentVideoMode();
            imGuiMonitor.MainSize = imGuiMonitor.WorkSize = ImVec2 { static_cast<float>(videoMode.width), static_cast<float>(videoMode.height) };

            coffee::WorkArea2D workArea = monitor->getWorkArea();
            if (workArea.extent.width > 0 && workArea.extent.height > 0) {
                imGuiMonitor.WorkPos = ImVec2 { static_cast<float>(workArea.offset.x), static_cast<float>(workArea.offset.y) };
                imGuiMonitor.WorkSize = ImVec2 { static_cast<float>(workArea.extent.width), static_cast<float>(workArea.extent.height) };
            }

            coffee::Float2D scale = monitor->getContentScale();
            imGuiMonitor.DpiScale = scale.x;

            platformIO.Monitors.push_back(imGuiMonitor);
        }
    }

    void ImGuiSystem::updateMouse(const coffee::Window& applicationWindow) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
        BackendPlatformData* backendData = static_cast<BackendPlatformData*>(io.BackendPlatformUserData);

        if (applicationWindow->getCursorState() == coffee::CursorState::Disabled) {
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
            return;
        }

        ImGuiID mouseViewportID = 0;
        const ImVec2 previousMousePosition = io.MousePos;

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::AbstractWindow* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (window->isFocused()) {
                if (io.WantSetMousePos) {
                    window->setMousePosition({ previousMousePosition.x - viewport->Pos.x, previousMousePosition.y - viewport->Pos.y });
                }

                if (backendData->windowPtr == nullptr) {
                    coffee::Float2D mousePosition = window->getMousePosition();

                    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                        mousePosition += window->getWindowPosition();
                    }

                    backendData->lastMousePos = ImVec2 { mousePosition.x, mousePosition.y };
                    io.AddMousePosEvent(mousePosition.x, mousePosition.y);
                }
            }

            bool windowNoInput = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;

            windowNoInput
                ? window->enablePassthrough()
                : window->disablePassthrough();

            if (window->isFocused() && !windowNoInput) {
                mouseViewportID = viewport->ID;
            }
        }

        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) {
            io.AddMouseViewportEvent(mouseViewportID);
        }
    }

    void ImGuiSystem::updateCursor(const coffee::Window& applicationWindow) {
        ImGuiIO& io = ImGui::GetIO();

        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || applicationWindow->getCursorState() == coffee::CursorState::Disabled) {
            return;
        }

        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

        for (int32_t i = 0; i < platformIO.Viewports.Size; i++) {
            ImGuiViewport* viewport = platformIO.Viewports[i];
            coffee::AbstractWindow* window = static_cast<ViewportData*>(viewport->PlatformUserData)->windowHandle;

            if (cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
                window->hideCursor();
                continue;
            }

            window->showCursor();
        }
    }

    void ImGuiSystem::prepareImGui(const coffee::Window& window) {
        coffee::Float2D framebufferSize = window->getFramebufferSize();
        coffee::Float2D mousePosition = window->getMousePosition();

        constexpr float splitterMinDistance = 16.0f;

        ImGui::NewFrame();
        ImGui::SetNextWindowSize({ framebufferSize.x, framebufferSize.y });
        ImGui::SetNextWindowPos({ 0.0f, 0.0f });

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PopStyleVar();

        if (ImGui::BeginMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                ImGui::SeparatorText("Actions");
                ImGui::MenuItem("Undo", "CTRL+Z");
                ImGui::MenuItem("Redo", "CTRL+Y");
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        static float splitterXPosition = 200.0f;
        static float splitterYPosition = 300.0f;
        ImGuiIO& io = ImGui::GetIO();

        ImGui::ShowDemoWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 1.0f, 1.0f });
        ImGui::BeginChild("Child1", { splitterXPosition, splitterYPosition }, true);
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::InvisibleButton("ActorGameImageSplitter", { 4.0f, splitterYPosition });
        if (ImGui::IsItemActive()) {
            splitterXPosition = std::clamp(mousePosition.x, splitterMinDistance, framebufferSize.x - splitterMinDistance);
        }

        ImGui::SameLine();
        ImGui::BeginChild("Child2", { 0.0f, splitterYPosition }, true);
        {
            ImGui::Text("Current splitter position: %.3f %.3f", splitterXPosition, splitterYPosition);
            ImGui::Text("Factory mouse position: %.3f %.3f", mousePosition.x, mousePosition.y);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }
        ImGui::EndChild();

        ImGui::InvisibleButton("HorizontalSplitter", { -1.0f, 4.0f });
        if (ImGui::IsItemActive()) {
            splitterYPosition = std::clamp(mousePosition.y, 16.0f, framebufferSize.y - 16.0f);
        }

        ImGui::BeginChild("Child3", { 0.0f, 0.0f }, true);
        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::EndFrame();
        ImGui::Render();
    }

    //{
//    ImGui::SetNextWindowSize({ static_cast<float>(factory.getFramebufferWidth()), static_cast<float>(factory.getFramebufferHeight()) });
//    ImGui::SetNextWindowPos({ 0.0f, 0.0f });

//    ImGui::Begin("Hello world!", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

//    ImGui::Text("Hello world!");

//    ImGui::SliderFloat("Framerate Limiter", &slider, 60.0f, 1440.0f);
//    ImGui::ColorEdit3("Clear Color", reinterpret_cast<float*>(&clearColor));

//    if (ImGui::Button("Apply new frame rate limit")) {
//        factory.setFrameLimit(slider);
//    }

//    ImGui::SameLine();
//    if (ImGui::Button("Change to FIFO")) {
//        factory.changePresentMode(coffee::PresentMode::FIFO);
//    }

//    ImGui::SameLine();
//    if (ImGui::Button("Change to Immediate")) {
//        factory.changePresentMode(coffee::PresentMode::Immediate);
//    }

//    ImGuiIO& io = ImGui::GetIO();
//    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

//    if (framebufferImage != nullptr) {
//        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
//        ImGui::Image(reinterpret_cast<void*>(&framebufferImage), viewportPanelSize);
//    }

//    ImGui::End();
//}

}

//ImGui::SetNextWindowSize({ static_cast<float>(factory.getFramebufferWidth()), static_cast<float>(factory.getFramebufferHeight()) });
//ImGui::SetNextWindowPos({ 0.0f, 0.0f });

//ImGui::Begin("Splitter", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

//static float windowWidth = 200.0f;
//static float windowHeight = 300.0f;
//ImGuiIO& io = ImGui::GetIO();

//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
//ImGui::BeginChild("Child1", { windowWidth, windowHeight }, true);
//ImGui::EndChild();

//ImGui::SameLine();
//ImGui::InvisibleButton("VerticalSplitter", { 8.0f, windowHeight });
//if (ImGui::IsItemActive() && windowWidth > std::abs(io.MouseDelta.x)) {
//    windowWidth += io.MouseDelta.x;
//}

//ImGui::SameLine();
//ImGui::BeginChild("Child2", { 0.0f, windowHeight }, true);
//{
//    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
//    ImGui::Image(reinterpret_cast<void*>(&framebufferImage), viewportPanelSize);
//}
//ImGui::EndChild();

//ImGui::InvisibleButton("HorizontalSplitter", { -1.0f, 8.0f });
//if (ImGui::IsItemActive() && windowHeight > std::abs(io.MouseDelta.y)) {
//    windowHeight += io.MouseDelta.y;
//}

//ImGui::BeginChild("Child3", { 0.0f, 0.0f }, true);
//ImGui::EndChild();

//ImGui::PopStyleVar();

//ImGui::End();