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
#include "../include/stb_image.h"

#include "main.h"
#include "linear.h"
#include "shader.h"
#include "obj.h"

struct Camera {
    Vec3 position;
    Vec3 front;
    Vec3 up;
};

static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window);
static Mat4 processCameraInput(GLFWwindow *window, struct Camera *cameraObj, float deltaTime);
static unsigned int loadTexture(char const *path);
static void meshSetUp(Mesh *mesh);
static void meshDraw(unsigned int shader, Mesh mesh);
static void objSetUp(Obj obj);
static void objDraw(unsigned int shader, Obj obj);

static float cameraSpeed = 2.0;

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
    float speed = cameraSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_K)) cameraSpeed += 0.2;
    if (glfwGetKey(window, GLFW_KEY_J)) cameraSpeed -= 0.2;

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


void
meshSetUp(Mesh *mesh)
{
    glGenVertexArrays(1, &(mesh->VAO));
    glGenBuffers(1, &(mesh->VBO));
    glGenBuffers(1, &(mesh->EBO));

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertexSize * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexSize * sizeof(int), mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void
meshDraw(unsigned int shader, Mesh mesh)
{
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indexSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void
objSetUp(Obj obj)
{
    int i;
    for (i = 0;  i < obj.size; i++)
        meshSetUp(obj.mesh + i);
}

void
objDraw(unsigned int shader, Obj obj)
{
    int i;
    for (i = 0; i < obj.size; i++) {
        shaderSetfv(shader, "mtl.ambient",  obj.mesh[i].material.ka, glUniform3fv);
        shaderSetfv(shader, "mtl.diffuse",  obj.mesh[i].material.kd, glUniform3fv);
        shaderSetfv(shader, "mtl.specular", obj.mesh[i].material.ks, glUniform3fv);
        meshDraw(shader, obj.mesh[i]);
    }
}


int main(int argc, char *argv[])
{
    Obj obj;
    GLFWwindow *window;
    const char *glfwErrno;

    if      (argc == 2) obj = objCreate(argv[1]);
    else if (argc == 1) obj = objCreate("-");
    else                return 1;

    if (obj.mesh == NULL) {
        glfwTerminate();
        userError("objCreateMesh Error", "NULL mesh returned");
    }

    if (!glfwInit()) {
        glfwGetError(&glfwErrno);
        glfwTerminate();
        userError("glfwInit() Error", glfwErrno);
    }

    window = glfwCreateWindow(640, 480, "Mverse", NULL, NULL);
    if (!window) {
        glfwTerminate();
        userError("glfwCreateWindow() Error", "Can't create window");
    }

    glfwSetFramebufferSizeCallback(window, glfw_size_callback);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLubyte glewErrno = glewInit();
    if (glewErrno != GLEW_OK) {
        glfwTerminate();
        userError("glewInit Error", (const char *)glewGetErrorString(glewErrno));
    }

    unsigned int shader = shaderCreateProgram("shaders/dummy.vsh", "shaders/dummy.fsh");


    objSetUp(obj);

    struct Camera mainCamera;
    mainCamera.position = linearVec3(0.0, 0.0, 10.0);
    mainCamera.front = linearVec3(0.0, 0.0, -1.0);
    mainCamera.up = linearVec3(0.0, 1.0, 0.0);

    Mat4 model, view, proj;
    Mat4 T, S, R;
    float t, t0, dt;
    int width, height;
    t0 = 0;

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetWindowSize(window, &width, &height);

        t = (float)glfwGetTime();
        dt = t - t0;
        t0 = t;

        view = processCameraInput(window, &mainCamera, dt);
        proj = linearPerspective(35, (float)width / height, 0.1, 100);
        T = linearTranslate(0.0, 0.0, 0.0);
        R = linearRotate(0, 1.0, 0.0, 0.0);
        S = linearScale(1, 1, 1);
        model = linearMat4Muln(3, T, R, S);

        glUseProgram(shader);

        shaderSetMatrixfv(shader, "model", model.matrix[0], glUniformMatrix4fv);
        shaderSetMatrixfv(shader, "proj", proj.matrix[0], glUniformMatrix4fv);
        shaderSetMatrixfv(shader, "view", view.matrix[0], glUniformMatrix4fv);
        shaderSetMatrixfv(shader, "rotNormals", R.matrix[0], glUniformMatrix4fv);

        objDraw(shader, obj);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}
