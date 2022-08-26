# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
uniform mat4 trans;

out vec3 vertexColor;

void main()
{
    gl_Position = trans * vec4(aPos, 1.0f);
    vertexColor = aColor;
}
