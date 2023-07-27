#version 450

layout (location = 0) in vec3 position;

layout (location = 0) out uint objectID;

layout (set = 0, binding = 0) uniform MVPUniformBuffer {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUniformBuffer;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    uint objectID;
} push;

void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

    gl_Position = mvpUniformBuffer.projection * mvpUniformBuffer.view * positionWorld;
    objectID = push.objectID;
}