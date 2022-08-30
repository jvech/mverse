#include "linear.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

Mat4
linearLookAt(Vec3 position, Vec3 target, Vec3 world_up)
{
    Mat4 out = linearMat4Identity(1.0);
    Mat4 translate;
    Vec3 cam_dir, cam_right, cam_up;
    /* position - target */
    cam_dir = linearVec3Add(position, linearVec3ScalarMulp(target, -1.0));
    cam_dir = linearVec3Normalize(cam_dir);

    cam_right = linearVec3CrossProduct(world_up, cam_dir);
    cam_right = linearVec3Normalize(cam_right);

    cam_up = linearVec3CrossProduct(cam_dir, cam_right);

    int i;
    for (i = 0; i < 3; i++) {
        out.matrix[0][i] = cam_right.vector[i];
        out.matrix[1][i] = cam_up.vector[i];
        out.matrix[2][i] = cam_dir.vector[i];
    }
    translate = linearTranslate(-position.vector[0],
                                -position.vector[1],
                                -position.vector[2]);
    return linearMat4Mul(out, translate);
}

Mat4
linearPerspective(float FoV, float ratio, float near, float far)
{
    Mat4 out = linearMat4Fill(0.0);
    float FoV_radians = FoV * M_PI / 180 / 2;
    float width = 2 * near * tanf(FoV_radians) * ratio;
    float height = 2 * near * tanf(FoV_radians);

    out.matrix[0][0] = near / width;
    out.matrix[1][1] = near / height;
    out.matrix[2][2] = -(far + near) / (far - near);
    out.matrix[2][3] = -2 * far * near / (far - near);
    out.matrix[3][2] = -1;
    return out;
}

Mat4
linearTranslate(float T_x, float T_y, float T_z)
{
    Mat4 out = linearMat4Identity(1.0);
    out.matrix[0][3] = T_x;
    out.matrix[1][3] = T_y;
    out.matrix[2][3] = T_z;
    return out;
}

Mat4
linearTranslatev(Vec3 T)
{
    return linearTranslate(T.vector[0], T.vector[1], T.vector[2]);
}

Mat4
linearScale(float S_x, float S_y, float S_z)
{
    Mat4 out = linearMat4Identity(1.0);
    out.matrix[0][0] = S_x;
    out.matrix[1][1] = S_y;
    out.matrix[2][2] = S_z;
    return out;
}

Mat4
linearScalev(Vec3 S)
{
    return linearScale(S.vector[0], S.vector[1], S.vector[2]);
}

Mat4
linearRotatev(float degree, Vec3 R_xyz)
{
    Mat4 out = linearMat4Identity(1.0);
    Vec3 R_xyz_normalized = linearVec3Normalize(R_xyz);
    float radians = degree * M_PI/180.0;
    float Rx = R_xyz_normalized.vector[0];
    float Ry = R_xyz_normalized.vector[1]; 
    float Rz = R_xyz_normalized.vector[2];
    float rcos = cosf(radians);
    float rsin = sinf(radians);

    out.matrix[0][0] = rcos + pow(Rx, 2) * (1 - rcos);
    out.matrix[0][1] = Rx * Ry * (1 - rcos) - Rz * rsin;
    out.matrix[0][2] = Rx * Rz * (1 - rcos) + Ry * rsin;

    out.matrix[1][0] = Rx * Ry * (1 - rcos) + Rz * rsin;
    out.matrix[1][1] = rcos + pow(Ry, 2) * ( 1 - rcos);
    out.matrix[1][2] = Ry * Rz * (1 - rcos) - Rx * rsin;

    out.matrix[2][0] = Rz * Rx * (1 - rcos) - Ry * rsin;
    out.matrix[2][1] = Ry * Rz * (1 - rcos) + Rx * rsin;
    out.matrix[2][2] = rcos + pow(Rz, 2) * ( 1 - rcos);
    return out;
}

Mat4
linearRotate(float degree, float Rx, float Ry, float Rz)
{
    return linearRotatev(degree, linearVec3(Rx, Ry, Rz));
}

Mat4
linearMat4Fill(float value)
{
    int i, j;
    Mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = value;
        }
    }
    return out;
}

Mat4
linearMat4Identity(float value)
{
    int i, j;
    Mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) out.matrix[i][j] = value;
            else out.matrix[i][j] = 0.0;
        }
    }
    return out;
}

Mat4
linearMat4Transpose(Mat4 x)
{
    Mat4 out;
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = x.matrix[j][i];
        }
    }
    return out;
}

Mat4
linearMat4Mul(Mat4 x1, Mat4 x2)
{
    Mat4 out;
    int i, j, k;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = 0.0;
            for (k = 0; k < 4; k++) 
                out.matrix[i][j] += x1.matrix[i][k] * x2.matrix[k][j];
        }
    }
    return out;
}

Mat4
linearMat4Muln(int n, ...)
{
    Mat4 out;

    if (n <= 0) {
        fprintf(stderr, "linearMat4Muln() Error: the specified number of args must be a positive integer greater than 0\n");
    }

    va_list(ap);
    va_start(ap, n);
    out = va_arg(ap, Mat4);

    int i;
    for (i = 1; i < n; i++) {
        out = linearMat4Mul(out, va_arg(ap, Mat4));
    }
    va_end(ap);
    return out;
}

Mat4
linearMat4Add(Mat4 x1, Mat4 x2)
{
    int i, j;
    Mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = x1.matrix[i][j] + x2.matrix[i][j];
        }
    }
    return out;
}

float
linearMat4Det(Mat4 x)
{
    return 0.0;
}

Vec3
linearVec3ScalarMulp(Vec3 x, float scalar)
{
    Vec3 out;
    int i;
    for (i = 0; i < 3; i++) {
        out.vector[i] = scalar * x.vector[i];
    }
    return out;
}

Vec3
linearVec3(float x, float y, float z)
{
    Vec3 out;
    out.vector[0] = x;
    out.vector[1] = y;
    out.vector[2] = z;
    return out;
}

Vec3
linearVec3Add(Vec3 x, Vec3 y) 
{
    Vec3 out;
    int i;
    for (i = 0; i < 3; i++) {
        out.vector[i] = x.vector[i] + y.vector[i];
    }
    return out;
}

Vec3
linearVec3Normalize(Vec3 x)
{
    Vec3 out;
    float norm = sqrtf(linearVec3DotProduct(x, x));
    int i;
    if (norm == 0) return x;
    for (i = 0; i < 3; i++) {
        out.vector[i] = x.vector[i] / norm;
    }
    return out;
}

Vec3
linearVec3CrossProduct(Vec3 x, Vec3 y)
{
    Vec3 out;
    out.vector[0] = x.vector[1] * y.vector[2] - x.vector[2] * y.vector[1];
    out.vector[1] = - x.vector[0] * y.vector[2] + x.vector[2] * y.vector[0];
    out.vector[2] = x.vector[0] * y.vector[1] - x.vector[1] * y.vector[0];
    return out;
}

float
linearVec3DotProduct(Vec3 x, Vec3 y)
{
    float out = 0;
    int i;
    for (i = 0; i < 3; i++) {
        out += x.vector[i] * y.vector[i];
    }
    return out;
}
