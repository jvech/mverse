#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "main.h"

static void sysError(const char *msg);
static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window, struct Background *c);

void
sysError(const char *msg)
{
    perror(msg);
    exit(1);
}

void 
userError(const char *msg, const char *detail)
{
    fprintf(stderr, "%s : %s\n", msg, detail);
    exit(1);
}

static void
glfw_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void 
processInput(GLFWwindow *window, struct Background *c)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS
               && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
               && c->R >= 0.0) {
        c->R -= 0.1;
    } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && c->R <= 1.0) {
        c->R += 0.1;
    } 
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

    struct Background colors = {.R = 0.0, .G = 0.0, .B = 0.0, .A = 0.0};

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(colors.R, colors.G, colors.B, colors.A);

        processInput(window, &colors);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
