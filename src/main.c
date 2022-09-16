#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "main.h"
#include "linear.h"
#include "shader.h"

struct Camera {
    Vec3 position;
    Vec3 front;
    Vec3 up;
};

float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window);
static Mat4 processCameraInput(GLFWwindow *window, struct Camera *cameraObj, float deltaTime);
static unsigned int loadTexture(char const *path);

void
userError(const char *msg, const char *detail)
{
    fprintf(stderr, "%s: %s\n", msg, detail);
    exit(1);
}

void
glfw_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void
processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}

Mat4
processCameraInput(GLFWwindow *window, struct Camera *camObj, float deltaTime)
{
    /*
     * Keyboard Input
     */
    Vec3 tmp;
    float speed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // pos = pos + front * speed
        tmp = linearVec3ScalarMulp(camObj->front, speed);
        camObj->position = linearVec3Add(camObj->position, tmp);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // pos = pos + front * (-speed)
        tmp = linearVec3ScalarMulp(camObj->front, -speed);
        camObj->position = linearVec3Add(camObj->position, tmp);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // pos = pos + unit|(up x front)| * (speed)
        tmp = linearVec3CrossProduct(camObj->front, camObj->up);
        tmp = linearVec3Normalize(tmp);
        tmp = linearVec3ScalarMulp(tmp, speed);
        camObj->position = linearVec3Add(camObj->position, tmp);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // pos = pos + unit|(up x front)| * (-speed)
        tmp = linearVec3CrossProduct(camObj->front, camObj->up);
        tmp = linearVec3Normalize(tmp);
        tmp = linearVec3ScalarMulp(tmp, -speed);
        camObj->position = linearVec3Add(camObj->position, tmp);
    }

    /*
     * Mouse Input
     */

    static int firstMouse = 1;
    float sensibility = 0.1f;
    static float yaw = -90.0;
    static float pitch = 0.0;

    double xpos, ypos;
    static double lastX, lastY;
    float xoffset, yoffset;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        firstMouse = 0;
        lastX = xpos;
        lastY = ypos;
    }

    xoffset = (xpos - lastX) * sensibility;
    yoffset = (ypos - lastY) * sensibility;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0) pitch = 89.0f;
    else if (pitch < -89.0) pitch = -89.0f;
    float rpitch = M_PI / 180 * pitch;
    float ryaw = M_PI / 180 * yaw;

    tmp = linearVec3(
            cosf(ryaw) * cosf(-rpitch),
            sinf(-rpitch),
            sinf(ryaw) * cosf(-rpitch));
    camObj->front = linearVec3Normalize(tmp);

    return linearLookAt(camObj->position, 
                        linearVec3Add(camObj->front, camObj->position), 
                        camObj->up);
}


static unsigned int
loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

    if (data) {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        if (channels == 3)
            format = GL_RED;
        if (channels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        return textureID;
    }
    char errorMsg[500];
    sprintf(errorMsg, "file %200s not found", path);
    userError("loadTexture() Error", errorMsg);
    return textureID;
}

