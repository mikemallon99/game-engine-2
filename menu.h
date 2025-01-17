#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "text.h"
#include "keyboard.h"
#include "stage.h"

// Menu has a list of text items that have functions associated with them
// User has a current selection, and can use arrow keys to move the cursor around
// when the item on the menu is highlighted, then the color of it will change

glm::vec3 selColor = glm::vec3(0.9, 0.9f, 0.9f);
glm::vec3 defaultColor = glm::vec3(0.3, 0.7f, 0.9f);

// class Vec3VectorView {
// public:
//     std::vector<glm::vec3>* vectorPtr;
//     std::vector<MenuNode*>* menuStackPtr;
//     int cursorPos;

//     // Variable for Vec3 type
//     // When selected a value, spawn a new view with the vec3 value and push it to the stack

//     // Pointer to vector

//     // How to get select to push something to the stack?
//     void Select() {
//         Vec3View* newVec3View = new Vec3View(&((*vectorPtr)[cursorPos]));
//         menuStackPtr->push_back();
//     }
// };

// class Vec3View {
// public:
//     // Pointer to Vec3 value
// };

enum MenuNodeType {
    STRING,
    VALUE,
    EDITABLE_VALUE,
    FACE_VECTOR_VIEW,
    FACE_VIEW,
};

class MenuNode {
public:
    std::vector<MenuNode*> children;
    std::string label;
    float* valuePtr;
    std::vector<Face*>* faceVectorPtr;
    Face* facePtr;
    float delta;
    int cursorPos;
    MenuNodeType menuType;
    std::function<void(MenuNode*)> select_func;
    std::function<void()> plus_func;
    std::function<void()> minus_func;

    MenuNode* makeVec3Menu(std::string name, glm::vec3* in_data, float delta);
    MenuNode* makeVertexVec3Menu(std::string name, Face* facePtr_in, int idx, float delta);


    MenuNode(std::string name) {
        label = name;
        cursorPos = 0;
        menuType = STRING;
        select_func = nullptr;
        valuePtr = nullptr;
    }

    MenuNode(std::string name, float* data_ptr) {
        label = name;
        cursorPos = 0;
        menuType = VALUE;
        select_func = nullptr;
        valuePtr = data_ptr;
    }

    MenuNode(std::string name, float* data_ptr, float delta_in) {
        label = name;
        delta = delta_in;
        cursorPos = 0;
        menuType = EDITABLE_VALUE;
        select_func = nullptr;
        valuePtr = data_ptr;
        plus_func = [this]() {
            *(this->valuePtr) += this->delta;
        };
        minus_func = [this]() {
            *(this->valuePtr) -= this->delta;
        };
    }

    // For making a FaceVectorView
    MenuNode(std::string name, std::vector<Face*>* faceVectorPtr_in, std::function<void(MenuNode*)> select_func_in) {
        label = name;
        cursorPos = 0;
        menuType = FACE_VECTOR_VIEW;
        select_func = select_func_in;
        faceVectorPtr = faceVectorPtr_in;
        plus_func = [this]() {
            (this->faceVectorPtr)->push_back(new Face());
        };
        // minus_func = [this]() {
        //     (this->faceVectorPtr)->pop(new Face());
        // };
    }

    // For making a FaceView
    MenuNode(std::string name, Face* facePtr_in) {
        label = name;
        cursorPos = 0;
        menuType = FACE_VIEW;
        select_func = nullptr;
        facePtr = facePtr_in;
        this->Add(makeVec3Menu("translate", &(facePtr->translate), 1.0f));
        this->Add(makeVec3Menu("scale", &(facePtr->scale), 0.1f));

        // Need way to update VBO once we change a vertex
        // Get vertex from face, change, writeback
        this->Add(makeVertexVec3Menu("v1", facePtr, 0, 0.1f));
        this->Add(makeVertexVec3Menu("v2", facePtr, 1, 0.1f));
        this->Add(makeVertexVec3Menu("v3", facePtr, 2, 0.1f));
        this->Add(makeVertexVec3Menu("v4", facePtr, 3, 0.1f));
    }

