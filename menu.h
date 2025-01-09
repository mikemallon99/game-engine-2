#ifndef MENU_H
#define MENU_H

#include <string>
#include <functional>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory> // For smart pointers

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

enum PageName {
    MAIN_PAGE,
    TEXTURE_SELECT,
    EDIT_FACE,
    EDIT_FACE_VERTS,
};

class MenuItem
{
public:
    std::string displayString;
    virtual void Select() {
    }
    virtual void Deselect() {
    }
};

class StringItem : MenuItem
{
public:
    std::string gotoPageID;
    std::string itemName;
    std::function<void(std::string)> open_callback;

    StringItem(std::string name) {
        itemName = name;
        displayString = itemName;
    }

    void selectPageID(std::string selectID) {
        gotoPageID = selectID;
    }

    void Select() override {
        open_callback(gotoPageID);
    }
};

class FloatItem : MenuItem
{
public:
    std::string valueName;
    float value;
    float delta;
    int selected;

    FloatItem(std::string in_name, float in_value, float in_delta) {
        valueName = in_name;
        value = in_value;
        delta = in_delta;
    }

    void updateString() {
        std::ostringstream oss; 
        oss << valueName << ": " << value;
        displayString = oss.str();
    }

    void ProcessKeyboard(KeyboardDebouncer& kbd) {
        if (kbd.checkKey(GLFW_KEY_UP)) {
            value += delta;
        }
        if (kbd.checkKey(GLFW_KEY_DOWN)) {
            value -= delta;
        }
        if (kbd.checkKey(GLFW_KEY_LEFT)) {
            selected = 0;
        }

        updateString();
    }
};

class MenuPage
{
public:
    int cursorPos;
    std::string ID;
    std::vector<std::unique_ptr<MenuItem>> menuItems;
    std::function<void()> close_callback;

    void Select() {
        menuItems[cursorPos].Select();
    }

    void Deselect() {
        close_callback();
    }

    void setPageID(std::string newID) {
        ID = newID;
    }

    void Up() {
        if (cursorPos < menuItems.size() - 1) {
            cursorPos++;
        }
    }

    void Down() {
        if (cursorPos > 0) {
            cursorPos--;
        }
    }

    std::unique_ptr<StringItem> addStringItem(std::string itemName) {
        std::unique_ptr<StringItem> strItemPtr = std::make_unique<StringItem>(itemName);
        menuItems.push_back(strItemPtr);
        return (std::unique_ptr<StringItem>)menuItems.back();
    }

    virtual void draw(Shader s) {
        for (int i=0; i < menuItems.size(); i++) {
            float y_pos = 570.0f - i * 30.0f;
            glm::vec3 color = (i == cursorPos) ? selColor : defaultColor;
            RenderText(s, menuItems[i]->displayString, 20.0f, y_pos, 0.5f, color);
        }
    }
};

class Vec3MenuPage : MenuPage
{
public:
    glm::vec3 data;

    // VertexEditMenuPage() {
    //     vertex_select = 0;
    //     vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
    //     vertices.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
    //     vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    //     vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
    // }
};

class DebugMenu
{
public:
    std::vector<MenuPage*> pageStack;
    std::vector<MenuPage> allPages;

    int currentPage = MAIN_PAGE;

    int mainMenuCursor = 0;

    int textureMenuCursor = 0;

    int editFaceMenuCursor = 0;
    int editFaceVertexMenuCursor = 0;
    int editFaceTranslateMenuCursor = 0;
    int editFaceScaleMenuCursor = 0;

    KeyboardDebouncer kbd;

    DebugMenu();
    void Draw(Shader s);
    void ProcessKeyboard(GLFWwindow *window);

    void open_menu_callback(std::string pageID) {
        for (int i=0; i < allPages.size(); i++) {
            if (allPages[i].ID == pageID) {
                pageStack.push_back(&(allPages[i]));
            }
        }
    }

    void close_menu_callback() {
        if (pageStack.size() > 1) {
            pageStack.pop_back();
        }
    }
};

// Debug menu holds values for all the stuff
// how to handle submenus?
//      menu page, menu page filled with entries, entries can do a function or change active menu page, also push to a stack
//      what to do if i want to do stuff like have incrementing, sublists, etc?

DebugMenu::DebugMenu() {
    kbdMgr.registerKeyboard(&kbd);
    // Add root page to the page stack
    allPages.emplace_back();
    MenuPage& mainPage = allPages.back();
    pageStack.push_back(&(allPages.back()));
    auto texSelectItem = mainPage.addStringItem("texture_select");
    texSelectItem->selectPageID("texture_select");
    texSelectItem->open_callback = [this](std::string pageID) {
        this->open_menu_callback(pageID);
    };

    allPages.emplace_back();
    MenuPage& textureSelectPage = allPages.back();
    // Need a way to retrieve a page from its ID
    textureSelectPage.close_callback = [this]() {
        this->close_menu_callback();
    };
    textureSelectPage.setPageID("texture_select");
    textureSelectPage.addStringItem("bathroom_tile");
    textureSelectPage.addStringItem("container");
    textureSelectPage.addStringItem("bricks");

    // MenuPage faceEditPage;
    // // Need a way to retrieve a page from its ID
    // faceEditPage.setPageID("face_edit");
    // vertexEditPage.addStringItem("edit vertices");
    // translateEditPage.addStringItem("edit translate");
    // scaleEditPage.addStringItem("edit scale");

    // MenuPage verticesEditPage;
    // verticesEditPage.setPageID("edit_face_vertices");
    // verticesEditPage.addIntValueItem("index", 0, 1);
    // verticesEditPage.addFloatValueItem("x");
    // verticesEditPage.addFloatValueItem("y");
    // verticesEditPage.addFloatValueItem("z");
};

void DebugMenu::Draw(Shader s) {
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    s.use();
    s.setMat4("projection", projection);

    pageStack.back()->draw(s);
}

void DebugMenu::ProcessKeyboard(GLFWwindow *window)
{
    // normally if you hit left it should go back
    // if you have a float item selected though, then it should just deselect
    if (kbd.checkKey(GLFW_KEY_UP)) {
        pageStack.back()->Up();
    }
    if (kbd.checkKey(GLFW_KEY_DOWN)) {
        pageStack.back()->Down();
    }
    if (kbd.checkKey(GLFW_KEY_LEFT)) {
        pageStack.back()->Deselect();
    }
    // If you select an item and its a link to a new page, then we will have to somehow activate the new page
    if (kbd.checkKey(GLFW_KEY_RIGHT)) {
        pageStack.back()->Select();
    }
}


#endif