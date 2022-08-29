#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "linear.h"
#include "shader.h"

float vertices[] = {
    -0.5,-0.5,-0.5, 1.0, 1.0, 1.0,
     0.5,-0.5,-0.5, 1.0, 1.0, 0.0,
    -0.5, 0.5,-0.5, 1.0, 0.0, 1.0,
     0.5, 0.5,-0.5, 1.0, 0.0, 0.0,
    -0.5,-0.5, 0.5, 0.0, 1.0, 1.0,
     0.5,-0.5, 0.5, 0.0, 1.0, 0.0,
    -0.5, 0.5, 0.5, 0.0, 0.0, 1.0,
     0.5, 0.5, 0.5, 0.0, 0.0, 0.0,
};

unsigned int indices[] = {
    0, 1, 2,
    2, 3, 1,

    4, 5, 7,
    7, 6, 4,

    1, 3, 5,
    3, 7, 5,

    6, 0, 2,
    6, 0, 4,

    2, 7, 6,
    2, 7, 3,

    0, 1, 5,
    0, 4, 5
};

static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window);

static void linearPrintMat4(mat4 x);

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

void
linearPrintMat4(mat4 x)
{
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            printf("%f ", x.matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
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

    GLubyte glewErrno = glewInit();
    if (glewErrno != GLEW_OK) {
        glfwTerminate();
        userError("glewInit() failed", (const char *)glewGetErrorString(glewErrno));
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int shaderProgram = shaderCreateProgram("shaders/vertex.vsh", 
                                                     "shaders/fragment.fsh");

    unsigned int modelLoc, viewLoc, projLoc;
    mat4 model, view, proj;
    mat4 T, S, R;

    float t;
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        t = (float)glfwGetTime();
        T = linearTranslate(0.0, 0.0, 0.0);
        R = linearRotate(180 * t / M_PI, 0.0, 1.0, 0.0);
        S = linearScale(1.0, 1.0, 1.0);

        model = linearMat4Muln(3, T, R, S);

        T = linearTranslate(0.0, -0.0, -80);
        R = linearRotate(60, 1, 0, 0);

        view = linearMat4Muln(2, T, R);

        proj = linearPerspective(35, 4 / 3.0, 0.1, 100);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        modelLoc = glGetUniformLocation(shaderProgram, "model");
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        projLoc = glGetUniformLocation(shaderProgram, "proj");

        glUniformMatrix4fv(modelLoc, 1, GL_TRUE, model.matrix[0]);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        glUniformMatrix4fv(viewLoc, 1, GL_TRUE, view.matrix[0]);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        glUniformMatrix4fv(projLoc, 1, GL_TRUE, proj.matrix[0]);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
