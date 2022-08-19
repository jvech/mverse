#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <GL/glew.h>
#include "shader.h"

static char *getShaderSource(const char *shaderPath);
static void checkShaderCompile(unsigned int shader, const char *shaderPath);
static void checkProgramLink(unsigned int shader);

char
*getShaderSource(const char *shaderPath)
{
    long fileSize;
    char *shaderSource;
    FILE *shaderFile = fopen(shaderPath, "r");

    if (!shaderFile) {
        perror("getShaderSource() ERROR");
        exit(1);
    }

    fseek(shaderFile, 0L, SEEK_END);
    fileSize = ftell(shaderFile);
    if (!fileSize) {
        fprintf(stderr, "getShaderSource() ERROR: %s file is empty\n", shaderPath);
        exit(1);
    }

    rewind(shaderFile);

    shaderSource = malloc(fileSize * sizeof(char));
    if (!fscanf(shaderFile, "%[^\\]", shaderSource)) {
        perror("getShaderSource() ERROR");
        exit(1);
    }

    fclose(shaderFile);
    return shaderSource;
}

void
checkShaderCompile(unsigned int shader, const char *shaderPath)
{
    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "shaderCompile() ERROR\n%s %s\n", shaderPath, infoLog);
        exit(1);
    }
}

void
checkProgramLink(unsigned int shaderProgram)
{
    int success;
    char infoLog[512];
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "shaderProgramLink() ERROR\n%s\n", infoLog);
        exit(1);
    }
}

unsigned int
shaderCreateProgram(const char *vertexShaderPath, const char *fragmentShaderPath)
{
    unsigned int vertexShader, fragmentShader, shaderProgram;
    char *vertexShaderSource, *fragmentShaderSource;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    vertexShaderSource = getShaderSource(vertexShaderPath);
    fragmentShaderSource = getShaderSource(fragmentShaderPath);

    glShaderSource(vertexShader, 1, (const char **)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompile(vertexShader, vertexShaderPath);

    glShaderSource(fragmentShader, 1, (const char **)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompile(fragmentShader, fragmentShaderPath);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLink(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexShaderSource);
    free(fragmentShaderSource);

    return shaderProgram;
}
