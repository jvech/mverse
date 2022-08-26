#include "linear.h"
#include <math.h>

mat4
linearLookAt(vec3 position, vec3 target, vec3 world_up)
{
    mat4 out = linearMat4Identity();
    mat4 translate;
    vec3 cam_dir, cam_right, cam_up;
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

mat4
linearPerspective(float FoV, float ratio, float near, float far)
{
    mat4 out = linearMat4Fill(0.0);
    float FoV_radians = FoV * M_PI / 180;
    float width = near * tanf(FoV_radians) * ratio;
    float height = near * tanf(FoV_radians);

    out.matrix[0][0] = near / width;
    out.matrix[1][1] = near / height;
    out.matrix[2][2] = -(far + near) / (far - near);
    out.matrix[2][3] = -2 * far * near / (far - near);
    out.matrix[3][2] = -1;
    return out;
}

mat4
linearTranslate(float T_x, float T_y, float T_z)
{
    mat4 out = linearMat4Identity();
    out.matrix[0][3] = T_x;
    out.matrix[1][3] = T_y;
    out.matrix[2][3] = T_z;
    return out;
}

mat4
linearTranslatev(vec3 T)
{
    return linearTranslate(T.vector[0], T.vector[1], T.vector[2]);
}

mat4
linearScale(float S_x, float S_y, float S_z)
{
    mat4 out = linearMat4Identity();
    out.matrix[0][0] = S_x;
    out.matrix[1][1] = S_y;
    out.matrix[2][2] = S_z;
    return out;
}

mat4
linearScalev(vec3 S)
{
    return linearScale(S.vector[0], S.vector[1], S.vector[2]);
}

mat4
linearRotate(float degree, vec3 R_xyz)
{
    mat4 out = linearMat4Identity();
    vec3 R_xyz_normalized = linearVec3Normalize(R_xyz);
    float radians = degree * M_PI/180.0;
    float Rx = R_xyz_normalized.vector[0];
    float Ry = R_xyz_normalized.vector[1]; 
    float Rz = R_xyz_normalized.vector[2];
    float rcos = cosf(radians);
    float rsin = sinf(radians);

    out.matrix[0][0] = rcos + pow(Rx, 2) * ( 1 - rcos);
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

mat4
linearMat4Fill(float value)
{
    int i, j;
    mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = value;
        }
    }
    return out;
}

mat4
linearMat4Identity()
{
    int i, j;
    mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) out.matrix[i][j] = 1.0;
            else out.matrix[i][j] = 0.0;
        }
    }
    return out;
}

mat4
linearMat4Transpose(mat4 x)
{
    mat4 out;
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = x.matrix[j][i];
        }
    }
    return out;
}

mat4
linearMat4Mul(mat4 x1, mat4 x2)
{
    mat4 out;
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

mat4
linearMat4Add(mat4 x1, mat4 x2)
{
    int i, j;
    mat4 out;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            out.matrix[i][j] = x1.matrix[i][j] + x2.matrix[i][j];
        }
    }
    return out;
}

float
linearMat4Det(mat4 x)
{
    return 0.0;
}

vec3
linearVec3ScalarMulp(vec3 x, float scalar)
{
    vec3 out;
    int i;
    for (i = 0; i < 3; i++) {
        out.vector[i] = scalar * x.vector[i];
    }
    return out;
}

vec3
linearVec3(float x, float y, float z)
{
    vec3 out;
    out.vector[0] = x;
    out.vector[1] = y;
    out.vector[2] = z;
    return out;
}

vec3
linearVec3Add(vec3 x, vec3 y) 
{
    vec3 out;
    int i;
    for (i = 0; i < 3; i++) {
        out.vector[i] = x.vector[i] + y.vector[i];
    }
    return out;
}

vec3
linearVec3Normalize(vec3 x)
{
    vec3 out;
    float norm = sqrtf(linearVec3DotProduct(x, x));
    int i;
    if (norm == 0) return x;
    for (i = 0; i < 3; i++) {
        out.vector[i] = x.vector[i] / norm;
    }
    return out;
}

vec3
linearVec3CrossProduct(vec3 x, vec3 y)
{
    vec3 out;
    out.vector[0] = x.vector[1] * y.vector[2] - x.vector[2] * y.vector[1];
    out.vector[1] = - x.vector[0] * y.vector[2] + x.vector[2] * y.vector[0];
    out.vector[2] = x.vector[0] * y.vector[1] - x.vector[1] * y.vector[0];
    return out;
}

float
linearVec3DotProduct(vec3 x, vec3 y)
{
    float out = 0;
    int i;
    for (i = 0; i < 3; i++) {
        out += x.vector[i] * y.vector[i];
    }
    return out;
}
