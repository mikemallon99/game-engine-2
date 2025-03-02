#pragma once

#include <vector>
#include <array>
#include <string>
#include <stdio.h>

#include "../shader.h"
#include "../text.h"
#include "../camera.h"

const int HEIGHT = 10;
const int WIDTH = 60;

// GUI can be written to just using a logger.write(string)
// Like a regular chatlog, it starts at the bottom and then scrolls up
// Log is just a vector (can be written out to a file as well)
// Write to cout as well
// Gui displays whatever fits into a box
// Should log shader be fed from somewhere else? or should it have its own? who provides the shaders?

class Logger {
public:
    std::vector<std::string> logHistory;

    void Draw(Shader s);
    void Write(std::string output);
};

void Logger::Write(std::string output) {
    std::cout << output << std::endl;
    logHistory.push_back(output);
}

void Logger::Draw(Shader s) {
    // Imagine a box in the bottom left corner of the screen. This is where the logs are written
    // Can be toggled on/off with '`'
    // Draw from bottom up, stop when screen exceeds width & height
    // Text needs to scroll when the string is longer than column limit
    // We can create a buffer of size char[HEIGHT][WIDTH] and then fill it up backwards?
        // cant fill up backwards cuz what if you need white space at the end of the string
    // Algo: have big buffer, write the last HEIGHT num of strings to it. Then when drawing, take a buffer of size [HEIGHT][WIDTH] from that

    std::vector<std::array<char, WIDTH>> big_buffer;

    // Declaring like this sets all values to 0 initially
    int startIdx = logHistory.size()-10;
    if (startIdx < 0) {
        startIdx = 0;
    }
    for (int i=startIdx; i < logHistory.size(); i++) {
        std::string curString = logHistory[i];
        std::array<char, WIDTH> newBuffer = {};
        int curCol = 0;
        for (int c=0; c < curString.length(); c++) {
            newBuffer[curCol] = curString[c];
            curCol++;
            // Start a new buffer once its filled up
            if (curCol >= WIDTH) {
                big_buffer.push_back(newBuffer);
                newBuffer.fill(0);
            }
        }
        // Push back the buffer if its not empty
        if (newBuffer[0] != 0) {
            big_buffer.push_back(newBuffer);
        }
    }

    std::array<std::array<char, WIDTH>, HEIGHT> draw_buffer = {};
    // Grab HEIGHT num of rows from big buffer
    int bufferIdx = big_buffer.size()-1;
    for (int i=HEIGHT-1; i >= 0; i--) {
        if (bufferIdx < 0) {
            break;
        }
        draw_buffer[i] = big_buffer[bufferIdx];
        bufferIdx--;
    }

    // Now take all the chars from this draw buffer and draw them onto the screen
    // need font height
    glm::mat4 projection = glm::ortho(0.0f, float(SCREEN_WIDTH), 0.0f, float(SCREEN_HEIGHT));
    s.use();
    s.setMat4("projection", projection);

    int maxYHeight = calcMaxYHeight();
    // int y_start = SCREEN_HEIGHT - maxYHeight * HEIGHT;
    float spacer = maxYHeight / 2;
    float margin = 20.0f;
    float y_start = spacer * 10;
    for (int i=0; i < HEIGHT; i++) {
        // Need to convert buffer into string for output
        std::string outStr(draw_buffer[i].data());
        RenderText(s, outStr, margin, margin + y_start - i*spacer, 0.5f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

Logger logGui;