/*
 * This file is part of mverse
 * Copyright (C) 2022  juanvalencia.xyz

 * mverse is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "obj.h"

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
static void readF(char *line, Mesh *mesh, int meshIndex, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn);

static Material * readMtl(char *line, const char *path, int *size);
static unsigned int useMtl(char *line, Material *mtl, unsigned int size);

/* -------------------------------------------------------------------------- */

struct Seti * readIndices(char *line, int *nIndices);
static int readIndex(char *line, struct Seti *f);
static Vertex createVertex(struct Seti f, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn);
static int vertexInVertices(Vertex vertex, Vertex *vertices, int nVertices);
static void vertexAdd(Mesh *mesh, Vertex vertex);
static int vertexGetIndex(Vertex *vertices,  Vertex vertex, int nVertices);
static void indexAdd(Mesh *mesh, int index);

/* -------------------------------------------------------------------------- */
static void getDir(char *filepath);
static void appendMtl(char *line, Material **mtl, int index);
static void readColor(char *line, float *k);


Obj
objCreate(const char *filename)
{
    Obj o;
    Mesh *mesh;
    Material *mtl;
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
        exit(1);
    }

    v   = (struct Setv3 *)calloc(1, vSize  * sizeof(struct Setv3));
    vt  = (struct Setv2 *)calloc(1, vtSize * sizeof(struct Setv2));
    vn  = (struct Setv3 *)calloc(1, vnSize * sizeof(struct Setv3));

    if (v == NULL) {
        fprintf(stderr, "objCreateMesh() Error: %s\n", strerror(errno));
        exit(1);
    }

    int n;
    char key[500];
    int vIndex, vtIndex, vnIndex;
    int mtlSize, mtlIndex, meshIndex;
    vIndex = vtIndex = vnIndex = meshIndex = 0;

    while (fgets(lineBuffer, OBJ_LINE_MAX_SIZE, fi)) {

        sscanf(lineBuffer, "%s%n", key, &n);

        if      (!strcmp("f" , key))  readF (lineBuffer + n, mesh, meshIndex, v, vt, vn);
        else if (!strcmp("vt", key))  readV2(lineBuffer + n, &vt, vtIndex++);
        else if (!strcmp("vn", key))  readV3(lineBuffer + n, &vn, vnIndex++);
        else if (!strcmp("v" , key))  readV3(lineBuffer + n, &v,  vIndex++);

        else if (!strcmp("mtllib", key)) mtl = readMtl(lineBuffer + n, filename, &mtlSize);
        else if (!strcmp("usemtl", key)) {
            mtlIndex = useMtl (lineBuffer + n, mtl, mtlSize);
            mesh = (Mesh *)realloc(mesh, (++meshIndex + 1) * sizeof(Mesh));
            memset(mesh + meshIndex, 0, sizeof(Mesh));
            mesh[meshIndex].material = mtl[mtlIndex];
            mesh[meshIndex].indexSize = 0;
        }
    }

    o.mesh = mesh;
    o.size = meshIndex + 1;
    for (int i = 1; i < o.size; i++) {
        o.mesh[i].vertices = o.mesh[0].vertices;
        o.mesh[i].vertexSize = o.mesh[0].vertexSize;
    }

    free(v);
    free(vt);
    free(vn);
    fclose(fi);

    return o;
}

