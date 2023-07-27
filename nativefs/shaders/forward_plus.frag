#version 450

#include "frustum.glsl"
#include "light_objects.glsl"
#include "materials.glsl"

layout (constant_id = 0) const uint MAX_AMOUNT_OF_POINT_LIGHTS = 64;
layout (constant_id = 1) const uint MAX_AMOUNT_OF_SPOT_LIGHTS = 32;

const float MAX_SHADOW_BIAS = 0.05;
const float MIN_SHADOW_BIAS = 0.01;

layout (location = 0) in vec3 fragPosWorld;
layout (location = 1) in vec3 fragNormalWorld;
layout (location = 2) in vec2 fragTexCoords;
layout (location = 3) in vec3 fragTangent;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform MVPUniformBuffer {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
} mvpUniformBuffer;

layout (set = 0, binding = 1) uniform SunlightUniformBuffer {
    mat4 sunlightSpaceMatrix;
    vec4 sunlightDirection;
    vec4 sunlightColor; // w is sunlightIntensity
} sunlightUniformBuffer;

layout (set = 0, binding = 2) uniform sampler2D sunlightShadowMap;

layout (set = 0, binding = 3) uniform PointLights { PointLight pointLights[MAX_AMOUNT_OF_POINT_LIGHTS]; };
layout (set = 0, binding = 4) uniform SpotLights { SpotLight spotLights[MAX_AMOUNT_OF_SPOT_LIGHTS]; };
layout (set = 0, binding = 5, std430) readonly buffer PointLightIndexList { uint pointLightIndexList[]; };
layout (set = 0, binding = 6, std430) readonly buffer SpotLightIndexList { uint spotLightIndexList[]; };
layout (set = 0, binding = 7, rg32ui) uniform readonly uimage2D pointLightGrid;
layout (set = 0, binding = 8, rg32ui) uniform readonly uimage2D spotLightGrid;

layout (set = 1, binding = 0) uniform MaterialsInfo {
    vec4 diffuseColor; // w is metallicFactor
    vec4 specularColor; // w is roughnessFactor
    uint textureFlags;
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

vec3 LambertDiffuse(vec3 radiance, float surfaceLightAngle) {
    return (1.0 / PI) * radiance * surfaceLightAngle;
}

float VisSchlick(PBRData data) {
    float toSquare = (data.roughnessSquared + 1.0);
    float directLighting = (toSquare * toSquare) / 8.0;
	
	float geometryFromView = data.surfaceViewAngle * (1.0 - directLighting) + directLighting;
	float geometryFromLight = data.surfaceLightAngle * (1.0 - directLighting) + directLighting;
	return 0.25 / max(geometryFromView * geometryFromLight, 1e-6);
}

float Beckmann(PBRData data) {
    float surfaceHalfAngleSquared = data.surfaceHalfAngle * data.surfaceHalfAngle;
    float roughnessAngle = data.roughnessQuadripled * surfaceHalfAngleSquared;

    return exp((surfaceHalfAngleSquared - 1.0) / max(roughnessAngle, 1e-6)) / max(PI * roughnessAngle * surfaceHalfAngleSquared, 1e-6);
}

vec3 calculateSchlickBeckmannBRDF(PBRData data, vec3 radiance) {
    float fresnel = FresnelSchlick(FRESNEL_0, data.halfLightAngle);
    float visSchlick = VisSchlick(data);
    float distribution = Beckmann(data);

    return data.surfaceLightAngle * fresnel * distribution * visSchlick * radiance;
}

float calculateShadow(vec3 shadowCoord, vec2 offset) {
    float closestDepth = texture(sunlightShadowMap, shadowCoord.xy + offset).r;
    float currentDepth = shadowCoord.z + MIN_SHADOW_BIAS;
    return currentDepth > closestDepth ? 0.0 : 1.0;
}

float filterPCF() {
    vec4 lightSpacePos = sunlightUniformBuffer.sunlightSpaceMatrix * vec4(fragPosWorld, 1.0);
    vec3 projectionCoordinates = lightSpacePos.xyz / lightSpacePos.w;
    projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

    ivec2 shadowMapSize = textureSize(sunlightShadowMap, 0);
    const float scale = 1.5;
    float dx = scale * 1.0 / float(shadowMapSize.x);
    float dy = scale * 1.0 / float(shadowMapSize.y);

    float shadowFactor = 0.0;
    
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            shadowFactor += calculateShadow(projectionCoordinates, vec2(dx * x, dy * y));
        }
    }

    return shadowFactor / 8.0;
}

