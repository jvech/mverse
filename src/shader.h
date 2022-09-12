#ifndef __SHADER__
#define __SHADER__

unsigned int shaderCreateProgram(const char *vertexShaderPath, const char *fragmentShaderPath);
void shaderSetfv(
        unsigned int program,
        char *uniformVariable,
        float *data,
        void (*uniform_callback)(int, int, const float *));

void shaderSetMatrixfv(
        unsigned int program,
        char *uniformVariable,
        float *data,
        void (*uniform_callback)(int, int, unsigned char, const float *));

void shaderSet1f(unsigned int program, char *uniformVariable, float data);
void shaderSet1i(unsigned int program, char *uniformVariable, int data);
#endif
