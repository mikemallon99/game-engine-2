#ifndef GEOM_PRIMITIVES_H
#define GEOM_PRIMITIVES_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class AABB {
public:
    // These are all planes
    float x0;
    float x1;
    float y0;
    float y1;
    float z0;
    float z1;

    AABB(float x0, float x1, float y0, float y1, float z0, float z1) {
        this->x0 = x0;
        this->x1 = x1;
        this->y0 = y0;
        this->y1 = y1;
        this->z0 = z0;
        this->z1 = z1;
    }

    // Should make function here for getting xform matrix for cube vertices
    glm::mat4 GetCubeXform() {
        float x_m = (x1 - x0) / 2 + x0;
        float y_m = (y1 - y0) / 2 + y0;
        float z_m = (z1 - z0) / 2 + z0;

        // eq: l * s = n
        // l = 1 
        float x_s = x1 - x0;
        float y_s = y1 - y0;
        float z_s = z1 - z0;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x_m, y_m, z_m));
        model = glm::scale(model, glm::vec3(x_s, y_s, z_s)); // Make it a smaller cube
        return model;
    }
};

#endif