vec3 calculatePBR(PBRData data) {
    data.surfaceNormal = normalize(data.surfaceNormal); // N
    data.eyeToSurface = normalize(data.eyeToSurface); // V

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    data.surfaceViewAngle = max(dot(data.surfaceNormal, data.eyeToSurface), 0.0); // NdV

    ivec2 tilePosition = ivec2(floor(gl_FragCoord.xy / FRUSTUM_BLOCK_SIZE));
    uvec4 pointLightInformation = imageLoad(pointLightGrid, tilePosition);
    uvec4 spotLightInformation = imageLoad(spotLightGrid, tilePosition);
    uint pointLightOffset = pointLightInformation.x;
    uint pointLightCount = pointLightInformation.y;
    uint spotLightOffset = spotLightInformation.x;
    uint spotLightCount = spotLightInformation.y;

    for (uint i = 0; i < pointLightCount; i++) {
        uint lightIndex = pointLightIndexList[pointLightOffset + i];
        PointLight pointLight = pointLights[lightIndex];

        vec3 directionToLight = pointLight.position - fragPosWorld;
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        vec3 radiance = attenuation * pointLight.color.rgb * pointLight.radius;

        directionToLight = normalize(directionToLight);
        vec3 halfWayAngle = normalize(data.eyeToSurface + directionToLight); // H

        data.halfLightAngle = max(dot(halfWayAngle, directionToLight), 0.0); // HdL
        data.surfaceLightAngle = max(dot(data.surfaceNormal, directionToLight), 0.0); // NdL
        data.surfaceHalfAngle = max(dot(data.surfaceNormal, halfWayAngle), 0.0); // NdH
        data.halfViewAngle = max(dot(halfWayAngle, data.eyeToSurface), 0.0); // VdH
        data.roughnessSquared = data.roughnessFactor * data.roughnessFactor;
        data.roughnessQuadripled = data.roughnessSquared * data.roughnessSquared;

        diffuse += 
            (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceLightAngle)) * 
            (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceViewAngle)) *
            LambertDiffuse(radiance, data.surfaceLightAngle) *
            radiance;
        specular += calculateSchlickBeckmannBRDF(data, radiance);
    }

    for (uint i = 0; i < spotLightCount; i++) {
        uint lightIndex = spotLightIndexList[spotLightOffset + i];
        SpotLight spotLight = spotLights[lightIndex];

        vec3 directionToLight = spotLight.position - fragPosWorld;
        float lightToSurfaceAngle = acos(dot(-normalize(directionToLight), normalize(spotLight.coneDirection)));

        if (lightToSurfaceAngle <= radians(spotLight.coneAngle)) {
            float attenuation = 1.0 / dot(directionToLight, directionToLight);
            vec3 radiance = attenuation * spotLight.color.rgb * spotLight.radius;

            directionToLight = normalize(directionToLight);
            vec3 halfWayAngle = normalize(data.eyeToSurface + directionToLight); // H

            data.halfLightAngle = max(dot(halfWayAngle, directionToLight), 0.0); // HdL
            data.surfaceLightAngle = max(dot(data.surfaceNormal, directionToLight), 0.0); // NdL
            data.surfaceHalfAngle = max(dot(data.surfaceNormal, halfWayAngle), 0.0); // NdH
            data.halfViewAngle = max(dot(halfWayAngle, data.eyeToSurface), 0.0); // VdH
            data.roughnessSquared = data.roughnessFactor * data.roughnessFactor;
            data.roughnessQuadripled = data.roughnessSquared * data.roughnessSquared;

            diffuse += 
                (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceLightAngle)) * 
                (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceViewAngle)) *
                LambertDiffuse(radiance, data.surfaceLightAngle) *
                radiance;
            specular += calculateSchlickBeckmannBRDF(data, radiance);
        }
    }

    vec3 directionToLight = normalize(vec3(sunlightUniformBuffer.sunlightDirection));
    vec3 radiance = sunlightUniformBuffer.sunlightColor.rgb * sunlightUniformBuffer.sunlightColor.a;
    vec3 halfWayAngle = normalize(data.eyeToSurface + directionToLight); // H

    data.halfLightAngle = max(dot(halfWayAngle, directionToLight), 0.0); // HdL
    data.surfaceLightAngle = max(dot(data.surfaceNormal, directionToLight), 0.0); // NdL
    data.surfaceHalfAngle = max(dot(data.surfaceNormal, halfWayAngle), 0.0); // NdH
    data.halfViewAngle = max(dot(halfWayAngle, data.eyeToSurface), 0.0); // VdH
    data.roughnessSquared = data.roughnessFactor * data.roughnessFactor;
    data.roughnessQuadripled = data.roughnessSquared * data.roughnessSquared;

    float sunlightShadow = 1.0 - filterPCF();
    diffuse += 
        sunlightShadow * 
        (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceLightAngle)) * 
        (1.0 - FresnelSchlick(FRESNEL_0, data.surfaceViewAngle)) *
        LambertDiffuse(radiance, data.surfaceLightAngle) *
        radiance;
    specular += sunlightShadow * calculateSchlickBeckmannBRDF(data, radiance);

    vec3 reflectedDiffuse = mix(diffuse * data.diffuseColor, vec3(0.0), data.metallicFactor);
    vec3 reflectedSpecular = mix(specular, specular * data.diffuseColor, data.metallicFactor) * data.specularColor;

    return reflectedDiffuse + reflectedSpecular;
}

void main() {
    vec3 diffuseColor = materialsInformation.diffuseColor.rgb;
    vec3 specularColor = materialsInformation.specularColor.rgb;
    vec3 surfaceNormal = vec3(0.0);
    float metallic = materialsInformation.diffuseColor.a;
    float roughness = materialsInformation.specularColor.a;
    
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

    vec3 cameraPosWorld = mvpUniformBuffer.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    PBRData data;
    data.diffuseColor = diffuseColor;
    data.specularColor = specularColor;
    data.metallicFactor = metallic;
    data.roughnessFactor = roughness;
    data.surfaceNormal = surfaceNormal;
    data.eyeToSurface = viewDirection;

    vec3 ambientLight = mvpUniformBuffer.ambientLightColor.rgb * mvpUniformBuffer.ambientLightColor.a * diffuseColor;
    vec3 pbrLight = calculatePBR(data);

    vec3 rawColor = ambientLight + pbrLight;
    outColor = vec4(pow(rawColor, vec3(1.0 / 2.2)), 1.0);
}