#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    int illum;
    float Ns;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform Material mtl;



void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = normalize(reflect(-lightDir, norm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mtl.Ns);

    vec3 ambient = dirLight.ambient * mtl.ambient;
    vec3 diffuse = diff * dirLight.diffuse * mtl.diffuse;
    vec3 specular = spec * dirLight.specular * mtl.specular;

    FragColor = vec4(mtl.diffuse, 1.0);
}
