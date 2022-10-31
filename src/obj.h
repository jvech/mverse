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

# ifndef __OBJ__
#define __OBJ__

#define OBJ_LINE_MAX_SIZE 1024
#define OBJ_MAX_WORD 512

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
