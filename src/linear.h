#ifndef __LINEAR__
#define __LINEAR__

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct {
    float matrix[4][4];
} mat4;

typedef struct {
    float vector[3];
} vec3;

mat4 linearTranslatev(vec3 translate_vector);
mat4 linearRotatev(float degree, vec3 rotation_axis);
mat4 linearScalev(vec3 scale_vector);
mat4 linearTranslate(float translate_x, float translate_y, float translate_z);
mat4 linearScale(float scale_x, float scale_y, float scale_z);
mat4 linearRotate(float degree, float rotate_x, float rotate_y, float rotate_z);
mat4 linearPerspective(float FoV, float ratio, float near, float far);
mat4 linearLookAt(vec3 position, vec3 target, vec3 up);

mat4 linearMat4Fill(float value);
mat4 linearMat4Identity(float value);
mat4 linearMat4Mul(mat4 x1, mat4 x2);
mat4 linearMat4Muln(int n, ...);
mat4 linearMat4Transpose(mat4 x);
mat4 linearMat4Inv(mat4 x); //TODO
mat4 linearMat4Add(mat4 x1, mat4 x2); //TODO
float linearMat4Det(mat4 x); //TODO

vec3 linearVec3(float x, float y, float z);
vec3 linearVec3Normalize(vec3 vector);
vec3 linearVec3Add(vec3 vector1, vec3 vector2);
vec3 linearVec3ScalarMulp(vec3 vector, float scalar);
vec3 linearVec3CrossProduct(vec3 vector1, vec3 vector2);
float linearVec3DotProduct(vec3 vector1, vec3 vector2);
#endif
