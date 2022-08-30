#ifndef __SHADER__
#define __SHADER__

unsigned int shaderCreateProgram(const char *vertexShaderPath, const char *fragmentShaderPath);
void shaderSetUniformMatrix4fv(unsigned int program, char *uniformVariable, float *data);
void shaderSetUniform1f(unsigned int program, char *uniformVariable, float data);
#endif
