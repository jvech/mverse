# ifndef __OBJ__
#define __OBJ__

#define OBJ_LINE_MAX_SIZE 1024
#define OBJ_MAX_NAME 512

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
    char name[OBJ_LINE_MAX_SIZE];
    float ka[3], kd[3], ks[3];
    unsigned int illum;
    float ns;
} Material;

typedef struct {
    Vertex *vertices;
    Material material;
    unsigned int *indices;
    unsigned int indexSize, vertexSize;
    unsigned int VAO, EBO, VBO;
} Mesh;

typedef struct {
    Mesh *mesh;
    unsigned int size;
} Obj;

Obj objCreate(const char *filename);
# endif