int main()
{
    GLFWwindow *window;
    const char *glfwErrno;

    if (!glfwInit()) {
        glfwGetError(&glfwErrno);
        glfwTerminate();
        userError("glfwInit() Error", glfwErrno);
    }

    window = glfwCreateWindow(640, 480, "Mverse", NULL, NULL);
    if (!window) {
        glfwTerminate();
        userError("glfwCreateWindow() failed", "Can't create window");
    }

    glfwSetFramebufferSizeCallback(window, glfw_size_callback);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLubyte glewErrno = glewInit();
    if (glewErrno != GLEW_OK) {
        glfwTerminate();
        userError("glewInit() failed", (const char *)glewGetErrorString(glewErrno));
    }

    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(1);

    unsigned int programColor = shaderCreateProgram("shaders/color.vsh",
                                                    "shaders/color.fsh");
    unsigned int programLight = shaderCreateProgram("shaders/lightsource.vsh",
                                                    "shaders/lightsource.fsh");
    unsigned int VBO, VAO, lightVAO;
    int vertexSize = 8;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("textures/container2.png");
    unsigned int specularMap = loadTexture("textures/container2_specular.png");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    /*============ Initializations ===============*/

    Vec3 cubePositions[] = {
                            linearVec3( 0.5f,  0.0f,  0.0f),
                            linearVec3( 2.0f,  5.0f, -15.0f),
                            linearVec3(-1.5f, -2.2f, -2.5f),
                            linearVec3(-3.8f, -2.0f, -12.3f),
                            linearVec3( 2.4f, -0.4f, -3.5f),
                            linearVec3(-1.7f,  3.0f, -7.5f),
                            linearVec3( 1.3f, -2.0f, -2.5f),
                            linearVec3( 1.5f,  2.0f, -2.5f),
                            linearVec3( 1.5f,  0.2f, -1.5f),
                            linearVec3(-1.3f,  1.0f, -1.5f)
                            };

    Vec3 pointLightsPos[4] = {
                            linearVec3(0.7f, 0.2f, 2.0f),
                            linearVec3(2.3f, -3.3f, -4.0f),
                            linearVec3(-4.0f, 2.0f, -12.0f),
                            linearVec3(0.0f, 0.0f, -3.0f),
                            };

    struct Camera mainCamera;
    mainCamera.position = linearVec3(0.0, 0.0, 3.0);
    mainCamera.front = linearVec3(0.0, 0.0, -1.0);
    mainCamera.up = linearVec3(0.0, 1.0, 0.0);

    Mat4 model, view, proj;
    Mat4 T, S, R;

    Vec3 dirLight = linearVec3(-0.2, -1.0, 0.3);
    Vec3 ambient = linearVec3(0.0, 0.0, 0.0);
    Vec3 diffuse = linearVec3(0.5, 0.5, 0.5);
    Vec3 specular = linearVec3(1.0, 1.0, 1.0);
    float dt, t, t0;
    int width, height;
    t0 = 0;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* -------------------
         * frame calculations
         * ------------------*/
        t = (float)glfwGetTime();
        dt = t - t0;
        t0 = t;


        view = processCameraInput(window, &mainCamera, dt);
        glfwGetWindowSize(window, &width, &height);
        proj = linearPerspective(35, (float)width / height, 0.1, 100);

        glUseProgram(programColor);

        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.032f;

        shaderSetfv(programColor, "dirLight.direction", dirLight.vector, glUniform3fv);
        shaderSetfv(programColor, "dirLight.ambient",   ambient.vector, glUniform3fv);
        shaderSetfv(programColor, "dirLight.diffuse",   diffuse.vector, glUniform3fv);
        shaderSetfv(programColor, "dirLight.specular",  vec3(1.0, 1.0, 1.0), glUniform3fv);

        char varNames[7][50];
        for (int i = 0; i < 4; i++) {
            sprintf(varNames[0], "pointLight[%d].position", i);
            sprintf(varNames[1], "pointLight[%d].ambient", i);
            sprintf(varNames[2], "pointLight[%d].diffuse", i);
            sprintf(varNames[3], "pointLight[%d].specular", i);
            sprintf(varNames[4], "pointLight[%d].constant", i);
            sprintf(varNames[5], "pointLight[%d].linear", i);
            sprintf(varNames[6], "pointLight[%d].quadratic", i);

            shaderSetfv(programColor, varNames[0], pointLightsPos[i].vector, glUniform3fv);
            shaderSetfv(programColor, varNames[1], ambient.vector, glUniform3fv);
            shaderSetfv(programColor, varNames[2], diffuse.vector, glUniform3fv);
            shaderSetfv(programColor, varNames[3], specular.vector, glUniform3fv);
            shaderSet1f(programColor, varNames[4], constant);
            shaderSet1f(programColor, varNames[5], linear);
            shaderSet1f(programColor, varNames[6], quadratic);
        }

        shaderSetfv(programColor, "flashLight.position", mainCamera.position.vector, glUniform3fv);
        shaderSetfv(programColor, "flashLight.direction", mainCamera.front.vector, glUniform3fv);
        shaderSetfv(programColor, "flashLight.ambient", ambient.vector, glUniform3fv);
        shaderSetfv(programColor, "flashLight.diffuse", vec3(0.8, 0.5, 0.5), glUniform3fv);
        shaderSetfv(programColor, "flashLight.specular", vec3(1.0, 0.0, 0.0), glUniform3fv);
        shaderSet1f(programColor, "flashLight.constant", constant);
        shaderSet1f(programColor, "flashLight.linear", linear);
        shaderSet1f(programColor, "flashLight.quadratic", quadratic);
        shaderSet1f(programColor, "flashLight.cutOff", cosf(12.5f * M_PI / 180));
        shaderSet1f(programColor, "flashLight.outerCutOff", cosf(17.5f * M_PI / 180));

        shaderSetfv(programColor, "viewPos", mainCamera.position.vector, glUniform3fv);
        shaderSetfv(programColor, "material.specular", vec3(0.5, 0.5, 0.5), glUniform3fv);
        shaderSet1f(programColor, "material.shininess", 64.0f);
        shaderSet1i(programColor, "material.diffuse", 0);
        shaderSet1i(programColor, "material.specular", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        shaderSetMatrixfv(programColor, "view", view.matrix[0], glUniformMatrix4fv);
        shaderSetMatrixfv(programColor, "proj", proj.matrix[0], glUniformMatrix4fv);
        for (int i = 0; i < 10; i++) {
            // World Transformation
            // --------------------
            T = linearTranslatev(cubePositions[i]);
            R = linearRotate(180 * i / M_PI, 1.0, 0.3, 0.5);
            S = linearScale(1.0, 1.0, 1.0);
            model = linearMat4Muln(3, T, R, S);
            shaderSetMatrixfv(programColor, "model", model.matrix[0], glUniformMatrix4fv);
            shaderSetMatrixfv(programColor, "rotNormals", R.matrix[0], glUniformMatrix4fv);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
        }

        /* -----------------
         *  Light Source
         * -----------------*/
        for (int i = 0; i < 4; i++) {
            model = linearMat4Mul(linearTranslatev(pointLightsPos[i]), 
                                  linearScale(0.2, 0.2, 0.2));
            glUseProgram(programLight);
            glBindVertexArray(lightVAO);

            shaderSetMatrixfv(programColor, "model", model.matrix[0], glUniformMatrix4fv);
            shaderSetMatrixfv(programColor, "view", view.matrix[0], glUniformMatrix4fv);
            shaderSetMatrixfv(programColor, "proj", proj.matrix[0], glUniformMatrix4fv);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
