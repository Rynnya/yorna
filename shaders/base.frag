#version 450

#define M_PI 3.1415926535897932384626433832795

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
const uint AMBIENT_TEXTURE_BIT = 0x4;
const uint EMISSIVE_TEXTURE_BIT = 0x8;
const uint HEIGHT_TEXTURE_BIT = 0x10;
const uint NORMAL_TEXTURE_BIT = 0x20;
const uint ROUGHNESS_TEXTURE_BIT = 0x40;
const uint METALLIC_TEXTURE_BIT = 0x80;
const uint AMBIENT_OCCLUSION_TEXTURE_BIT = 0x100;

layout (set = 1, binding = 0) uniform MaterialsInfo {
    vec3 diffuseColor;
    vec3 specularColor;
    vec4 ambientColor;
    uint textureFlags;
    float shininessExponent;
    float metallicFactor;
    float roughnessFactor;
} materialsInformation;

layout (set = 1, binding = 1) uniform sampler2D textureDiffuse;
layout (set = 1, binding = 2) uniform sampler2D textureSpecular;
layout (set = 1, binding = 3) uniform sampler2D textureNormal;
layout (set = 1, binding = 4) uniform sampler2D textureMetallic;
layout (set = 1, binding = 5) uniform sampler2D textureRoughness;

layout (push_constant) uniform Push {
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

float GXXPartialGeometry(float macroAngle, float roughnessQuadripled) {
    float angleSquared = clamp(macroAngle * macroAngle, 0.0, 1.0);
    float tanSquared = (1 - angleSquared) / angleSquared;
    return 2 / (1 + sqrt(1 + roughnessQuadripled * tanSquared));
}

float GXXDistribution(float macroHeightAngle, float roughnessQuadripled) {
    float angleSquared = clamp(macroHeightAngle * macroHeightAngle, 0.0, 1.0);
    float subFormula = angleSquared * roughnessQuadripled + (1.0 - angleSquared);
    return angleSquared / (M_PI * subFormula * subFormula);
}

float FresnelSchlick(float F0, float cosTheta) {
    return F0 + (1.0 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

vec3 CookTorrance(
    vec3 diffuseColor,
    vec3 macroNormal,
    vec3 lightVector,
    vec3 viewVector,
    float metallicFactor,
    float roughnessFactor,
    float F0
) {
    macroNormal = normalize(macroNormal);
    lightVector = normalize(lightVector);
    viewVector = normalize(viewVector);
    vec3 heightVector = normalize(viewVector + lightVector);

    float macroLightAngle = dot(macroNormal, lightVector);
    if (macroLightAngle <= 0.0) {
        return vec3(0.0);
    }

    float macroViewAngle = dot(macroNormal, viewVector);
    if (macroViewAngle <= 0.0) {
        return vec3(0.0);
    }

    float macroHeightAngle = dot(macroNormal, heightVector);
    float heightViewAngle = dot(heightVector, viewVector);
    float roughnessQuadripled = roughnessFactor * roughnessFactor * roughnessFactor * roughnessFactor;

    float fullGeometry = GXXPartialGeometry(macroViewAngle, roughnessQuadripled) * GXXPartialGeometry(macroLightAngle, roughnessQuadripled);
    float distribution = GXXDistribution(macroHeightAngle, roughnessQuadripled);
    float fresnelSchlick = FresnelSchlick(F0, heightViewAngle);

    float specularCoefficient = fullGeometry * distribution * fresnelSchlick * 0.25 / heightViewAngle;
    float diffuseCoefficient = clamp(1.0 - fresnelSchlick, 0.0, 1.0);

    return max(diffuseColor * diffuseCoefficient * macroLightAngle / M_PI + specularCoefficient, 0.0);
}

void main() {
    vec3 diffuseColor = materialsInformation.diffuseColor;
    vec3 specularColor = materialsInformation.specularColor;
    vec3 surfaceNormal = vec3(0.0);
    float metallic = materialsInformation.metallicFactor;
    float roughness = materialsInformation.roughnessFactor;
    
    if ((materialsInformation.textureFlags & DIFFUSE_TEXTURE_BIT) != 0) {
        diffuseColor *= texture(textureDiffuse, fragTexCoords).rgb;
    }

    if ((materialsInformation.textureFlags & SPECULAR_TEXTURE_BIT) != 0) {
        specularColor *= texture(textureSpecular, fragTexCoords).x;
    }

    if ((materialsInformation.textureFlags & NORMAL_TEXTURE_BIT) != 0) {
        mat3 TBN = mat3(fragTangent, cross(fragNormalWorld, fragTangent), fragNormalWorld);
        surfaceNormal = texture(textureNormal, fragTexCoords).xyz;
        surfaceNormal = normalize(TBN * (surfaceNormal * 2.0 - 1.0));
    } else {
        surfaceNormal = normalize(fragNormalWorld);
    }

    if ((materialsInformation.textureFlags & METALLIC_TEXTURE_BIT) != 0) {
        metallic *= texture(textureMetallic, fragTexCoords).x;
    }

    if ((materialsInformation.textureFlags & ROUGHNESS_TEXTURE_BIT) != 0) {
        roughness *= texture(textureRoughness, fragTexCoords).x;
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
    float blinnTerm = pow(clamp(dot(surfaceNormal, halfAngle), 0.0, 1.0), materialsInformation.shininessExponent);
    vec3 specularLight = blinnTerm * specularColor * intensity;

    outColor = vec4(ambientLight + diffuseLight + specularLight, 1.0);
}