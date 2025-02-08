#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


bool rayIntersectsAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 boxMin, glm::vec3 boxMax) {
    // Check each plane on the AABB and see if the ray gets up in there
    float t, x_i, y_i, z_i;

    // XY0
    t = (boxMin.z - rayOrigin.z) / rayDir.z;
    x_i = rayOrigin.x + rayDir.x * t;
    y_i = rayOrigin.y + rayDir.y * t;
    if (t >= 0.0f && boxMin.x <= x_i && x_i <= boxMax.x && boxMin.y <= y_i && y_i <= boxMax.y) {
        return true;
    }

    // XY1
    t = (boxMax.z - rayOrigin.z) / rayDir.z;
    x_i = rayOrigin.x + rayDir.x * t;
    y_i = rayOrigin.y + rayDir.y * t;
    if (t >= 0.0f && boxMin.x <= x_i && x_i <= boxMax.x && boxMin.y <= y_i && y_i <= boxMax.y) {
        return true;
    }

    // XZ0
    t = (boxMin.y - rayOrigin.y) / rayDir.y;
    x_i = rayOrigin.x + rayDir.x * t;
    z_i = rayOrigin.z + rayDir.z * t;
    if (t >= 0.0f && boxMin.x <= x_i && x_i <= boxMax.x && boxMin.z <= z_i && z_i <= boxMax.z) {
        return true;
    }

    // XZ1
    t = (boxMax.y - rayOrigin.y) / rayDir.y;
    x_i = rayOrigin.x + rayDir.x * t;
    z_i = rayOrigin.z + rayDir.z * t;
    if (t >= 0.0f && boxMin.x <= x_i && x_i <= boxMax.x && boxMin.z <= z_i && z_i <= boxMax.z) {
        return true;
    }

    // YZ0
    t = (boxMin.x - rayOrigin.x) / rayDir.x;
    y_i = rayOrigin.y + rayDir.y * t;
    z_i = rayOrigin.z + rayDir.z * t;
    if (t >= 0.0f && boxMin.z <= z_i && z_i <= boxMax.z && boxMin.y <= y_i && y_i <= boxMax.y) {
        return true;
    }

    // YZ1
    t = (boxMax.x - rayOrigin.x) / rayDir.x;
    y_i = rayOrigin.y + rayDir.y * t;
    z_i = rayOrigin.z + rayDir.z * t;
    if (t >= 0.0f && boxMin.z <= z_i && z_i <= boxMax.z && boxMin.y <= y_i && y_i <= boxMax.y) {
        return true;
    }

    return false;
}

#endif