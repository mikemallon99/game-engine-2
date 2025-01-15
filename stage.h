#ifndef STAGE_H
#define STAGE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "face.h"

class Stage {
public:
    std::vector<Face*> faceVector;
    std::vector<glm::vec3> cubePosVector;

    Stage();
    ~Stage();

    void init();
};

Stage::Stage() {
}

void Stage::init() {
    faceVector.push_back(new Face());
}

Stage::~Stage() {
    for (int i=0; i < faceVector.size(); i++) {
        delete faceVector[i];
    }
}

#endif