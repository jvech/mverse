# ifndef __OBJ__
#define __OBJ__

#define OBJ_LINE_MAX_SIZE 1024

typedef struct {
    float position[3];
    float normal[3];
    float texCoords[2];
} Vertex;

typedef struct {
    unsigned int id;
    char type[50];
} Texture;

typedef struct {
    Vertex *vertices;
    Texture *textures;
    unsigned int *indices;
    unsigned int indexSize, vertexSize;
    unsigned int VAO, EBO, VBO;
} Mesh;

Mesh * objCreateMesh(const char *filename);
# endif
