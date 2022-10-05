#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "obj.h"

enum ObjVertexType {
    OBJ_V,
    OBJ_VN,
    OBJ_VT,
    OBJ_F,
    OBJ_OTHER
};

struct Setv3 {
    float data[3];
};

struct Setv2 {
    float data[2];
};

struct Seti {
    int v, vn, vt;
};

static void readV3(char *line, struct Setv3 **v, int vIndex);
static void readV2(char *line, struct Setv2 **vn, int vtIndex);
static void readF(char *line, Mesh *mesh, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn);

// ----------------------------------------------------------------------------
static int readIndices(char *line, struct Seti *f);
static int __readIndices(char *line, struct Seti *f);
static struct Seti * __callocIndices(char *line);
static Vertex createVertex(struct Seti f, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn);
static int vertexInVertices(Vertex vertex, Vertex *vertices, int nVertices);
static void vertexAdd(Mesh *mesh, Vertex vertex);
static int vertexGetIndex(Vertex *vertices,  Vertex vertex, int nVertices);
static void indexAdd(Mesh *mesh, int index);

Mesh *
objCreateMesh(const char *filename)
{
    Mesh *mesh;
    char lineBuffer[OBJ_LINE_MAX_SIZE]; 
    FILE *fi;

    struct Setv3 *v, *vn;
    struct Setv2 *vt;
    int vSize, vtSize, vnSize;
    vSize = vtSize = vnSize = 0;

    fi = (!strcmp(filename, "-")) ? stdin : fopen(filename, "r");
    mesh = (Mesh *)calloc(1, sizeof(Mesh));

    // On error return NULL
    if (fi == NULL || mesh == NULL) {
        fprintf(stderr, "objCreateMesh() Error: %s\n", strerror(errno));
        fclose(fi);
        return NULL;
    }

    v   = (struct Setv3 *)calloc(1, vSize  * sizeof(struct Setv3));
    vt  = (struct Setv2 *)calloc(1, vtSize * sizeof(struct Setv2));
    vn  = (struct Setv3 *)calloc(1, vnSize * sizeof(struct Setv3));

    /*
    if (vSize == 0) {
        fprintf(stderr, "objCreateMesh() Error: v components not found\n");
        exit(1);
    } else if (v == NULL) {
        fprintf(stderr, "objCreateMesh() Error: %s\n", strerror(errno));
        exit(1);
    }
    */

    int vIndex, vtIndex, vnIndex;
    vIndex = vtIndex = vnIndex = 0;
    while (fgets(lineBuffer, OBJ_LINE_MAX_SIZE, fi)) {
        if      (!strncmp("f",  lineBuffer, 1))  readF (lineBuffer + 1, mesh, v, vt, vn);
        else if (!strncmp("vt", lineBuffer, 2))  readV2(lineBuffer + 2, &vt, vtIndex++);
        else if (!strncmp("vn", lineBuffer, 2))  readV3(lineBuffer + 2, &vn, vnIndex++);
        else if (!strncmp("v",  lineBuffer, 1))  readV3(lineBuffer + 1, &v,  vIndex++);
    }

    free(v);
    free(vt);
    free(vn);
    fclose(fi);

    return mesh;
}

void
readF(char *line, Mesh *mesh, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn)
{
    Vertex vertexBuffer;
    struct Seti *f;
    int i, nIndices, vi;
    f = __callocIndices(line);
    nIndices = readIndices(line, f);

    for (i = 0; i < nIndices; i++) {
        vertexBuffer = createVertex(f[i], v, vt, vn);
        if (!vertexInVertices(vertexBuffer, mesh->vertices, mesh->vertexSize))
            vertexAdd(mesh, vertexBuffer); 
        vi = vertexGetIndex(mesh->vertices, vertexBuffer, mesh->vertexSize);
        indexAdd(mesh, vi);
    }
    free(f);
}

void
readV3(char *line, struct Setv3 **v, int vIndex)
{
    int i, n;
    char *ptr;
    struct Setv3 *vptr;

    if (vIndex > 0)
        *v = (struct Setv3 *)realloc(*v, (vIndex + 1) * sizeof(struct Setv3));

    vptr = *v;
    for (i = 0, ptr = line, n = 0; i < 3; i++, ptr += n)
        sscanf(ptr, " %f%n", vptr[vIndex].data + i, &n);
}

void
readV2(char *line, struct Setv2 **v, int vIndex)
{
    int i, n;
    char *ptr;
    struct Setv2 *vptr;

    if (vIndex > 0)
        *v = (struct Setv2 *)realloc(*v, (vIndex + 1) * sizeof(struct Setv2));

    vptr = *v;
    for (i = 0, ptr = line, n = 0; i < 2; i++, ptr += n)
        sscanf(line, " %f%n", vptr[vIndex].data + i, &n);
}

