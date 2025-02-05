#ifndef GIZMO_H
#define GIZMO_H

#include "geom_primitives.h"
#include "camera.h"
// #include "keyboard.h"

float BOX_RADIUS = 0.1;

class MoveGizmo {
public:
    bool active;
    glm::vec3 gizmoOrigin;
    glm::vec3* childOrigin;
    AABB xAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    AABB yAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    AABB zAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    bool hoverXBox;
    bool hoverYBox;
    bool hoverZBox;
    Camera* camera;

    // MoveGizmo(Camera& camera, glm::vec3& childOrigin)
    //     : camera(camera), childOrigin(childOrigin), gizmoOrigin(childOrigin) {
    //     ResetBoxes();
    // }

    void SetTranslationXform(glm::vec3 translate) {
        ResetBoxes();
        xAxisBBox = xAxisBBox.Translate(translate);
        yAxisBBox = yAxisBBox.Translate(translate);
        zAxisBBox = zAxisBBox.Translate(translate);
        gizmoOrigin = translate;
        // TODO: I stopped here. Need to refactor main.cpp so I can test out dragging along the X axis
    }

    // Input processing
    // If camera is intersecting a bbox, then that bbox is selected
    void ProcessKeyboard(GLFWwindow *window) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouseDown) {
            mouseDown = true;
            mouseDownRayDir = camera->Front;
            mouseDownRayOrigin = camera->Position;

            if (hoverXBox) {
                selXBox = true;
            }
            if (hoverYBox) {
                selYBox = true;
            }
            if (hoverZBox) {
                selZBox = true;
            }
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mouseDown) {
            if (selXBox || selYBox || selZBox) {
                gizmoOrigin = *childOrigin;
                SetTranslationXform(gizmoOrigin);
            }

            mouseDown = false;
            selXBox = false;
            selYBox = false;
            selZBox = false;
        }
    }

    void Update() {
        ProcessCollisions();
        if (selXBox || selYBox || selZBox) {
            *childOrigin = gizmoOrigin + calcTransVec();
        }
    }

private:
    bool selXBox;
    bool selYBox;
    bool selZBox;
    glm::vec3 mouseDownRayDir;
    glm::vec3 mouseDownRayOrigin;
    bool mouseDown = false;
    // collision processing
    // Check if camera ray is intersecting a bbox
    void ProcessCollisions() {
        // Check gizmo axes on wellbox
        hoverXBox = rayIntersectsAABB(camera->Position, camera->Front, xAxisBBox.GetMin(), xAxisBBox.GetMax());
        hoverYBox = rayIntersectsAABB(camera->Position, camera->Front, yAxisBBox.GetMin(), yAxisBBox.GetMax());
        hoverZBox = rayIntersectsAABB(camera->Position, camera->Front, zAxisBBox.GetMin(), zAxisBBox.GetMax());
    }

    void ResetBoxes() {
        xAxisBBox = AABB(
            0.0f, 1.5f,
            -BOX_RADIUS, BOX_RADIUS,
            -BOX_RADIUS, BOX_RADIUS
        );
        yAxisBBox = AABB(
            -BOX_RADIUS, BOX_RADIUS,
            0.0f, 1.5f,
            -BOX_RADIUS, BOX_RADIUS
        );
        zAxisBBox = AABB(
            -BOX_RADIUS, BOX_RADIUS,
            -BOX_RADIUS, BOX_RADIUS,
            0.0f, 1.5f
        );
    }

    glm::vec3 calcTransVec() {
        /*
        algo:
            when you press mouse down on an axis, we need to save the camera vector so we can project it onto the axis
            then, we take the current camera vector and then project that onto the axis too
            the difference between these 2 points is the translation vector
        */
        float t, x_i, y_i, z_i;
        glm::vec3 transVec = glm::vec3(0.0f, 0.0f, 0.0f);
        if (selXBox) {
            // Project onto plane perpendicular to you and x axis
            t = (gizmoOrigin.z - mouseDownRayOrigin.z) / mouseDownRayDir.z;
            float x1 = mouseDownRayOrigin.x + mouseDownRayDir.x * t;
            t = (gizmoOrigin.z - camera->Position.z) / camera->Front.z;
            float x2 = camera->Position.x + camera->Front.x * t;
            transVec.x = x2 - x1;
        }
        if (selYBox) {
            // Project onto plane perpendicular to you and x axis
            t = (gizmoOrigin.z - mouseDownRayOrigin.z) / mouseDownRayDir.z;
            float y1 = mouseDownRayOrigin.y + mouseDownRayDir.y * t;
            t = (gizmoOrigin.z - camera->Position.z) / camera->Front.z;
            float y2 = camera->Position.y + camera->Front.y * t;
            transVec.y = y2 - y1;
        }
        if (selZBox) {
            // Project onto plane perpendicular to you and x axis
            t = (gizmoOrigin.y - mouseDownRayOrigin.y) / mouseDownRayDir.y;
            float z1 = mouseDownRayOrigin.z + mouseDownRayDir.z * t;
            t = (gizmoOrigin.y - camera->Position.y) / camera->Front.y;
            float z2 = camera->Position.z + camera->Front.z * t;
            transVec.z = z2 - z1;
        }

        return transVec;
    }
};

#endif