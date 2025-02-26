#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "model.h"

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
            unsigned char* rgbData = new unsigned char[width * height * 3]; // Allocate RGB data

            for (int i = 0; i < width * height; ++i) {
                rgbData[i * 3 + 0] = data[i]; // Red
                rgbData[i * 3 + 1] = data[i]; // Green
                rgbData[i * 3 + 2] = data[i]; // Blue
            }

            // Use the new RGB data
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData);

            // Free the RGB buffer
            delete[] rgbData;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
