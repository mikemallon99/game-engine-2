#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "geom_primitives.h"

// Update physics function
// If on floor, dont need to calculate
// If in the air, need to calculate
// Calculate collisions
// What is the floor if the ground is not level? or if cubes are placed all over the place
    // Use a bbox, calculate collisions

float YSNAP = 0.05;

// AABB rbBBox = AABB{-0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0.5f};
AABB playerBBox = AABB(-0.2f, 0.2f, -0.5f, 0.5f, -0.2f, 0.2f);

float yVelocity = 0.0f;

glm::vec3 calcGravity() {
    glm::vec3 newPos = glm::vec3(0.0f, 0.0f, 0.0f);
    yVelocity -= 0.0025f;
    newPos.y += yVelocity;
    return newPos;
}

glm::vec3 calcCollisionsAABB(glm::vec3 playerPos, glm::vec3 movVec, AABB rbBox) {
    glm::vec3 newPos = playerPos + movVec;

    // Translate playerBBox bbox
    AABB playerTrans(
        playerBBox.x0 + newPos.x, 
        playerBBox.x1 + newPos.x,
        playerBBox.y0 + newPos.y,
        playerBBox.y1 + newPos.y,
        playerBBox.z0 + newPos.z,
        playerBBox.z1 + newPos.z
    );

    // Check for overlap
    // X
    bool xCol = false;
    float xOverlap = 0.0f;
    if (rbBox.x0 <= playerTrans.x0 && playerTrans.x0 <= rbBox.x1 && rbBox.x1 <= playerTrans.x1) {
        xCol = true;
        xOverlap = rbBox.x1 - playerTrans.x0;
    }
    if (rbBox.x0 <= playerTrans.x0 && playerTrans.x0 <= playerTrans.x1 && playerTrans.x1 <= rbBox.x1) {
        xCol = true;
        if ((rbBox.x1 - playerTrans.x1) <= (playerTrans.x0 - rbBox.x0)) {
            xOverlap = rbBox.x1 - playerTrans.x0;
        } else {
            xOverlap = rbBox.x0 - playerTrans.x1;
        }
    }
    else if(playerTrans.x0 <= rbBox.x0 && rbBox.x0 <= playerTrans.x1 && playerTrans.x1 <= rbBox.x1) {
        xCol = true;
        xOverlap = -1 * (playerTrans.x1 - rbBox.x0);
    }
    else if(playerTrans.x0 <= rbBox.x0 && rbBox.x0 <= rbBox.x1 && rbBox.x1 <= playerTrans.x1) {
        xCol = true;
        if ((playerTrans.x1 - rbBox.x1) <= (rbBox.x0 - playerTrans.x0)) {
            xOverlap = rbBox.x0 - playerTrans.x1;
        } else {
            xOverlap = rbBox.x1 - playerTrans.x0;
        }
    }

    bool yCol = false;
    float yOverlap = 0.0f;
    if (rbBox.y0 <= playerTrans.y0 && playerTrans.y0 <= rbBox.y1 && rbBox.y1 <= playerTrans.y1) {
        yCol = true;
        yOverlap = rbBox.y1 - playerTrans.y0;
    }
    if (rbBox.y0 <= playerTrans.y0 && playerTrans.y0 <= playerTrans.y1 && playerTrans.y1 <= rbBox.y1) {
        yCol = true;
        if ((rbBox.y1 - playerTrans.y1) <= (playerTrans.y0 - rbBox.y0)) {
            yOverlap = rbBox.y1 - playerTrans.y0;
        } else {
            yOverlap = rbBox.y0 - playerTrans.y1;
        }
    }
    else if(playerTrans.y0 <= rbBox.y0 && rbBox.y0 <= playerTrans.y1 && playerTrans.y1 <= rbBox.y1) {
        yCol = true;
        yOverlap = -1 * (playerTrans.y1 - rbBox.y0);
    }
    else if(playerTrans.y0 <= rbBox.y0 && rbBox.y0 <= rbBox.y1 && rbBox.y1 <= playerTrans.y1) {
        yCol = true;
        if ((playerTrans.y1 - rbBox.y1) <= (rbBox.y0 - playerTrans.y0)) {
            yOverlap = rbBox.y0 - playerTrans.y1;
        } else {
            yOverlap = rbBox.y1 - playerTrans.y0;
        }
    }

    bool zCol = false;
    float zOverlap = 0.0f;
    if (rbBox.z0 <= playerTrans.z0 && playerTrans.z0 <= rbBox.z1 && rbBox.z1 <= playerTrans.z1) {
        zCol = true;
        zOverlap = rbBox.z1 - playerTrans.z0;
    }
    if (rbBox.z0 <= playerTrans.z0 && playerTrans.z0 <= playerTrans.z1 && playerTrans.z1 <= rbBox.z1) {
        zCol = true;
        if ((rbBox.z1 - playerTrans.z1) <= (playerTrans.z0 - rbBox.z0)) {
            zOverlap = rbBox.z1 - playerTrans.z0;
        } else {
            zOverlap = rbBox.z0 - playerTrans.z1;
        }
    }
    else if(playerTrans.z0 <= rbBox.z0 && rbBox.z0 <= playerTrans.z1 && playerTrans.z1 <= rbBox.z1) {
        zCol = true;
        zOverlap = -1 * (playerTrans.z1 - rbBox.z0);
    }
    else if(playerTrans.z0 <= rbBox.z0 && rbBox.z0 <= rbBox.z1 && rbBox.z1 <= playerTrans.z1) {
        zCol = true;
        if ((playerTrans.z1 - rbBox.z1) <= (rbBox.z0 - playerTrans.z0)) {
            zOverlap = rbBox.z0 - playerTrans.z1;
        } else {
            zOverlap = rbBox.z1 - playerTrans.z0;
        }
    }

    if (xCol && yCol && zCol) {
        float xOverlapAbs = xOverlap >= 0 ? xOverlap : -1*xOverlap;
        float yOverlapAbs = yOverlap >= 0 ? yOverlap : -1*yOverlap;
        float zOverlapAbs = zOverlap >= 0 ? zOverlap : -1*zOverlap;

        if (yOverlapAbs <= YSNAP) {
            newPos.y += yOverlap;
            yVelocity = 0.0f;
            return newPos - playerPos;
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
    }
    return newPos - playerPos;
}

#endif