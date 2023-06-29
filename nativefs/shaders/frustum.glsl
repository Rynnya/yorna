#ifndef FRUSTUM_GLSL
#define FRUSTUM_GLSL

const uint FRUSTUM_BLOCK_SIZE = 16;

struct Plane {
    vec4 normalDistance;
};

// Order are following:
// - Left
// - Right
// - Top
// - Bottom
struct Frustum {
    Plane planes[4];
};

struct Sphere {
    vec4 centerRadius;
};

struct Cone {
    vec4 tipHeight;
    vec4 directionRadius;
};

Plane computePlane(vec3 p0, vec3 p1, vec3 p2) {
    Plane plane;

    vec3 v0 = p1 - p0;
    vec3 v2 = p2 - p0;

    plane.normalDistance.xyz = normalize(cross(v0, v2));
    plane.normalDistance.w = dot(plane.normalDistance.xyz, p0);

    return plane;
}

vec4 clipToView(mat4 inverseProjection, vec4 clip) {
    vec4 view = inverseProjection * clip;
    view = view / view.w;
    return view;
}

vec4 screenToView(mat4 inverseProjection, vec2 screenDimensions, vec4 screen) {
    vec2 texCoords = screen.xy / screenDimensions;
    vec4 clip = vec4(vec2(texCoords.x, 1.0 - texCoords.y) * 2.0 - 1.0, screen.z, screen.w);
    return clipToView(inverseProjection, clip);
}

bool pointInsidePlane(Plane plane, vec3 point) {
    return dot(plane.normalDistance.xyz, point) - plane.normalDistance.w < 0;
}

bool sphereInsidePlane(Plane plane, Sphere sphere) {
    return dot(plane.normalDistance.xyz, sphere.centerRadius.xyz) - plane.normalDistance.w < -sphere.centerRadius.w;
}

bool coneInsidePlane(Plane plane, Cone cone) {
    vec3 parallel = cross(cross(plane.normalDistance.xyz, cone.directionRadius.xyz), cone.directionRadius.xyz);
    vec3 farthest = cone.tipHeight.xyz + cone.directionRadius.xyz * cone.directionRadius.w - parallel * cone.directionRadius.w;

    return pointInsidePlane(plane, cone.tipHeight.xyz) && pointInsidePlane(plane, farthest);
}

bool sphereInsideFrustum(Frustum frustum, Sphere sphere, float zNear, float zFar) {
    bool result = true;

    if (sphere.centerRadius.z - sphere.centerRadius.w > zNear || sphere.centerRadius.z + sphere.centerRadius.w < zFar) {
        result = false;
    }

    for (int i = 0; i < 4 && result; i++) {
        if (sphereInsidePlane(frustum.planes[i], sphere)) {
            result = false;
        }
    }

    return result;
}

bool coneInsideFrustum(Frustum frustum, Cone cone, float zNear, float zFar) {
    bool result = true;

    Plane nearPlane = { vec4(0.0, 0.0, -1.0, -zNear) };
    Plane farPlane = { vec4(0.0, 0.0, 1.0, zFar) };

    if (coneInsidePlane(nearPlane, cone) || coneInsidePlane(farPlane, cone)) {
        result = false;
    }

    for (int i = 0; i < 4 && result; i++) {
        if (coneInsidePlane(frustum.planes[i], cone)) {
            result = false;
        }
    }

    return result;
}

#endif