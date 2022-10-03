# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model, view, proj;
uniform mat4 rotNormals;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
    //FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = vec3(rotNormals * vec4(aNormal, 1.0));
    //TexCoords = aTexCoords;
}
