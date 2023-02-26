#version 450

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec3 fragNormalWorld;
layout (location = 2) in vec2 fragTexCoords;
layout (location = 3) in vec3 fragTangent;

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
    PointLight lightPoint;
} lightUbo;

const uint DIFFUSE_TEXTURE_BIT = 0x1;
const uint SPECULAR_TEXTURE_BIT = 0x2;
const uint NORMAL_TEXTURE_BIT = 0x4;

layout (set = 1, binding = 0) uniform MaterialsInfo {
    uint textureFlags;
    float shininess;
    vec3 diffuseColor;
    vec3 specularColor;
    vec4 ambientColor;
} materialsInformation;

layout (set = 1, binding = 1) uniform sampler2D textureDiffuse;
layout (set = 1, binding = 2) uniform sampler2D textureSpecular;
layout (set = 1, binding = 3) uniform sampler2D textureNormal;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main() {
    vec3 diffuseColor = materialsInformation.diffuseColor;
    vec3 specularColor = materialsInformation.specularColor;
    vec3 surfaceNormal = vec3(0.0);
    
    if ((materialsInformation.textureFlags & DIFFUSE_TEXTURE_BIT) != 0) {
        diffuseColor *= texture(textureDiffuse, fragTexCoords).rgb;
    }

    if ((materialsInformation.textureFlags & SPECULAR_TEXTURE_BIT) != 0) {
        specularColor *= texture(textureSpecular, fragTexCoords).rgb;
    }

    if ((materialsInformation.textureFlags & NORMAL_TEXTURE_BIT) != 0) {
        mat3 TBN = mat3(fragTangent, cross(fragNormalWorld, fragTangent), fragNormalWorld);
        surfaceNormal = texture(textureNormal, fragTexCoords).rgb;
        surfaceNormal = normalize(TBN * (surfaceNormal * 2.0 - 1.0));
    } else {
        surfaceNormal = normalize(fragNormalWorld);
    }

    vec3 cameraPosWorld = mvpUbo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    // Ambient light
    vec3 ambientLight = materialsInformation.ambientColor.rgb * materialsInformation.ambientColor.a * diffuseColor;

    // Attenuation and intensity
    vec3 directionToLight = lightUbo.lightPoint.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    vec3 intensity = lightUbo.lightPoint.color.rgb * lightUbo.lightPoint.color.a * attenuation;

    // Diffuse light
    float cosAngIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0.0);
    vec3 diffuseLight = cosAngIncidence * diffuseColor * intensity;

    // Specular light
    vec3 halfAngle = normalize(normalize(directionToLight) + viewDirection);
    float blinnTerm = pow(clamp(dot(surfaceNormal, halfAngle), 0.0, 1.0), materialsInformation.shininess);
    vec3 specularLight = blinnTerm * specularColor * intensity;

    outColor = vec4(ambientLight + diffuseLight + specularLight, 1.0);
}