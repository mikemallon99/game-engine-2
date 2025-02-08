#ifndef GIZMO_H
#define GIZMO_H

#include "geom_primitives.h"
#include "camera.h"
#include "shader.h"
// #include "keyboard.h"

float BOX_RADIUS = 0.1;

class MoveGizmo {
public:
    bool active = false;
    glm::vec3 gizmoOrigin;
    glm::vec3* childOrigin;
    AABB xAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    AABB yAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    AABB zAxisBBox = AABB(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    bool hoverXBox;
    bool hoverYBox;
    bool hoverZBox;
    Camera* camera;
    Shader gizmoShader;
    unsigned int originVBO, originVAO;

    MoveGizmo() : gizmoShader("shaders/movegizmo.vs", "shaders/movegizmo.gs", "shaders/movegizmo.fs") {
        float originVertex[] = {
            0.0f, 0.0f, 0.0f
        };
        glGenVertexArrays(1, &originVAO);
        glGenBuffers(1, &originVBO);
        glBindVertexArray(originVAO);

        glBindBuffer(GL_ARRAY_BUFFER, originVAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(originVertex), originVertex, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Draw() {
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();

        // this draws the gizmo
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(originVAO); 
        gizmoShader.use();
        // Dont scale & rotate the gizmo, just get the translate
        glm::mat4 gizmoMat = glm::mat4(1.0f); // Identity matrix
        gizmoMat = glm::translate(gizmoMat, gizmoOrigin); // Copy only the translation column
        gizmoShader.setMat4("model", gizmoMat);
        gizmoShader.setMat4("projection", projection);
        gizmoShader.setMat4("view", view);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, 1);
        glEnable(GL_DEPTH_TEST);
    }

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
            // This method will translate the origin based off the projection of the cameras ray onto the selected axis
            // A better method might be to transform the thing based off the delta of the cameras angle
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