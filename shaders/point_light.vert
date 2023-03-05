#version 450

const vec2 OFFSETS[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(-1.0, 1.0),
    vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout (set = 0, binding = 0) uniform MVPUBO {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUbo;

layout (push_constant) uniform Push {
    vec4 position; // w is radius
    vec4 color;
} push;

void main() {
    fragOffset = OFFSETS[gl_VertexIndex];

    vec3 cameraRightWorld = { mvpUbo.view[0][0], mvpUbo.view[1][0], mvpUbo.view[2][0] };
    vec3 cameraUpWorld = { mvpUbo.view[0][1], mvpUbo.view[1][1], mvpUbo.view[2][1] };

    vec3 positionWorld = push.position.xyz
        + push.position.w * fragOffset.x * cameraRightWorld
        + push.position.w * fragOffset.y * cameraUpWorld;

    gl_Position = mvpUbo.projection * mvpUbo.view * vec4(positionWorld, 1.0);
}