    ~MenuNode() {
        for (int i=0; i < children.size(); i++) {
            delete children[i];
        }
    }

    MenuNode* Add(MenuNode* newNode) {
        children.push_back(newNode);
        return newNode;
    }

    virtual std::string GetValue() {
        if (menuType == STRING || menuType == FACE_VECTOR_VIEW) {
            return label;
        }
        else if (menuType == VALUE) {
            std::ostringstream oss;
            oss << label << ": " << *valuePtr;
            return oss.str();
        }
        else if (menuType == EDITABLE_VALUE) {
            std::ostringstream oss;
            oss << label << ": " << *valuePtr << " (+/-" << delta << ")";
            return oss.str();
        }
    }

    void Draw(Shader s) {
        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
        s.use();
        s.setMat4("projection", projection);

        int i;
        float y_pos;
        glm::vec3 color;
        if (menuType == FACE_VECTOR_VIEW) {
            // Do this differently since the children dont exist
            for (int i=0; i < faceVectorPtr->size(); i++) {
                y_pos = 570.0f - i * 30.0f;
                color = (i == cursorPos) ? selColor : defaultColor;
                std::ostringstream oss;
                oss << "face" << i;
                RenderText(s, oss.str(), 20.0f, y_pos, 0.5f, color);
            }
        } else {
            for (int i=0; i < children.size(); i++) {
                y_pos = 570.0f - i * 30.0f;
                color = (i == cursorPos) ? selColor : defaultColor;
                RenderText(s, children[i]->GetValue(), 20.0f, y_pos, 0.5f, color);
            }
        } 
    }

    bool Select() {
        if (menuType == FACE_VECTOR_VIEW) {
            std::ostringstream oss;
            oss << "face" << cursorPos;
            select_func(new MenuNode(oss.str(), faceVectorPtr->at(cursorPos)));
            return true;
        }
        else {
            return false;
        }
    }

    bool Plus() {
        if (plus_func) {
            plus_func();
            return true;
        }
        return false;
    }

    bool Minus() {
        if (minus_func) {
            minus_func();
            return true;
        }
        return false;
    }

    MenuNode* getSelectedChild() {
        return children[cursorPos];
    }

    bool selectedHasChildren() {
        if (
            // has children
            cursorPos < children.size() && 
            // children have children
            children[cursorPos]->children.size() > 0
        ) {
            return true;
        }
        else {
            return false;
        }
    }
};


class DebugMenu
{
public:
    MenuNode* root;
    MenuNode* textureMenu;
    MenuNode* editMenu;

    Stage* stage;

    std::vector<MenuNode*> menuNodeStack;
    KeyboardDebouncer kbd;

    // DebugMenu();
    ~DebugMenu();
    void Draw(Shader s);
    void ProcessKeyboard(GLFWwindow *window);
    void Setup(Stage* stage_in);

private:
    std::vector<MenuNode*> allMenuNodes;
};

// Debug menu holds values for all the stuff
// how to handle submenus?
//      menu page, menu page filled with entries, entries can do a function or change active menu page, also push to a stack
//      what to do if i want to do stuff like have incrementing, sublists, etc?

void DebugMenu::Setup(Stage* stage_in) {
    stage = stage_in;

    kbdMgr.registerKeyboard(&kbd);

    stage->faceVector[0]->translate = glm::vec3(3.0f, 0.0f, 3.0f);
    stage->faceVector[0]->scale = glm::vec3(1.0f, 1.0f, 1.0f);

    // Root menu
    root = new MenuNode("root");
    menuNodeStack.push_back(root);

    textureMenu = new MenuNode("texture");
    textureMenu->Add(new MenuNode("bathroom_tiles"));
    textureMenu->Add(new MenuNode("container"));
    textureMenu->Add(new MenuNode("bricks"));

    // editMenu = new MenuNode("edit_face");
    // editMenu->Add(makeVec3Menu("translate", &(stage->faceVector[0]->translate), 1.0f));
    // editMenu->Add(makeVec3Menu("scale", &(stage->faceVector[0]->scale), 0.1f));

    std::function<void(MenuNode*)> sel_func = [this](MenuNode* newNode) {
        (this->menuNodeStack).push_back(newNode);
    };
    editMenu = new MenuNode("edit_faces", &(stage->faceVector), sel_func);

    // Add "File" and "Edit" menus to the root
    root->Add(textureMenu);
    root->Add(editMenu);
}

