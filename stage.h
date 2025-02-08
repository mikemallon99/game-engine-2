#ifndef STAGE_H
#define STAGE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "face.h"
#include "model.h"
#include "camera.h"
#include "gizmo.h"
#include "cube.h"

class Stage {
public:
    std::vector<Face*> faceVector;
    std::vector<glm::vec3> cubePosVector;

    std::vector<Model*> stageModels;
    std::vector<AABB> modelBBoxes;
    std::vector<bool> modelHoverStatus;
    MoveGizmo moveGizmo;
    // Will be -1 if nothings selected
    int selectedModel;

    Stage();
    ~Stage();

    void init(Camera* camera);
    void AddModel(Model* model);
    void Draw(Shader modelShader, Shader wireShader);
    void ProcessKeyboard(GLFWwindow *window);
    void Update();

private:
    bool mouseDown = false;
    Camera* camera;

    void checkModelRayCollisions();
};

Stage::Stage() {
    selectedModel = -1;
}

void Stage::Draw(Shader modelShader, Shader wireShader) {
    for (int i=0; i < stageModels.size(); i++) {
        modelShader.use();
        modelShader.setMat4("model", stageModels[i]->GetModelMatrix());
        stageModels[i]->Draw(modelShader);

        wireShader.use();
        glBindVertexArray(cubeVAO); 
        glm::vec3 bboxColor = glm::vec3(1.0f, 0.0f, 0.0f);
        if (modelHoverStatus[i]) {
            bboxColor = glm::vec3(1.0f, 1.0f, 0.0f);
        }
        wireShader.setVec3("wireframeColor", bboxColor);
        wireShader.setMat4("model", modelBBoxes[i].GetCubeXform());
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    if (moveGizmo.active) {
        moveGizmo.Draw();
        // Draw bboxes for axes if you want here
    }
}

void Stage::init(Camera* camera) {
    faceVector.push_back(new Face());
    this->camera = camera;
    moveGizmo.camera = this->camera;
}

void Stage::Update() {
    checkModelRayCollisions();

    // Update gizmo if active
    if (moveGizmo.active) {
        moveGizmo.Update();
    }
}

void Stage::AddModel(Model* model) {
    stageModels.push_back(model);
    modelBBoxes.push_back(model->GetBoundingBox());
    modelHoverStatus.push_back(false);
}

void Stage::checkModelRayCollisions() {
    for (int i=0; i < stageModels.size(); i++) {
        modelHoverStatus[i] = rayIntersectsAABB(this->camera->Position, this->camera->Front, modelBBoxes[i].GetMin(), modelBBoxes[i].GetMax());
    }
}

void Stage::ProcessKeyboard(GLFWwindow *window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouseDown) {
        mouseDown = true;

        // Eventually we will have to swap this out for some kind of grouping system
        if (moveGizmo.hoverXBox || moveGizmo.hoverYBox || moveGizmo.hoverZBox) {
            // Do nothin
        }
        else {
            selectedModel = -1;
            for (int i=0; i < stageModels.size(); i++) {
                if (modelHoverStatus[i]) {
                    selectedModel = i;
                    // Once a model is selected, add a gizmo to it
                    // gizmo setup
                    moveGizmo.childOrigin = &(stageModels[i]->origin);
                    moveGizmo.SetTranslationXform(stageModels[i]->origin);
                    moveGizmo.active = true;
                    break;
                }
            }
            // If model is deselected, deactivate the gizmo
            if (selectedModel == -1) {
                moveGizmo.active = false;
            }
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mouseDown) {
        mouseDown = false;
        if (selectedModel != -1) {
            modelBBoxes[selectedModel] = stageModels[selectedModel]->GetBoundingBox();
        }
    }

    if (moveGizmo.active) {
        moveGizmo.ProcessKeyboard(window);
    }
}

Stage::~Stage() {
    for (int i=0; i < faceVector.size(); i++) {
        delete faceVector[i];
    }
}

#endif