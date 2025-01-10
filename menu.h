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

// Menu has a list of text items that have functions associated with them
// User has a current selection, and can use arrow keys to move the cursor around
// when the item on the menu is highlighted, then the color of it will change

glm::vec3 selColor = glm::vec3(0.9, 0.9f, 0.9f);
glm::vec3 defaultColor = glm::vec3(0.3, 0.7f, 0.9f);

class MenuNode {
public:
    std::vector<MenuNode*> children;
    std::string label;
    int cursorPos;
    std::function<void()> select_func;

    MenuNode(std::string name) {
        label = name;
        cursorPos = 0;
        select_func = nullptr;
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
        return label;
    }

    void Draw(Shader s) {
        glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
        s.use();
        s.setMat4("projection", projection);

        int i;
        float y_pos;
        glm::vec3 color;
        for (int i=0; i < children.size(); i++) {
            y_pos = 570.0f - i * 30.0f;
            color = (i == cursorPos) ? selColor : defaultColor;
            RenderText(s, children[i]->GetValue(), 20.0f, y_pos, 0.5f, color);
        }
    }

    virtual bool Select() {
        if (select_func) {
            select_func();
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


class DataNode : MenuNode {
public:
    int* valuePtr;

    DataNode(std::string name, int* data_ptr) : MenuNode(name) {
        valuePtr = data_ptr;
    }

    std::string GetValue() override {
        std::ostringstream oss;
        oss << label << ": " << *valuePtr;
        return oss.str();
    }
};


class DebugMenu
{
public:
    MenuNode* root;
    MenuNode* textureMenu;
    MenuNode* editMenu;

    int intValueX;
    int intValueY;
    int intValueZ;

    std::vector<MenuNode*> menuNodeStack;
    KeyboardDebouncer kbd;

    DebugMenu();
    ~DebugMenu();
    void Draw(Shader s);
    void ProcessKeyboard(GLFWwindow *window);

private:
    std::vector<MenuNode*> allMenuNodes;
};

// Debug menu holds values for all the stuff
// how to handle submenus?
//      menu page, menu page filled with entries, entries can do a function or change active menu page, also push to a stack
//      what to do if i want to do stuff like have incrementing, sublists, etc?

DebugMenu::DebugMenu() {
    kbdMgr.registerKeyboard(&kbd);

    // Root menu
    root = new MenuNode("root");
    menuNodeStack.push_back(root);

    textureMenu = new MenuNode("texture");
    textureMenu->Add(new MenuNode("bathroom_tiles"));
    textureMenu->Add(new MenuNode("container"));
    textureMenu->Add(new MenuNode("bricks"));

    editMenu = new MenuNode("edit face");
    MenuNode* editMenuX = editMenu->Add(new MenuNode("X"));
    MenuNode* editMenuY = editMenu->Add(new MenuNode("Y"));
    MenuNode* editMenuZ = editMenu->Add(new MenuNode("Z"));

    editMenuX->Add((MenuNode*)(new DataNode("X value", &intValueX)));
    MenuNode* intIncX = editMenuX->Add(new MenuNode("+1"));
    intIncX->select_func = [this]() {
        (this->intValueX)++;
    };
    MenuNode* intDecX = editMenuX->Add(new MenuNode("-1"));
    intDecX->select_func = [this]() {
        (this->intValueX)--;
    };

    editMenuY->Add((MenuNode*)(new DataNode("Y value", &intValueY)));
    MenuNode* intIncY = editMenuY->Add(new MenuNode("+1"));
    intIncY->select_func = [this]() {
        (this->intValueY)++;
    };
    MenuNode* intDecY = editMenuY->Add(new MenuNode("-1"));
    intDecY->select_func = [this]() {
        (this->intValueY)--;
    };

    editMenuZ->Add((MenuNode*)(new DataNode("Z value", &intValueZ)));
    MenuNode* intIncZ = editMenuZ->Add(new MenuNode("+1"));
    intIncZ->select_func = [this]() {
        (this->intValueZ)++;
    };
    MenuNode* intDecZ = editMenuZ->Add(new MenuNode("-1"));
    intDecZ->select_func = [this]() {
        (this->intValueZ)--;
    };

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
            !menuNodeStack.back()->getSelectedChild()->Select() && menuNodeStack.back()->selectedHasChildren()
        ) {
            menuNodeStack.push_back(menuNodeStack.back()->getSelectedChild());
        }
    }
}


#endif