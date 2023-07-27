#version 450

layout (location = 0) flat in uint objectID;

layout (set = 0, binding = 1, std430) writeonly buffer PickedObjectSSBO {
    uint objectID;
} pickedObjectSSBO;

void main() {
    pickedObjectSSBO.objectID = objectID;
}