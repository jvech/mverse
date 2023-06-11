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

#ifndef __LINEAR__
#define __LINEAR__

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct {
    float matrix[4][4];
} Mat4;

typedef struct {
    float vector[3];
} Vec3;

Mat4 linearTranslatev(Vec3 translate_vector);
Mat4 linearRotatev(float degree, Vec3 rotation_axis);
Mat4 linearScalev(Vec3 scale_vector);
Mat4 linearTranslate(float translate_x, float translate_y, float translate_z);
Mat4 linearScale(float scale_x, float scale_y, float scale_z);
Mat4 linearRotate(float degree, float rotate_x, float rotate_y, float rotate_z);
Mat4 linearPerspective(float FoV, float ratio, float near, float far);
Mat4 linearOrtho(float left, float right, float bottom, float top, float near, float far);
Mat4 linearLookAt(Vec3 position, Vec3 target, Vec3 up);

Mat4 linearMat4Fill(float value);
Mat4 linearMat4Identity(float value);
Mat4 linearMat4Mul(Mat4 x1, Mat4 x2);
Mat4 linearMat4Muln(int n, ...);
Mat4 linearMat4Transpose(Mat4 x);
Mat4 linearMat4Inv(Mat4 x); //TODO
Mat4 linearMat4Add(Mat4 x1, Mat4 x2);
float linearMat4Det(Mat4 x); //TODO

Vec3 linearVec3(float x, float y, float z);
Vec3 linearVec3Normalize(Vec3 vector);
Vec3 linearVec3Add(Vec3 vector1, Vec3 vector2);
Vec3 linearVec3ScalarMulp(Vec3 vector, float scalar);
Vec3 linearVec3CrossProduct(Vec3 vector1, Vec3 vector2);
float linearVec3DotProduct(Vec3 vector1, Vec3 vector2);
#endif
