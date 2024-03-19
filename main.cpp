//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

int glWindowWidth = 1280;
int glWindowHeight = 1024;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::mat4 lightRotation;
glm::mat4 secondLightRotation;

glm::mat4 modelElice;
GLuint modelEliceLoc;

glm::mat4 modelAvion;
GLuint modelAvionLoc;

glm::mat4 modelLumina;
GLuint modelLuminaLoc;

glm::mat4 modelLumina2;
GLuint modelLuminaLoc2;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::vec3 secondLightDir;
GLuint secondLightDirLoc;
glm::vec3 secondLightColor;
GLuint secondLightColorLoc;

gps::Camera myCamera(
                glm::vec3(0.0f, 2.0f, 5.5f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.05f;

bool pressedKeys[1024];
float angleY = 0.0f;
float rotatieElice = 0.0f;
float pozitieAvion = 0.0f;
int setLuminaUmbra = 0;
GLfloat lightAngle;
GLfloat lightAngle2;

gps::Model3D scena;
gps::Model3D elice;
gps::Model3D avion;
gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;

gps::Shader depthMapShader;
gps::Shader secondDepthMapShader;

GLuint shadowMapFBO;
GLuint secondShadowMapFBO;
GLuint depthMapTexture;
GLuint secondDepthMapTexture;

bool showDepthMap;
bool show = false;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

bool cursorDisabled = false;

int putFog = 0;
int sceneMode = 0;
int cameraTime = 0;

void cameraTour()
{
    rotatieElice++;
    pozitieAvion -= 0.005;
    if(cameraTime < 150){
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime < 250){
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime < 400){
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime == 400){
        myCamera.rotate(0, 0);
        cameraTime++;
    }
    else if(cameraTime < 500){
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime < 600){
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime < 1000){
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime == 1000){
        myCamera.rotate(0, 180);
        cameraTime++;
    }
    else if(cameraTime < 1200){
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime == 1200){
        myCamera.rotate(45, -90);
        cameraTime++;
    }
    else if(cameraTime < 1400){
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime < 1450){
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        cameraTime++;
    }
    else if(cameraTime == 1450){
        pozitieAvion = 0;
        rotatieElice = 0;
    }
    
}

void initSkybox(){
    std::vector<const GLchar*> faces;
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    mySkyBox.Load(faces);
}

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
    //TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            pressedKeys[key] = true;
        else if (action == GLFW_RELEASE)
            pressedKeys[key] = false;
    }
    
    if (pressedKeys[GLFW_KEY_V]) {
        cursorDisabled = !cursorDisabled;

        if (cursorDisabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    
    if (pressedKeys[GLFW_KEY_X]) {
        show = !show;
        cameraTime = 0;
        if(show){
            gps::Camera myCameraCopy(
                            glm::vec3(0.0f, 2.0f, 5.5f),
                            glm::vec3(0.0f, 0.0f, 0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
            myCamera = myCameraCopy;
        }
    }
    
}

float firstMouse = 0;

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if(!show){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        static double lastx = 0.0f;
        static double lasty = 0.0f;
        if(firstMouse == 0){
            lastx = xpos;
            lasty = ypos;
            firstMouse = 1;
        }
        
        double newYaw, newPitch;
        
        double xoffset = xpos - lastx;
        double yoffset = ypos - lasty;
        lastx = xpos;
        lasty = ypos;
        
        const float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        
        newYaw = myCamera.yaw + xoffset + 0.1f;
        newPitch = myCamera.pitch + yoffset + 0.1f;
        
        
        if (newPitch > 89.0f) {
            newPitch = 89.0f;
        }
        if (newPitch < -89.0f) {
            newPitch = -89.0f;
        }
        
        myCamera.rotate(newPitch, newYaw);
    }
}

void processMovement()
{
    if(!show){
        if (pressedKeys[GLFW_KEY_Q]) {
            lightAngle2 -= 1.0f;
        }
        if (pressedKeys[GLFW_KEY_E]) {
            lightAngle2 += 1.0f;
        }
        if (pressedKeys[GLFW_KEY_J]) {
            lightAngle -= 1.0f;
        }
        if (pressedKeys[GLFW_KEY_L]) {
            lightAngle += 1.0f;
        }
        if (pressedKeys[GLFW_KEY_1]) {
            rotatieElice -= 1.0f;
        }
        if (pressedKeys[GLFW_KEY_2]) {
            rotatieElice += 1.0f;
        }
        if (pressedKeys[GLFW_KEY_3]) {
            pozitieAvion -= 0.01f;
        }
        if (pressedKeys[GLFW_KEY_4]) {
            pozitieAvion += 0.01f;
        }
        if (pressedKeys[GLFW_KEY_5]) {
            setLuminaUmbra = 0;
        }
        if (pressedKeys[GLFW_KEY_6]) {
            setLuminaUmbra = 1;
        }
        if (pressedKeys[GLFW_KEY_W]) {
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_S]) {
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_A]) {
            myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        }
        if (pressedKeys[GLFW_KEY_D]) {
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        }
        //enable fog option
        if (pressedKeys[GLFW_KEY_F]) {
            putFog = 1;
            myCustomShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "putFog"), putFog);
        }
        //disable fog option
        if (pressedKeys[GLFW_KEY_H]) {
            putFog = 0;
            myCustomShader.useShaderProgram();
            glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "putFog"), putFog);
        }
        
        // visualize the 3 modes
            if (pressedKeys[GLFW_KEY_G]) {
                sceneMode += 1;
                switch (sceneMode) {
                case 0:
                    // solid mode;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    break;
                case 1:
                    // wireframe objects;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    break;
                case 2:
                    // polygonal and smooth;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    break;
                case 3:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glEnable(GL_POLYGON_SMOOTH);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
                    break;
                }
                if (sceneMode == 3) {
                    sceneMode = -1;
                }
                pressedKeys[GLFW_KEY_G] = false;
            }
    }
}

bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    //for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    //for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    //glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);

    glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    //for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}

void initOpenGLState()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
    scena.LoadModel("objects/ScenaFull/proiectTerminat.obj");
    avion.LoadModel("objects/avion/Avion.obj");
    elice.LoadModel("objects/ScenaFull/elice.obj");
//    nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
//    ground.LoadModel("objects/ground/ground.obj");
    lightCube.LoadModel("objects/cube/cube.obj");
    screenQuad.LoadModel("objects/quad/quad.obj");
    initSkybox();
}

void initShaders() {
    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
    depthMapShader.useShaderProgram();
    secondDepthMapShader.loadShader("shaders/secondDepthShader.vert", "shaders/secondDepthShader.frag");
    secondDepthMapShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initUniforms() {
    myCustomShader.useShaderProgram();

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    
    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    
    
    //set the second light direction (direction towards the light)
    secondLightDir = glm::vec3(1.0f, 1.0f, -2.5f);
    secondLightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
    secondLightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glUniform3fv(secondLightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * secondLightRotation)) * secondLightDir));

    //set light color
    secondLightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    secondLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    //generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);
    glGenFramebuffers(1, &secondShadowMapFBO);
    
    
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
    0);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //create second depth texture for FBO
    glGenTextures(1, &secondDepthMapTexture);
    glBindTexture(GL_TEXTURE_2D, secondDepthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    //attach second texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, secondShadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, secondDepthMapTexture,
    0);
    
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    
    // Varianta corecta
    lightDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0.0f, 1.0f, 1.0f, 0.0f));
    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 6.0f;
    glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

