#version 450

layout (set = 0, binding = 1) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 fragWorldUV;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = texture(samplerCubeMap, fragWorldUV);
}