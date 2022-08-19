#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "main.h"

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

//static void sysError(const char *msg);
static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window, struct Background *c);

void 
userError(const char *msg, const char *detail)
{
    fprintf(stderr, "%s: %s\n", msg, detail);
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

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int shaderProgram = shaderCreateProgram("shaders/vertex.vsh", 
                                                     "shaders/fragment.vsh");

    while (!glfwWindowShouldClose(window)) {
        processInput(window, &colors);

        glClearColor(colors.R, colors.G, colors.B, colors.A);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    int nattributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nattributes);
    printf("%d\n", nattributes);
    glfwTerminate();
    return 0;
}