void
readF(char *line, Mesh *mesh, int meshIndex, struct Setv3 *v, struct Setv2 *vt, struct Setv3 *vn)
{
    Vertex vertexBuffer;
    struct Seti *f;
    int i, nIndices, vi;
    f = readIndices(line, &nIndices);

    for (i = 0; i < nIndices; i++) {
        vertexBuffer = createVertex(f[i], v, vt, vn);
        if (!vertexInVertices(vertexBuffer, mesh->vertices, mesh->vertexSize))
            vertexAdd(mesh, vertexBuffer); 
        vi = vertexGetIndex(mesh->vertices, vertexBuffer, mesh->vertexSize);
        indexAdd(mesh + meshIndex, vi);
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

struct Seti *
readIndices(char *line, int *nIndices)
{
    struct Seti *f;
    struct Seti *buffer;
    char *ptr;
    int bufferSize;
    int n;

    buffer = (struct Seti *)calloc(3, sizeof(struct Seti));
    for (ptr = line, bufferSize = 0; *ptr != '\n'; ptr += n, bufferSize++) {
        if (bufferSize + 1 > 3)
            buffer = (struct Seti *)realloc(buffer, (bufferSize + 1) * sizeof(struct Seti));

        n = readIndex(ptr, buffer + bufferSize);

        if (buffer[bufferSize].v  != -1) buffer[bufferSize].v--;
        if (buffer[bufferSize].vt != -1) buffer[bufferSize].vt--;
        if (buffer[bufferSize].vn != -1) buffer[bufferSize].vn--;

        if (n > 0) continue;

        memset(buffer, 0, (bufferSize + 1) * sizeof(struct Seti));
        free(buffer);
        fprintf(stderr, "readIndices() Error: bad format in line '%s'", line);
        exit(1);
    }

    *nIndices = (bufferSize - 2) * 3;
    f = (struct Seti *)calloc(nIndices[0], sizeof(struct Seti));

    int i, j, k;

    for (i = j = 0, k = 1; i < bufferSize - 2; i++) {

        f[3 * i] = (j == 0) ? buffer[0] : buffer[j];

        if (j + k < bufferSize) {
            f[3 * i + 1] = buffer[j + k];
        } else {
            f[3 * i + 1] = buffer[0];
            j = 0;
            k *= 2;
            f[3 * i + 2] = buffer[j + k];
            j += k;
            continue;
        }

        f[3 * i + 2] = (j + 2 * k < bufferSize) ? buffer[j + 2 * k] : buffer[0];

        if (j + 2 * k < bufferSize) {
            j += 2 * k;
        } else {
            j = 0;
            k *= 2; // 1 2 4
        }
    }

    free(buffer);
    return f;
}

int
readIndex(char *line, struct Seti *f)
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

Material *
readMtl(char *line, const char *objFile, int *size)
{
    Material *out;
    char buffer[OBJ_LINE_MAX_SIZE], mtlFilename[OBJ_LINE_MAX_SIZE], key[500];
    char path[512], fileName[512];
    FILE *fin;
    int i, n;

    strncpy(path, objFile, 512);
    getDir(path);
    sscanf(line, " ./%s", fileName);
    sprintf(mtlFilename, "%s/%s", path, fileName);

    fin = fopen(mtlFilename, "r");
    i = 0;
    while(fgets(buffer, OBJ_LINE_MAX_SIZE, fin)) {
        sscanf(buffer, "%s%n", key, &n);
        if  (!strcmp(key, "newmtl")) appendMtl(buffer + n, &out, i++);
        if (i > 0) {
            if      (!strcmp(key, "Ka"))     readColor(buffer + n, out[i - 1].ka);
            else if (!strcmp(key, "Kd"))     readColor(buffer + n, out[i - 1].kd);
            else if (!strcmp(key, "Ks"))     readColor(buffer + n, out[i - 1].ks);
            else if (!strcmp(key, "illum"))  sscanf(buffer + n, "%u", &out[i - 1].illum);
            else if (!strcmp(key, "Ns"))     sscanf(buffer + n, "%f", &out[i - 1].ns);
        }
    }

    if (size) *size = i;
    fclose(fin);
    return out;
}

void
appendMtl(char *line, Material **mtl, int index)
{
    char name[OBJ_LINE_MAX_SIZE];

    if (index == 0)
        *mtl = (Material *)calloc(1, sizeof(Material));
    else
        *mtl = (Material *)realloc(*mtl, (index + 1) * sizeof(Material));

    sscanf(line, "%s", name);
    strncpy((*mtl + index)->name, name, OBJ_LINE_MAX_SIZE);

    if (mtl == NULL) {
        fprintf(stderr, "appendMtl() Error: %s\n", strerror(errno));
        exit(1);
    }
}

void
readColor(char *line, float *k)
{
    int ret;
    ret = sscanf(line, "%f %f %f", k, k + 1, k + 2);
    switch (ret) {
        case 1:
            k[1] = k[2] = k[0];
            break;
        case 3:
            break;
        default:
            fprintf(stderr, "readColor() Error: line '%s' invalid format", line - 2);
            exit(1);
            break;
    }
}

unsigned int
useMtl(char *line, Material *mtl, unsigned int size)
{
    int i;
    char name[OBJ_LINE_MAX_SIZE];
    for (i = 0; i < size; i++) {
        sscanf(line, "%s", name);
        if (!strcmp(name, mtl[i].name))
            return i;
    }
    return 0;
}

void
getDir(char *filepath)
{
    int i;
    for (i = strlen(filepath) - 1; i >= 0; i--) {
        switch (filepath[i]) {
            case '/':
            case '\\':
                filepath[i] = '\0';
                return;
            default:
                break;
        }
    }
    strcpy(filepath, ".");
}
