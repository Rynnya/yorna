#ifndef LIGHT_OBJECTS_GLSL
#define LIGHT_OBJECTS_GLSL

const float FRESNEL_0 = 0.4;
const float PI = 3.1415926535897932384626433832795;

struct PointLight {
    vec4 position;  // w is radius
    vec4 color;     // w is intensity
};

struct SpotLight {
    vec4 position;      // w is radius
    vec4 color;         // w is intensity
    vec4 coneDirection; // w is coneAngle
};

struct DirectionalLight {
    vec4 direction;
    vec4 color;         // w is intensity
};

struct PBRData {
    vec3 diffuseColor;
    vec3 specularColor;
    float metallicFactor;
    float roughnessFactor;

    vec3 surfaceNormal; // N
    vec3 eyeToSurface; // V

    float surfaceViewAngle;  // NdV
    float halfLightAngle;  // HdL
    float surfaceLightAngle; // NdL
    float surfaceHalfAngle;  // NdH 
    float halfViewAngle;   // VdH

    float roughnessSquared;
    float roughnessQuadripled;
};

#endif