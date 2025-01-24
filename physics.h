#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Update physics function
// If on floor, dont need to calculate
// If in the air, need to calculate
// Calculate collisions
// What is the floor if the ground is not level? or if cubes are placed all over the place
    // Use a bbox, calculate collisions

float YSNAP = 0.05;

struct AABB {
    // These are all planes
    float x0;
    float x1;
    float y0;
    float y1;
    float z0;
    float z1;
};

AABB rbBBox = AABB{-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f};
AABB player = AABB{-0.2f, 0.2f, -0.5f, 0.5f, -0.2f, 0.2f};

float yVelocity = 0.0f;

glm::vec3 calcGravity() {
    glm::vec3 newPos = glm::vec3(0.0f, 0.0f, 0.0f);
    yVelocity -= 0.0025f;
    newPos.y += yVelocity;
    return newPos;
}

glm::vec3 calcCollisions(glm::vec3 playerPos, glm::vec3 rigidBodyPos) {
    glm::vec3 newPos = playerPos;

    // Translate player bbox
    AABB playerTrans;
    playerTrans.x0 = player.x0 + playerPos.x;
    playerTrans.y0 = player.y0 + playerPos.y;
    playerTrans.z0 = player.z0 + playerPos.z;
    playerTrans.x1 = player.x1 + playerPos.x;
    playerTrans.y1 = player.y1 + playerPos.y;
    playerTrans.z1 = player.z1 + playerPos.z;

    AABB rbBoxTrans;
    rbBoxTrans.x0 = rbBBox.x0 + rigidBodyPos.x;
    rbBoxTrans.y0 = rbBBox.y0 + rigidBodyPos.y;
    rbBoxTrans.z0 = rbBBox.z0 + rigidBodyPos.z;
    rbBoxTrans.x1 = rbBBox.x1 + rigidBodyPos.x;
    rbBoxTrans.y1 = rbBBox.y1 + rigidBodyPos.y;
    rbBoxTrans.z1 = rbBBox.z1 + rigidBodyPos.z;

    // Check for overlap
    // X
    bool xCol = false;
    float xOverlap = 0.0f;
    if (rbBoxTrans.x0 <= playerTrans.x0 && playerTrans.x0 <= rbBoxTrans.x1 && rbBoxTrans.x1 <= playerTrans.x1) {
        xCol = true;
        xOverlap = rbBoxTrans.x1 - playerTrans.x0;
    }
    if (rbBoxTrans.x0 <= playerTrans.x0 && playerTrans.x0 <= playerTrans.x1 && playerTrans.x1 <= rbBoxTrans.x1) {
        xCol = true;
        if ((rbBoxTrans.x1 - playerTrans.x1) <= (playerTrans.x0 - rbBoxTrans.x0)) {
            xOverlap = rbBoxTrans.x1 - playerTrans.x0;
        } else {
            xOverlap = rbBoxTrans.x0 - playerTrans.x1;
        }
    }
    else if(playerTrans.x0 <= rbBoxTrans.x0 && rbBoxTrans.x0 <= playerTrans.x1 && playerTrans.x1 <= rbBoxTrans.x1) {
        xCol = true;
        xOverlap = -1 * (playerTrans.x1 - rbBoxTrans.x0);
    }
    else if(playerTrans.x0 <= rbBoxTrans.x0 && rbBoxTrans.x0 <= rbBoxTrans.x1 && rbBoxTrans.x1 <= playerTrans.x1) {
        xCol = true;
        if ((playerTrans.x1 - rbBoxTrans.x1) <= (rbBoxTrans.x0 - playerTrans.x0)) {
            xOverlap = rbBoxTrans.x0 - playerTrans.x1;
        } else {
            xOverlap = rbBoxTrans.x1 - playerTrans.x0;
        }
    }

    bool yCol = false;
    float yOverlap = 0.0f;
    if (rbBoxTrans.y0 <= playerTrans.y0 && playerTrans.y0 <= rbBoxTrans.y1 && rbBoxTrans.y1 <= playerTrans.y1) {
        yCol = true;
        yOverlap = rbBoxTrans.y1 - playerTrans.y0;
    }
    if (rbBoxTrans.y0 <= playerTrans.y0 && playerTrans.y0 <= playerTrans.y1 && playerTrans.y1 <= rbBoxTrans.y1) {
        yCol = true;
        if ((rbBoxTrans.y1 - playerTrans.y1) <= (playerTrans.y0 - rbBoxTrans.y0)) {
            yOverlap = rbBoxTrans.y1 - playerTrans.y0;
        } else {
            yOverlap = rbBoxTrans.y0 - playerTrans.y1;
        }
    }
    else if(playerTrans.y0 <= rbBoxTrans.y0 && rbBoxTrans.y0 <= playerTrans.y1 && playerTrans.y1 <= rbBoxTrans.y1) {
        yCol = true;
        yOverlap = -1 * (playerTrans.y1 - rbBoxTrans.y0);
    }
    else if(playerTrans.y0 <= rbBoxTrans.y0 && rbBoxTrans.y0 <= rbBoxTrans.y1 && rbBoxTrans.y1 <= playerTrans.y1) {
        yCol = true;
        if ((playerTrans.y1 - rbBoxTrans.y1) <= (rbBoxTrans.y0 - playerTrans.y0)) {
            yOverlap = rbBoxTrans.y0 - playerTrans.y1;
        } else {
            yOverlap = rbBoxTrans.y1 - playerTrans.y0;
        }
    }

    bool zCol = false;
    float zOverlap = 0.0f;
    if (rbBoxTrans.z0 <= playerTrans.z0 && playerTrans.z0 <= rbBoxTrans.z1 && rbBoxTrans.z1 <= playerTrans.z1) {
        zCol = true;
        zOverlap = rbBoxTrans.z1 - playerTrans.z0;
    }
    if (rbBoxTrans.z0 <= playerTrans.z0 && playerTrans.z0 <= playerTrans.z1 && playerTrans.z1 <= rbBoxTrans.z1) {
        zCol = true;
        if ((rbBoxTrans.z1 - playerTrans.z1) <= (playerTrans.z0 - rbBoxTrans.z0)) {
            zOverlap = rbBoxTrans.z1 - playerTrans.z0;
        } else {
            zOverlap = rbBoxTrans.z0 - playerTrans.z1;
        }
    }
    else if(playerTrans.z0 <= rbBoxTrans.z0 && rbBoxTrans.z0 <= playerTrans.z1 && playerTrans.z1 <= rbBoxTrans.z1) {
        zCol = true;
        zOverlap = -1 * (playerTrans.z1 - rbBoxTrans.z0);
    }
    else if(playerTrans.z0 <= rbBoxTrans.z0 && rbBoxTrans.z0 <= rbBoxTrans.z1 && rbBoxTrans.z1 <= playerTrans.z1) {
        zCol = true;
        if ((playerTrans.z1 - rbBoxTrans.z1) <= (rbBoxTrans.z0 - playerTrans.z0)) {
            zOverlap = rbBoxTrans.z0 - playerTrans.z1;
        } else {
            zOverlap = rbBoxTrans.z1 - playerTrans.z0;
        }
    }

    if (xCol && yCol && zCol) {
        float xOverlapAbs = xOverlap >= 0 ? xOverlap : -1*xOverlap;
        float yOverlapAbs = yOverlap >= 0 ? yOverlap : -1*yOverlap;
        float zOverlapAbs = zOverlap >= 0 ? zOverlap : -1*zOverlap;

        if (yOverlapAbs <= YSNAP) {
            newPos.y += yOverlap;
            yVelocity = 0.0f;
            return newPos;
        }

        if (zOverlapAbs <= xOverlapAbs && zOverlapAbs <= yOverlapAbs) {
            newPos.z += zOverlap;
        }
        else if (yOverlapAbs <= xOverlapAbs && yOverlapAbs <= zOverlapAbs) {
            newPos.y += yOverlap;
            yVelocity = 0.0f;
        }
        else {
            newPos.x += xOverlap;
        }
        return newPos;
    }
    else {
        return playerPos;
    }
}

#endif