#include <glad/glad.h>
#include <string>
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"
#include "raycast.h"
#include "physics.h"
#include "text.h"
#include "menu.h"
#include "face.h"
#include "keyboard.h"
#include "stage.h"
#include "collisions.h"
#include "sound.h"


// MOVE: used for text studd
float mixValue;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 cameraPos   = glm::vec3(1.0f, 2.0f,  1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

SoundManager soundMgr;
int fartIdx;
int stepIdx;

Camera camera(cameraPos);
Stage stage;
DebugMenu debugMenu;
KeyboardDebouncer kbd;

float lastX = 400, lastY = 400;
float fov = 45.0;
float yaw, pitch;
bool firstMouse = true;


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

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

unsigned int loadCubemap(vector<std::string> faces)
{
    stbi_set_flip_vertically_on_load(false); 
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true); 
    return textureID;
}  

bool flashlightOn = true;
void processInput(GLFWwindow *window)
{
    kbdMgr.updateWithInput(window);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        direction.z += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction.z -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction.x -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction.x += 1.0f;
    }
    if (direction != glm::vec3(0.0f, 0.0f, 0.0f)) {
        camera.ProcessKeyboard(direction, deltaTime);
    }
    else {
        // TODO: handle the move vector better
        camera.movVector = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // flashlight
    if (kbd.checkKey(GLFW_KEY_F)) {
        flashlightOn = !flashlightOn;
    }

    // play sound
    if (kbd.checkKey(GLFW_KEY_Q)) {
        soundMgr.play_sound(fartIdx);
    }

    // Physics
    if (kbd.checkKey(GLFW_KEY_SPACE)) {
        yVelocity = 0.05f;
    }

    debugMenu.ProcessKeyboard(window);
}

