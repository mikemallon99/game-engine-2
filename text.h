#ifndef TEXT_H
#define TEXT_H

#include "shader.h"
#include "texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

unsigned int VAO_text, VBO_text;

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

std::map<char, Character> Characters;
unsigned int bubbleTexID;

void RenderTextBubble(Shader &textShader, Shader &bubbleShader, std::string text, float scale, glm::vec3 color)
{
    // Calculate size of the bubble
    std::string::const_iterator c;
    float x = 0.0f;
    float y = 0.0f;
    float x_max, y_max;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        x_max = xpos+w;
        y_max = ypos+h;
    }

    // TODO: render the big quad behind them with the bubble
    float x_offset = 0.1f;
    float y_offset = 0.1f;
    float verticesBox[6][4] = {
        { -x_max/2 - x_offset,     y_max/2 + y_offset,   0.0f, 0.0f },            
        { -x_max/2 - x_offset,     -y_max/2 - y_offset,       0.0f, 1.0f },
        { x_max/2 + x_offset, -y_max/2 - y_offset,       1.0f, 1.0f },

        { -x_max/2 - x_offset,     y_max/2 + y_offset,   0.0f, 0.0f },
        { x_max/2 + x_offset, -y_max/2 - y_offset,       1.0f, 1.0f },
        { x_max/2 + x_offset, y_max/2 + y_offset,   1.0f, 0.0f }           
    };
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, bubbleTexID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesBox), verticesBox); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    bubbleShader.use();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Render all the text
    // activate corresponding render state	
    textShader.use();
    glUniform3f(glGetUniformLocation(textShader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);

    x = 0.0f;
    y = 0.0f;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos - x_max/2,     ypos + h - y_max/2,   0.0f, 0.0f },            
            { xpos - x_max/2,     ypos - y_max/2,       0.0f, 1.0f },
            { xpos + w - x_max/2, ypos - y_max/2,       1.0f, 1.0f },

            { xpos - x_max/2,     ypos + h - y_max/2,   0.0f, 0.0f },
            { xpos + w - x_max/2, ypos - y_max/2,       1.0f, 1.0f },
            { xpos + w - x_max/2, ypos + h - y_max/2,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    s.use();
    s.setMat4("model", glm::mat4(1.0f));
    s.setMat4("view", glm::mat4(1.0f));
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_text);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int loadTextStuff() {
    // Text loading stuff
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    std::string fontpath = "fonts/arial.ttf";
    if (FT_New_Face(ft, fontpath.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;  
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);  

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int text_texture;
        glGenTextures(1, &text_texture);
        glBindTexture(GL_TEXTURE_2D, text_texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            text_texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &VAO_text);
    glGenBuffers(1, &VBO_text);
    glBindVertexArray(VAO_text);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);      

    bubbleTexID = loadTexture("textures/speechbubble.png");

    return 0;
}

int calcMaxYHeight() {
    int maxHeight = 0;
    for (int c=0; c < Characters.size(); c++) {
        if (Characters[c].Size.y > maxHeight) {
            maxHeight = Characters[c].Size.y;
        }
    }
    return maxHeight;
}

#endif 