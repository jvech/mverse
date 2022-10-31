#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int illum;
    float Ns;
};

uniform Material mtl;
out vec4 FragColor;

void main()
{
    FragColor = vec4(mtl.diffuse, 1.0);
}
