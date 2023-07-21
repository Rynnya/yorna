#ifndef FRUSTUM_GLSL
#define FRUSTUM_GLSL

// Setting anything else will cause culling to go crazy for some reason
const uint FRUSTUM_BLOCK_SIZE = 16;

struct Plane {
    vec3 normal;
    float dist;
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
    vec3 center;
    float radius;
};

struct Cone {
    vec3 tip;
    float height;
    vec3 direction;
    float radius;
};

Plane computePlane(vec3 p0, vec3 p1, vec3 p2) {
    Plane plane;

    vec3 v0 = p1 - p0;
    vec3 v2 = p2 - p0;

    plane.normal = normalize(cross(v0, v2));
    plane.dist = dot(plane.normal, p0);

    return plane;
}

vec4 clipToView(mat4 inverseProjection, vec4 clip) {
    vec4 view = inverseProjection * clip;
    view = view / view.w;
    return view;
}

vec4 screenToView(mat4 inverseProjection, vec2 screenDimensions, vec4 screenSpace) {
    vec2 texCoords = screenSpace.xy / screenDimensions;
    vec4 clip = vec4(texCoords * 2.0 - 1.0, screenSpace.z, screenSpace.w);
    return clipToView(inverseProjection, clip);
}

bool pointInsidePlane(Plane plane, vec3 point) {
    return dot(plane.normal, point) - plane.dist < 0.0;
}

bool sphereInsidePlane(Plane plane, Sphere sphere) {
    return dot(plane.normal, sphere.center) - plane.dist < -sphere.radius;
}

bool coneInsidePlane(Plane plane, Cone cone) {
    vec3 furthestPointDirection = cross(cross(plane.normal, cone.direction), cone.direction);
    vec3 furthestPointOnCircle = cone.tip + cone.direction * cone.height - furthestPointDirection * cone.radius;

    return pointInsidePlane(plane, cone.tip) && pointInsidePlane(plane, furthestPointOnCircle);
}

bool sphereInsideFrustum(Frustum frustum, Sphere sphere, float zNear, float zFar) {
    bool result = true;

    if (sphere.center.z - sphere.radius > zNear || sphere.center.z + sphere.radius < zFar) {
        result = false;
    }

    for (int i = 0; i < 4 && result; i++) {
        result = !sphereInsidePlane(frustum.planes[i], sphere);
    }

    return result;
}

bool coneInsideFrustum(Frustum frustum, Cone cone, float zNear, float zFar) {
    bool result = true;

    Plane nearPlane = { vec3(0.0, 0.0, -1.0), -zNear };
    Plane farPlane = { vec3(0.0, 0.0, 1.0), zFar };

    if (coneInsidePlane(nearPlane, cone) || coneInsidePlane(farPlane, cone)) {
        result = false;
    }

    for (int i = 0; i < 4 && result; i++) {
        result = !coneInsidePlane(frustum.planes[i], cone);
    }

    return result;
}

#endif