glm::mat4 computeLightSpaceTrMatrix2() {
    //TODO - Return the light-space transformation matrix;
    
    // Varianta corecta
    secondLightDir = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1.0f, 1.0f, -2.5f, 0.0f));
    glm::mat4 secondLightView = glm::lookAt(secondLightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 6.0f;
    glm::mat4 secondLightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    glm::mat4 secondLightSpaceTrMatrix = secondLightProjection * secondLightView;
    return secondLightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {
        
    shader.useShaderProgram();
    
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

//    nanosuit.Draw(shader);
    scena.Draw(shader);

//    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(0.5f));
//    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

//    ground.Draw(shader);
}

void renderScene() {

    // depth maps creation pass
    //TODO - Send the light-space transformation matrix to the depth map creation shader and
    //         render the scene in the depth map

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
    1,
    GL_FALSE,
    glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    drawObjects(depthMapShader, true);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Second depthMapShader
    secondDepthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(secondDepthMapShader.shaderProgram, "lightSpaceTrMatrix2"),
    1,
    GL_FALSE,
    glm::value_ptr(computeLightSpaceTrMatrix2()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, secondShadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    drawObjects(secondDepthMapShader, true);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render depth map on screen - toggled with the M key

    if (showDepthMap) {
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        if(setLuminaUmbra == 0){
            glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        }
        else
            glBindTexture(GL_TEXTURE_2D, secondDepthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myCustomShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        GLint viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        // Rotatie elice
        modelElice = glm::mat4(1.0f);
        // Daca nu merge centruElice bine, pune centruElice = glm::vec3(0.0f, -0.926f, 0.0f);s
        glm::vec3 centruElice = glm::vec3(0.0f, -0.926f, 2.14f);
        float angleInRadians = glm::radians(rotatieElice);
        modelElice = glm::translate(modelElice, -centruElice);
        modelElice = glm::rotate(modelElice, angleInRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        modelElice = glm::translate(modelElice, centruElice);
        modelEliceLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
        glUniformMatrix4fv(modelEliceLoc, 1, GL_FALSE, glm::value_ptr(modelElice));
        elice.Draw(myCustomShader);
        
        // Miscare avion
        
        modelAvion = glm::mat4(1.0f);
        modelAvion = glm::translate(modelAvion, glm::vec3(0.0f, 0.0f, pozitieAvion));
        
        modelAvionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
        glUniformMatrix4fv(modelAvionLoc, 1, GL_FALSE, glm::value_ptr(modelAvion));
        avion.Draw(myCustomShader);
                
//        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));
        
        if(setLuminaUmbra == 0)
            drawObjects(myCustomShader, false);
        
        //bind the second shadow map
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, secondDepthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 4);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix2()));

        drawObjects(myCustomShader, false);

        //draw a white cube around the light
        
        if(setLuminaUmbra == 0){
            glUniform3fv(secondLightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * secondLightRotation)) * secondLightDir));
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
        }
        else{
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
            glUniform3fv(secondLightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * secondLightRotation)) * secondLightDir));
        }
        
        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        modelLumina = lightRotation;
        modelLumina = glm::translate(modelLumina, 1.0f * lightDir);
        modelLumina = glm::scale(modelLumina, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelLumina));

        lightCube.Draw(lightShader);
        
        
        
        // A doua lumina
        
        
        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        modelLumina2 = secondLightRotation;
        modelLumina2 = glm::translate(modelLumina2, 1.0f * secondLightDir);
        modelLumina2 = glm::scale(modelLumina2, glm::vec3(0.05f, 0.05f, 0.05f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelLumina2));

        lightCube.Draw(lightShader);
        
        
        //Desenare skybox
        mySkyBox.Draw(skyboxShader, view, projection);
        
        if(show){
            cameraTour();
        }
            
    }
}

void cleanup() {
    glDeleteTextures(1, &depthMapTexture);
    glDeleteTextures(1, &secondDepthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteFramebuffers(1, &secondShadowMapFBO);
    glfwDestroyWindow(glWindow);
    //close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char * argv[]) {

    if (!initOpenGLWindow()) {
        glfwTerminate();
        return 1;
    }

    initOpenGLState();
    initObjects();
    initShaders();
    initUniforms();
    initFBO();

    

    while (!glfwWindowShouldClose(glWindow)) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();

    return 0;
}