int
readIndices(char *line, struct Seti *f)
{
    int i, j, n, fSize, maxVertices, triangleCount, vertexCount;
    char *ptr;

    vertexCount = 0;
    triangleCount = 0;
    for (ptr = line, n = fSize = 0; *ptr != '\n'; fSize++, ptr += n) {
        if (!(triangleCount % 3) && triangleCount != 0) {
            f[fSize] = f[fSize - 1];
            n = 0;
            triangleCount = 1;
            continue;
        } else {
            n = __readIndices(ptr, &f[fSize]);
            triangleCount++;
            vertexCount++;
        }

        if (n > 0) continue;
        fprintf(stderr, "readIndices() Error: bad format on line '%s'\n", line - 1);
        exit(1);
    }

    for (i = 0; i < fSize; i++) {
        if (f[i].v > 0) f[i].v--;
        if (f[i].vn > 0) f[i].vn--;
        if (f[i].vt > 0 && i < 2) f[i].vt--;
    }

    if (fSize == 3) return fSize;
    else if (fSize < 3) {
        fprintf(stderr, "readIndices() Error: line '%s' must have at least 3 vertices not %d\n", line - 1, fSize);
        exit(1);
    }

    maxVertices = (vertexCount - 2) * 3;
    f[fSize] = (!(fSize % 3)) ? f[fSize - 1] : f[0];
    triangleCount += (fSize % 3) ? 1: 0;

    int inc;
    inc = 2;
    for (j = i - fSize,  i = fSize + 1; i < maxVertices; i++, j+=inc) {
        if (j >= vertexCount) {
            j = 0;
            inc *= 2;
        }

        if (!(triangleCount % 3) && triangleCount != 0) {
            f[i] = f[i - 1];
            triangleCount = 1;
        } else {
            f[i] = f[j];
            triangleCount++;
        }
    }
    return maxVertices;
}

struct Seti *
__callocIndices(char *line)
{
    struct Seti *f;
    char *ptr;
    int n, fSize, i, j, k;
    for (ptr = line, n = fSize = 0; *ptr != '\n'; ptr += n) {
        if      ((sscanf(ptr, " %d/%d/%d%n", &i, &j, &k, &n) >= 3)) fSize++;
        else if ((sscanf(ptr, " %d//%d%n",   &i, &j,     &n) >= 2)) fSize++;
        else if ((sscanf(ptr, " %d/%d%n",    &i, &j,     &n) >= 2)) fSize++;
        else if ((sscanf(ptr, " %d%n",       &i,         &n) >= 1)) fSize++;
        else {
            fprintf(stderr, "__callocIndices() Error: the line '%s' format is wrong\n", line - 1);
            exit(1);
        }
    }
    f = (struct Seti *)calloc(3 * (fSize - 2), sizeof(struct Seti));
    return f;
}

int
__readIndices(char *line, struct Seti *f)
{
    int n;
    f->v = f->vt = f->vn = -1;
    if ((sscanf(line, " %d/%d/%d%n", &f->v, &f->vn, &f->vt, &n) >= 3)) return n;

    f->v = f->vt = f->vn = -1;
    if ((sscanf(line, " %d//%d%n",   &f->v, &f->vn, &n)         >= 2)) return n;

    f->v = f->vt = f->vn = -1;
    if ((sscanf(line, " %d/%d%n",    &f->v, &f->vt, &n)         >= 2)) return n;

    f->v = f->vt = f->vn = -1;
    if ((sscanf(line, " %d%n",       &f->v, &n)                 >= 1)) return n;

    return 0;
}

Vertex
createVertex(struct Seti f, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn)
{
    int i;
    Vertex out = {.position = {0, 0, 0}, .normal = {0, 0, 0}, .texCoords = {0, 0}};
    for (i = 0; i < 3; i++) {
        if (f.v != -1) out.position[i] = v[f.v].data[i];
        if (f.vn != -1) out.normal[i] = vn[f.vn].data[i];
        if (f.vt != -1 && i < 2) out.texCoords[i] = vt[f.vt].data[i];
    }
    return out;
}

int
vertexInVertices(Vertex vertex, Vertex *vertices, int nVertices)
{
    return vertexGetIndex(vertices, vertex, nVertices) + 1;
}

int
vertexGetIndex(Vertex *vertices, Vertex vertex, int nVertices) {
    int i, j, n;
    for (i = 0; i < nVertices; i++) {
        n = 0;
        for (j = 0; j < 3; j++) {
            if (vertices[i].position[j] == vertex.position[j]) n++;
            if (vertices[i].normal[j] == vertex.normal[j]) n++;
        }
        for (j = 0; j < 2; j++) {
            if (vertices[i].texCoords[j] == vertex.texCoords[j]) n++;
        }

        if (n == 8) return i;
    }
    return -1;
}

void
vertexAdd(Mesh *mesh, Vertex vertex)
{
    Vertex *v;
    int i;
    unsigned int vSize;

    v = mesh->vertices;
    vSize = mesh->vertexSize;

    if (!vSize) v = (Vertex *)calloc(1, sizeof(Vertex));
    else        v = (Vertex *)realloc(v, (vSize + 1) * sizeof(Vertex));

    if (v == NULL) {
        fprintf(stderr, "vertexAdd() Error: %s\n", strerror(errno));
        exit(1);
    }

    for (i = 0; i < 3; i++) {
        v[vSize].position[i] = vertex.position[i];
        v[vSize].normal[i]   = vertex.normal[i];
        if (i < 2)
            v[vSize].texCoords[i]   = vertex.texCoords[i];
    }
    mesh->vertices = v;
    mesh->vertexSize++;
}

void
indexAdd(Mesh *mesh, int index)
{
    unsigned int *iv, iSize;

    iv = mesh->indices;
    iSize = mesh->indexSize;

    if (!iSize) iv = (unsigned int *)calloc(1, sizeof(int));
    else        iv = (unsigned int *)realloc(iv, (iSize + 1) * sizeof(int));

    if (iv == NULL) {
        fprintf(stderr, "vertexAdd() Error: %s\n", strerror(errno));
        exit(1);
    }

    iv[iSize] = index;
    mesh->indices = iv;
    mesh->indexSize++;
}
