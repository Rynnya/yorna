#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

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

layout (set = 0, binding = 1) uniform LightUBO {
    PointLight lightPoints[10];
    uint len;
} lightUbo;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffuseLight = mvpUbo.ambientLightColor.xyz * mvpUbo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = mvpUbo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for (uint i = 0; i < lightUbo.len; i++) {
        PointLight light = lightUbo.lightPoints[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);

        directionToLight = normalize(directionToLight);
        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = pow(clamp(dot(surfaceNormal, halfAngle), 0, 1), 512.0);

        specularLight += intensity * blinnTerm;
    }

    outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}