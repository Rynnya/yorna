#version 450

layout (location = 0) in vec3 position;

layout (location = 0) out vec3 fragWorldUV;

layout (set = 0, binding = 0) uniform MVPUniformBuffer {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUniformBuffer;

void main() {
    fragWorldUV = position;

    vec4 clip = mvpUniformBuffer.projection * mat4(mat3(mvpUniformBuffer.view)) * vec4(fragWorldUV, 1.0);
    gl_Position = vec4(clip.xy, 0.0, clip.w);
}