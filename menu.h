#ifndef MENU_H
#define MENU_H

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

enum PageName {
    MAIN_PAGE,
    TEXTURE_SELECT,
    EDIT_FACE
};

class DebugMenu
{
public:
    int currentPage = MAIN_PAGE;

    int mainMenuCursor = 0;

    int textureMenuCursor = 0;

    KeyboardDebouncer kbd;

    glm::vec3 selColor = glm::vec3(0.9, 0.9f, 0.9f);
    glm::vec3 defaultColor = glm::vec3(0.3, 0.7f, 0.9f);

    DebugMenu();
    void Draw(Shader s);
    void ProcessKeyboard(GLFWwindow *window);
};

// Debug menu holds values for all the stuff
// how to handle submenus?
//      menu page, menu page filled with entries, entries can do a function or change active menu page, also push to a stack
//      what to do if i want to do stuff like have incrementing, sublists, etc?

DebugMenu::DebugMenu() {
    kbdMgr.registerKeyboard(&kbd);
}

void DebugMenu::Draw(Shader s) {
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    s.use();
    s.setMat4("projection", projection);

    int i;
    float y_pos;
    glm::vec3 color;
    if (currentPage == MAIN_PAGE) {
        i = 0;
        y_pos = 570.0f - i * 20.0f;
        color = (i == mainMenuCursor) ? selColor : defaultColor;
        RenderText(s, "texture_select", 20.0f, y_pos, 0.5f, color);

        i = 1;
        y_pos = 570.0f - i * 20.0f;
        color = (i == mainMenuCursor) ? selColor : defaultColor;
        RenderText(s, "face_edit", 20.0f, y_pos, 0.5f, color);
    }
    else if (currentPage == TEXTURE_SELECT) {
        i = 0;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "bathroom_tile", 20.0f, y_pos, 0.5f, color);

        i = 1;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "container", 20.0f, y_pos, 0.5f, color);

        i = 2;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "bricks", 20.0f, y_pos, 0.5f, color);
    }
    else if (currentPage == EDIT_FACE) {
        i = 0;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "bathroom_tile", 20.0f, y_pos, 0.5f, color);

        i = 1;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "container", 20.0f, y_pos, 0.5f, color);

        i = 2;
        y_pos = 570.0f - i * 20.0f;
        color = (i == textureMenuCursor) ? selColor : defaultColor;
        RenderText(s, "bricks", 20.0f, y_pos, 0.5f, color);
    }

    // for (int i=0; i < menuItems.size(); i++) {
    //     float y_pos = 570.0f - i * 20.0f;
    //     glm::vec3 color = (i == itemSelected) ? selColor : defaultColor;
    //     RenderText(s, menuItems[i], 20.0f, y_pos, 0.5f, color);
    // }
}

void DebugMenu::ProcessKeyboard(GLFWwindow *window)
{
    if (kbd.checkKey(GLFW_KEY_UP)) {
        if (currentPage == MAIN_PAGE) {
            if (mainMenuCursor > 0) {
                mainMenuCursor--;
            }
        }
        else if (currentPage == TEXTURE_SELECT) {
            if (textureMenuCursor > 0) {
                textureMenuCursor--;
            }
        }
        else if (currentPage == EDIT_FACE) {
            if (textureMenuCursor > 0) {
                textureMenuCursor--;
            }
        }
    }

    if (kbd.checkKey(GLFW_KEY_DOWN)) {
        if (currentPage == MAIN_PAGE) {
            if (mainMenuCursor < 1) {
                mainMenuCursor++;
            }
        }
        else if (currentPage == TEXTURE_SELECT) {
            if (textureMenuCursor < 2) {
                textureMenuCursor++;
            }
        }
        else if (currentPage == EDIT_FACE) {
            if (textureMenuCursor < 2) {
                textureMenuCursor++;
            }
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_LEFT)) {
        if (currentPage == TEXTURE_SELECT) {
            currentPage = MAIN_PAGE;
        }
    }
    
    if (kbd.checkKey(GLFW_KEY_RIGHT)) {
        if (currentPage == MAIN_PAGE) {
            if (mainMenuCursor == 0) {
                currentPage = TEXTURE_SELECT;
            }
            else if (mainMenuCursor == 1) {
                currentPage = EDIT_FACE;
            }
        }
    }
}


#endif