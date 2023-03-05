#version 450

#define M_PI 3.1415926535897932384626433832795

const float F0 = 0.4;

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec3 fragNormalWorld;
layout (location = 2) in vec2 fragTexCoords;
layout (location = 3) in vec3 fragTangent;

layout (location = 0) out vec4 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

struct PBRData {
    vec3 diffuseColor;
    vec3 specularColor;
    float metallicFactor;
    float roughnessFactor;

    vec3 microSurface; // N
    vec3 eyeToSurface; // V

    float macroViewAngle;  // NdV
    float halfLightAngle;  // HdL
    float macroLightAngle; // NdL
    float macroHalfAngle;  // NdH 
    float halfViewAngle;   // VdH

    float roughnessSquared;
    float roughnessQuadripled;
};

layout (set = 0, binding = 0) uniform MVPUBO {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUbo;

layout (set = 0, binding = 1) uniform LightUBO {
    PointLight[10] lightPoints;
    uint size;
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

float FresnelSchlick(float F0, float cosTheta) {
    return F0 + (1.0 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

vec3 LambertDiffuse(vec3 radiance, float macroLightAngle) {
    return (1.0 / M_PI) * radiance * macroLightAngle;
}

float VisSchlick(PBRData data) {
    float toSquare = (data.roughnessSquared + 1.0);
    float directLighting = (toSquare * toSquare) / 8.0;
	
	float geometryFromView = data.macroViewAngle * (1.0 - directLighting) + directLighting;
	float geometryFromLight = data.macroLightAngle * (1.0 - directLighting) + directLighting;
	return 0.25 / max(geometryFromView * geometryFromLight, 1e-6);
}

float Beckmann(PBRData data) {
    float macroHalfAngleSquared = data.macroHalfAngle * data.macroHalfAngle;
    float roughnessAngle = data.roughnessQuadripled * macroHalfAngleSquared;

    return exp((macroHalfAngleSquared - 1.0) / max(roughnessAngle, 1e-6)) / max(M_PI * roughnessAngle * macroHalfAngleSquared, 1e-6);
}

vec3 calculateSchlickBeckmannBRDF(PBRData data, vec3 radiance) {
    float fresnel = FresnelSchlick(F0, data.halfLightAngle);
    float visSchlick = VisSchlick(data);
    float distribution = Beckmann(data);

    return data.macroLightAngle * fresnel * distribution * visSchlick * radiance;
}

vec3 calculatePBR(PBRData data) {
    data.microSurface = normalize(data.microSurface); // N
    data.eyeToSurface = normalize(data.eyeToSurface); // V

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    data.macroViewAngle = max(dot(data.microSurface, data.eyeToSurface), 0.0); // NdV

    for (uint i = 0; i < lightUbo.size; i++) {
        vec3 directionToLight = vec3(lightUbo.lightPoints[i].position) - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        vec3 radiance = attenuation * lightUbo.lightPoints[i].color.rgb * lightUbo.lightPoints[i].color.a;

        directionToLight = normalize(directionToLight);
        vec3 halfWayAngle = normalize(data.eyeToSurface + directionToLight); // H

        data.halfLightAngle = max(dot(halfWayAngle, directionToLight), 0.0); // HdL
        data.macroLightAngle = max(dot(data.microSurface, directionToLight), 0.0); // NdL
        data.macroHalfAngle = max(dot(data.microSurface, halfWayAngle), 0.0); // NdH
        data.halfViewAngle = max(dot(halfWayAngle, data.eyeToSurface), 0.0); // VdH
        data.roughnessSquared = data.roughnessFactor * data.roughnessFactor;
        data.roughnessQuadripled = data.roughnessSquared * data.roughnessSquared;

        diffuse += 
            (1.0 - FresnelSchlick(F0, data.macroLightAngle)) * 
            (1.0 - FresnelSchlick(F0, data.macroViewAngle)) *
            LambertDiffuse(radiance, data.macroLightAngle) *
            radiance;
        specular += calculateSchlickBeckmannBRDF(data, radiance);
    }

    vec3 reflectedDiffuse = mix(diffuse * data.diffuseColor, vec3(0.0), data.metallicFactor);
    vec3 reflectedSpecular = mix(specular, specular * data.diffuseColor, data.metallicFactor) * data.specularColor;

    return reflectedDiffuse + reflectedSpecular;
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

    PBRData data;
    data.diffuseColor = diffuseColor;
    data.specularColor = specularColor;
    data.metallicFactor = metallic;
    data.roughnessFactor = roughness;
    data.microSurface = surfaceNormal;
    data.eyeToSurface = viewDirection;

    vec3 ambientLight = materialsInformation.ambientColor.rgb * materialsInformation.ambientColor.a * diffuseColor;
    vec3 pbrLight = calculatePBR(data);

    outColor = vec4(ambientLight + pbrLight, 1.0);
}