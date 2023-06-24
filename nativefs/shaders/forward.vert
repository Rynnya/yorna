#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;

layout (location = 0) out vec3 fragPosWorld;
layout (location = 1) out vec3 fragNormalWorld;
layout (location = 2) out vec2 fragTexCoords;
layout (location = 3) out vec3 fragTangent;

layout (set = 0, binding = 0) uniform MVPUBO {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUbo;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    mat3 normalMatrix = inverse(transpose(mat3(push.normalMatrix)));

    fragPosWorld = positionWorld.xyz;
    fragNormalWorld = normalize(normalMatrix * normal);
    fragTexCoords = texCoords;
    fragTangent = normalize(normalMatrix * tangent);

    gl_Position = mvpUbo.projection * mvpUbo.view * positionWorld;
}