glm::mat4 makeLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    glm::vec3 d = glm::normalize(eye - center);
    glm::vec3 r = glm::normalize(glm::cross(up, d));
    up = glm::cross(d, r);
    glm::mat4 l1(
        glm::vec4(r.x, up.x, d.x, 0.0f),
        glm::vec4(r.y, up.y, d.y, 0.0f),
        glm::vec4(r.z, up.z, d.z, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    glm::mat4 l2(1.0);
    l2[3][0] = -eye.x;
    l2[3][1] = -eye.y;
    l2[3][2] = -eye.z;

    return l1 * l2;
}

int main() {
    // OPENAL TESTING, MOVE THIS
    soundMgr.init();
    fartIdx = soundMgr.load_sound("sounds/fart-03.wav");
    stepIdx = soundMgr.load_sound("sounds/grass_step.wav");

    // OPENGL STUFF
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "mike da game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
  
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    glViewport(0, 0, 800, 600);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetScrollCallback(window, scroll_callback); 


    float verticesCube[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // Generate map from string
    std::string mapString[] = {
        "xxxxxxxxxx",
        "x        x",
        "x        x",
        "x        x",
        "x        x",
        "x        x",
        "x        x",
        "x        x",
        "x        x",
        "xxxxxxxxxx"
    };
    std::vector<glm::vec3> cubeMap;
    // Add cubes from 10x10 map
    for (int i=0; i < size(mapString); i++) {
        std::string item = mapString[i];
        for (int j=0; j < item.length(); j++) {
            // Place cube on map if it exists
            if (item[j] == 'x') {
                cubeMap.push_back(glm::vec3(i, 0.0f, j));
            }
            // Add cubes to floor
            cubeMap.push_back(glm::vec3(i, -1.0f, j));
        }
    }

    unsigned int VBO1;
    glGenBuffers(1, &VBO1);  

    Shader lightingShader("shader.vs", "shader.fs");
    Shader lightCubeShader("shader.vs", "light.fs");
    Shader textShader("text_shader.vs", "text_shader.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");
    Shader wireframeShader("wireframe.vs", "wireframe.gs", "wireframe.fs");
    // Shader ourShader("model_loading.vs", "model_loading.fs");

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);  
    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);  
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);  

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Do this to flip images upside down

    unsigned int tileTextureColor = loadTexture("textures/tiles_color.png");
    unsigned int tileTextureSpec = loadTexture("textures/tiles_roughness.png");
    unsigned int containerTextureColor = loadTexture("textures/container2.png");
    unsigned int containerTextureSpec = loadTexture("textures/container2_specular.png");
    unsigned int stonesTextureColor = loadTexture("textures/stones_color.png");
    unsigned int stonesTextureSpec = loadTexture("textures/stones_roughness.png");

    mixValue = 0.5f;

    // Enable z buffer
    glEnable(GL_DEPTH_TEST);  
    glDepthFunc(GL_LESS);  

    glm::vec3 cubePos( 0.0f,  0.0f,  0.0f);

    yaw = -90.0f;

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };  

    // Model backpack("models/backpack/backpack.obj");
    Model sword("models/sword.obj");
    sword.model = glm::translate(sword.model, glm::vec3(6.0f, 1.0f, 6.0f)); // translate it down so it's at the center of the scene
    sword.model = glm::scale(sword.model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

    Model well("models/well.obj");
    well.model = glm::translate(well.model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    well.model = glm::scale(well.model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down

    Model room("models/room.obj");
    room.model = glm::translate(room.model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    room.model = glm::scale(room.model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down

    Model park("models/park.obj");
    park.model = glm::translate(park.model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    park.model = glm::scale(park.model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down

    // Model cubeModel("models/cube.obj");
    // park.model = glm::translate(park.model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    // park.model = glm::scale(park.model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down

    CollisionObject roomCol(&room);
    CollisionObject parkCol(&park);

    loadTextStuff();

    stage.init();
    debugMenu.Setup(&stage, &(camera.Position));

    kbdMgr.registerKeyboard(&kbd);

    // Skybox 
    vector<std::string> faces
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);  

    float lastStepTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        // Input stuff
        processInput(window);

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // must clear z buffer bit so its not carried over
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // PHYSICS & COLLISIONS
        // AABB Box collisions
        // std::vector<bool> boxColliding;
        // for (int i=0; i < cubeMap.size(); i++) {
        //     glm::vec3 initPos = camera.Position;
        //     camera.Position = calcCollisionsAABB(camera.Position, cubeMap[i]);
        //     if (initPos != camera.Position) {
        //         boxColliding.push_back(true);
        //     } else {
        //         boxColliding.push_back(false);
        //     }
        // }

        glm::vec3 gravityMov = calcGravity();
        glm::vec3 totalMovVector = camera.movVector + gravityMov;
        // collisions against room model
        // auto colResult = roomCol.checkCollisions(camera.Position, totalMovVector, room.model);
        // glm::vec3 newMovVec = colResult.first;
        // bool hitFloor = colResult.second;
        // camera.Position += newMovVec;
        // if (hitFloor) {
        //     yVelocity = 0.0f;
        // }

        // AABB collisions against well bounding box
        // glm::vec3 wellAABB = glm::vec3(0.0f, 0.5f, 0.0f);
        AABB wellBBox = well.GetBoundingBox();
        totalMovVector = calcCollisionsAABB(camera.Position, totalMovVector, wellBBox);

        auto colResult = parkCol.checkCollisions(camera.Position, totalMovVector, park.model);
        glm::vec3 newMovVec = colResult.first;
        bool hitFloor = colResult.second;
        camera.Position += newMovVec;
        if (hitFloor) {
            yVelocity = 0.0f;
            // Play hit ground sound when we hit the floor
            // soundMgr.play_sound(stepIdx);
        }

        // Play step sound if weve moved
        if (currentFrame - lastStepTime > 0.5 && glm::length(newMovVec) > 0.025 && hitFloor) {
            soundMgr.play_sound(stepIdx);
            lastStepTime = currentFrame;
        }

        // Check if player is looking at the wellBBox
        glm::vec3 wellMin(wellBBox.x0, wellBBox.y0, wellBBox.z0);
        glm::vec3 wellMax(wellBBox.x1, wellBBox.y1, wellBBox.z1);
        bool lookingAtWell = rayIntersectsAABB(camera.Position, camera.Front, wellMin, wellMax);
        if (lookingAtWell) {
            std::cout << lookingAtWell << std::endl;
        }

        // RENDERING
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        glm::mat4 viewSkybox = glm::mat4(glm::mat3(view));
        skyboxShader.setMat4("view", viewSkybox);
        glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

        glBindVertexArray(VAO); 
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        // ... draw rest of the scene

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);

        /*
           Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
           the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
           by defining light types as classes and set their values in there, or by using a more efficient uniform approach
           by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        */
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   
        if (flashlightOn) {
            lightingShader.setFloat("spotLight.constant", 1.0f);
            lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        } else {
            lightingShader.setFloat("spotLight.constant", 0.0f);
            lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // render the loaded model
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        // lightingShader.setMat4("model", model);
        // backpack.Draw(lightingShader);

        // room stuff
        // lightingShader.setMat4("model", room.model);
        // room.Draw(lightingShader);
        // wireframeShader.use();
        // wireframeShader.setMat4("model", room.model);
        // wireframeShader.setMat4("projection", projection);
        // wireframeShader.setMat4("view", view);
        // room.Draw(wireframeShader);

        // -- begin section involving texture select --

        // bind diffuse map
        lightingShader.use();
        glActiveTexture(GL_TEXTURE0);
        if (debugMenu.textureMenu->cursorPos == 0) {
            glBindTexture(GL_TEXTURE_2D, tileTextureColor);
        }
        else if (debugMenu.textureMenu->cursorPos == 1) {
            glBindTexture(GL_TEXTURE_2D, containerTextureColor);
        }
        else if (debugMenu.textureMenu->cursorPos == 2) {
            glBindTexture(GL_TEXTURE_2D, stonesTextureColor);
        }
        glUniform1i(glGetUniformLocation(lightingShader.ID, "material.diffuse"), 0);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        if (debugMenu.textureMenu->cursorPos == 0) {
            glBindTexture(GL_TEXTURE_2D, tileTextureSpec);
        }
        else if (debugMenu.textureMenu->cursorPos == 1) {
            glBindTexture(GL_TEXTURE_2D, containerTextureSpec);
        }
        else if (debugMenu.textureMenu->cursorPos == 2) {
            glBindTexture(GL_TEXTURE_2D, stonesTextureSpec);
        }
        glUniform1i(glGetUniformLocation(lightingShader.ID, "material.specular"), 1);

        // render the sword
        lightingShader.setMat4("model", sword.model);
        sword.Draw(lightingShader);

        // render the well
        lightingShader.setMat4("model", well.model);
        well.Draw(lightingShader);

        // This draws all our AABB boxes
        glBindVertexArray(VAO); 
        wireframeShader.use();
        wireframeShader.setMat4("model", wellBBox.GetCubeXform());
        wireframeShader.setMat4("projection", projection);
        wireframeShader.setMat4("view", view);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        // for (int i=0; i < cubeMap.size(); i++) {
        //     model = glm::mat4(1.0f);
        //     model = glm::translate(model, cubeMap[i]); // translate it down so it's at the center of the scene
        //     lightingShader.setMat4("model", model);
        //     if (boxColliding[i]) {
        //         lightingShader.setVec3("addColor", glm::vec3(0.5f, 0.5f, 0.5f));
        //     }
        //     else {
        //         lightingShader.setVec3("addColor", glm::vec3(0.0f, 0.0f, 0.0f));
        //     }
        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        // This should just draw a face
        glm::mat4 model;
        lightingShader.use();
        lightingShader.setVec3("addColor", glm::vec3(0.0f, 0.0f, 0.0f));
        for (int i=0; i < stage.faceVector.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, stage.faceVector[i]->translate); 
            model = glm::scale(model, stage.faceVector[i]->scale); 
            lightingShader.setMat4("model", model);
            stage.faceVector[i]->Draw();
        }

        // Unbind texture stuff
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, 0); 
        glActiveTexture(GL_TEXTURE1); 
        glBindTexture(GL_TEXTURE_2D, 0); 

        // -- end section involving texture select --

        // park stuff
        lightingShader.setMat4("model", park.model);
        park.Draw(lightingShader);

        // also draw the lamp object(s)
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        debugMenu.Draw(textShader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO1);
    soundMgr.destroy();

    glfwTerminate();
    return 0;
}