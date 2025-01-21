#ifndef COLLISIONS_H
#define COLLISIONS_H

#define GLM_ENABLE_EXPERIMENTAL

#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

float RADIUS = 0.5f;
float RADIUS_2 = RADIUS * RADIUS;

// collision object has a model associated with it
// has a player circle that can intersect it
// check each triangle in the model and see if the player circle is intersecting
// if the circle is intersecting, then push it out by the normal until the circle is tangent to the plane
class CollisionObject {
public:
    Model* model;

    CollisionObject(Model* m);
    glm::vec3 checkCollisions(glm::vec3 playerPos);
};

CollisionObject::CollisionObject(Model* m) {
    model = m;
}

static bool intersectRaySegmentSphere(glm::vec3 o, glm::vec3 d, glm::vec3 so, float radius2, glm::vec3 &ip) {
    // pass in d non normalized to keep its length
    // use the length later to compare the intersection point
    // to make sure were within the ray segment
    float l = glm::length(d);
    if (l == 0.0f) {
        return false;
    }
    d = d / l;

    glm::vec3 m = o - so;
    float b = glm::dot(m, d);
    float c = glm::dot(m, m) - radius2;

    // Exit if r's origin is outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f) {
        return false;
    }
    float discr = b*b - c;

    // A negative discriminant corresponds to ray missing sphere 
    if (discr < 0.0f) {
        return false;
    }

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - sqrtf(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.0f) {
        t = 0.0f;
    }
    ip = o + (d * t);

    // This is the last segment talked about in the video
    if (t > l) {
        return false;
    }

    return true;
}

//2D test for which side of a 2D line a 2D point lies on
static bool leftOf(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &p)
{
  //3x3 determinant (can also think of this aprojecting onto 2D lines)
  // | ax  bx  px |
  // | ay  by  py |
  // | 1   1   1  |
  
  float area = 0.5f * (a.x * (b.y - p.y) +
                       b.x * (p.y - a.y) +
                       p.x * (a.y - b.y));
  return (area > 0.0f);
}

//2D test for point inside polygon
static bool pointInside(const glm::vec2 poly[], int pcount, const glm::vec2 &v)
{
  for(int i = 0; i < pcount; i++)
  {
    int next = i;
    next++;
    if(next == pcount)
      next = 0;
    
    if(!leftOf(poly[i], poly[next], v))
      return false;
  }
  return true;
}

glm::vec3 CollisionObject::checkCollisions(glm::vec3 playerPos) {
    glm::vec3 shiftDelta;
    int numCollisions = 0;

    // Iterate through each mesh in the model
    for (int i=0; i < model->meshes.size(); i++) {
        // Iterate through each triangle in the mesh
        Mesh& mesh = model->meshes[i];
        bool collideMesh = false;

        // For each triangle
        for (int t=0; t < mesh.indices.size()/3; t++) {
            bool outsidePlane = false;
            bool outsideAllVerts = false;
            bool outsideAllEdges = false;
            bool fullyInsidePlane = false;

            glm::vec3 v0 = mesh.vertices[mesh.indices[3*t + 0]].Position;
            glm::vec3 v1 = mesh.vertices[mesh.indices[3*t + 1]].Position;
            glm::vec3 v2 = mesh.vertices[mesh.indices[3*t + 2]].Position;

            glm::vec3 normal = glm::normalize(mesh.vertices[mesh.indices[3*t + 0]].Normal);

            float d = glm::dot(-((v0 + v1 + v2) / 3.0f), normal);
            float ppd = glm::dot(normal, playerPos) + d;

            if (fabs(ppd) > RADIUS) {
                outsidePlane = true;
                continue;
            }

            glm::vec3 a = v1-v0;
            glm::vec3 b = v2-v1;
            glm::vec3 c = v0-v2;

            glm::vec3 planeX = glm::normalize(a);
            glm::vec3 planeY = glm::normalize(glm::cross(normal, a));

            auto project2D = [&](const glm::vec3 &p){ return glm::vec2(glm::dot(p, planeX), glm::dot(p, planeY)); };

            glm::vec2 planePos2D = project2D(playerPos);
            glm::vec2 triangle2D[3] = {project2D(v0), project2D(v1), project2D(v2)};

            if (pointInside(triangle2D, 3, planePos2D)) {
                fullyInsidePlane = true;
            }

            bool outsideV0 = (glm::length2(v0 - playerPos) > RADIUS_2);
            bool outsideV1 = (glm::length2(v1 - playerPos) > RADIUS_2);
            bool outsideV2 = (glm::length2(v2 - playerPos) > RADIUS_2);

            if (outsideV0 && outsideV1 && outsideV2) {
                outsideAllVerts = true;
            }

            glm::vec3 ip;

            if (!intersectRaySegmentSphere(v0, a, playerPos, RADIUS_2, ip) &&
                !intersectRaySegmentSphere(v1, b, playerPos, RADIUS_2, ip) &&
                !intersectRaySegmentSphere(v2, c, playerPos, RADIUS_2, ip)) 
            {
                outsideAllEdges = true;
            }

            if (!outsideAllVerts) {
                // std::cout << "vertex" << std::endl;
            }
            if (!outsideAllEdges) {
                // std::cout << "edge" << ip.x << ip.y << ip.z << std::endl;
            }
            if (fullyInsidePlane) {
                std::cout << "insidePlane" << std::endl;
            }
            if (!outsidePlane) {
                // std::cout << "intersectPlane: " << ppd << std::endl;
            }

            if (outsideAllVerts && outsideAllEdges && !fullyInsidePlane) {
                continue;
            }

            shiftDelta += normal * (playerPos - ppd);
            numCollisions++;
            collideMesh = true;
        }

        if (collideMesh) {
            mesh.addColor = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        else {
            mesh.addColor = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // player is intersecting the plane
        // player is intersecting an edge
        // player is inside the face

        // player is intersecting a vertex
    }

    // if (numCollisions != 0) {
    //     shiftDelta /= (float)numCollisions;
    //     if (shiftDelta.length() > lastWalkSpeed) {
    //         shiftDelta = glm::normalize(shiftDelta);
    //         shiftDelta *= lastWalkSpeed*1.1f;
    //     }
    // }

    // Set position

    // If the conditions are true
    // If player is behind the plane, move it along the normal by D + R
    // If player is in front of the plane, move it along the normal by R - D

    return shiftDelta;
}


#endif