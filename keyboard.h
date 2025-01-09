#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <glad/glad.h>
#include <string>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>


class KeyboardDebouncer {
public:
    std::map<int, int> keyDebounce;

    KeyboardDebouncer();
    int checkKey(int key);
};

class KeyboardDebouncerManager {
public:
    std::vector<KeyboardDebouncer*> keyDebounceList;
    KeyboardDebouncer myDebouncer;

    void updateWithInput(GLFWwindow *window);
    void registerKeyboard(KeyboardDebouncer* kbd);
};

// Declare this here so it can be used whenever a system needs a kbd manager
KeyboardDebouncerManager kbdMgr;


KeyboardDebouncer::KeyboardDebouncer() {
    for (int i=0; i<349; i++) {
        keyDebounce[i] = 0;
    } 
}

int KeyboardDebouncer::checkKey(int key) {
    if (keyDebounce[key]) {
        keyDebounce[key] = 0;
        return 1;
    }
    else {
        return 0;
    }
}

void KeyboardDebouncerManager::updateWithInput(GLFWwindow *window) {
    for (int i=0; i<349; i++) {
        if(glfwGetKey(window, i) == GLFW_PRESS && !(myDebouncer.keyDebounce[i])) {
            myDebouncer.keyDebounce[i] = 1;
            for(int j=0; j < keyDebounceList.size(); j++) {
                keyDebounceList[j]->keyDebounce[i] = 1;
            }
        }
        if(glfwGetKey(window, i) == GLFW_RELEASE) {
            myDebouncer.keyDebounce[i] = 0;
            for(int j=0; j < keyDebounceList.size(); j++) {
                keyDebounceList[j]->keyDebounce[i] = 0;
            }
        }
    } 
}

void KeyboardDebouncerManager::registerKeyboard(KeyboardDebouncer* kbd) {
    keyDebounceList.push_back(kbd);
}

// debounce means that once we accept the value of the input, we dont want to test it again
// problem is that multiple systems might want to check the value of the input
// each system gets its own keyboard view which has its own debounce value

// manager understands when a key was pressed and when it was released
// when a key is pressed, put that its pressed down in each listener
// when a key is released, remove that info from each listener
// when a listener reads the input, then set that info to 0

#endif