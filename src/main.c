#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "main.h"

const char *vertexShaderSource = \
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = \
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

//static void sysError(const char *msg);
static void userError(const char *msg, const char *detail);
static void glfw_size_callback(GLFWwindow *window, int width, int height);
static void processInput(GLFWwindow *window, struct Background *c);
static void checkGLSuccess(unsigned int compiledShader, unsigned int shaderStep);

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

void 
checkGLSuccess(unsigned int shaderBinary, unsigned int shaderStep)
{
    int success;
    char infoLog[512];
    switch (shaderStep) {
        case 0:
            glGetShaderiv(shaderBinary, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shaderBinary, 512, NULL, infoLog);
                userError("glCompileShader() ERROR", infoLog);
            }
            break;

        case 1:
            glGetProgramiv(shaderBinary, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(shaderBinary, 512, NULL, infoLog);
                userError("glLinkProgram() ERROR", infoLog);
            }
            break;

        default:
            break;
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

    unsigned int vertexShader, fragmentShader, shaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkGLSuccess(vertexShader, 0);
    printf("Vertex Compiled\n");

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkGLSuccess(fragmentShader, 0);
    printf("Fragment Compiled\n");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkGLSuccess(shaderProgram, 1);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window)) {
        processInput(window, &colors);

        glClearColor(colors.R, colors.G, colors.B, colors.A);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