DebugMenu::~DebugMenu() {
    delete root;
}

void DebugMenu::Draw(Shader s) {
    menuNodeStack.back()->Draw(s);
}

void DebugMenu::ProcessKeyboard(GLFWwindow *window)
{
    if (kbd.checkKey(GLFW_KEY_UP)) {
        // Go up on current menu node
        if (menuNodeStack.back()->cursorPos > 0) {
            (menuNodeStack.back()->cursorPos)--;
        }
    }

    if (kbd.checkKey(GLFW_KEY_DOWN)) {
        // Go down on current menu node
        if (menuNodeStack.back()->cursorPos < menuNodeStack.back()->children.size() - 1) {
            (menuNodeStack.back()->cursorPos)++;
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_LEFT)) {
        // Pop menu node stack
        if (menuNodeStack.size() > 1) {
            menuNodeStack.pop_back();
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_RIGHT)) {
        // Select menu node
        if (
            menuNodeStack.back()->menuType == FACE_VECTOR_VIEW
        ) {
            menuNodeStack.back()->Select();
        }
        else {
            menuNodeStack.push_back(menuNodeStack.back()->getSelectedChild());
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_EQUAL)) {
        // Select menu node
        if (
            menuNodeStack.back()->menuType == FACE_VECTOR_VIEW
        ) {
            menuNodeStack.back()->Plus();
        } else {
            menuNodeStack.back()->getSelectedChild()->Plus();
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_MINUS)) {
        // Select menu node
        menuNodeStack.back()->getSelectedChild()->Minus();
    }
}


// RANDOM FUNCTIONS

MenuNode* MenuNode::makeVec3Menu(std::string name, glm::vec3* in_data, float delta) {
    MenuNode* newMenu = new MenuNode(name);
    newMenu->Add(new MenuNode("X value", &(in_data->x), delta));
    newMenu->Add(new MenuNode("Y value", &(in_data->y), delta));
    newMenu->Add(new MenuNode("Z value", &(in_data->z), delta));
    return newMenu;
}

MenuNode* MenuNode::makeVertexVec3Menu(std::string name, Face* facePtr_in, int idx, float delta) {
    MenuNode* newMenu = new MenuNode(name);

    MenuNode* xMenu = new MenuNode("X value", &(facePtr_in->vertices[8*idx + 0]), delta);
    newMenu->Add(xMenu);
    xMenu->plus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.x += delta;
        facePtr_in->editVertex(idx, vtx);
    };
    xMenu->minus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.x -= delta;
        facePtr_in->editVertex(idx, vtx);
    };

    MenuNode* yMenu = new MenuNode("Y value", &(facePtr_in->vertices[8*idx + 1]), delta);
    newMenu->Add(yMenu);
    yMenu->plus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.y += delta;
        facePtr_in->editVertex(idx, vtx);
    };
    yMenu->minus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.y -= delta;
        facePtr_in->editVertex(idx, vtx);
    };

    MenuNode* zMenu = new MenuNode("Z value", &(facePtr_in->vertices[8*idx + 2]), delta);
    newMenu->Add(zMenu);
    zMenu->plus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.z += delta;
        facePtr_in->editVertex(idx, vtx);
    };
    zMenu->minus_func = [facePtr_in, idx, delta]() {
        glm::vec3 vtx = facePtr_in->getVertex(idx);
        vtx.z -= delta;
        facePtr_in->editVertex(idx, vtx);
    };

    return newMenu;
}


#endif