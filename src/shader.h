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
