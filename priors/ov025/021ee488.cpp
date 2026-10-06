#include "System/Matrix.h"
#include <globaldefs.h>

extern "C" float _fflt(int v);
extern "C" float _fdiv(float a, float b);
extern "C" int _ffix(float v);

extern "C" void Mat4x4_Multiply(const Matrix4x4* inA, const Matrix4x4* inB, Matrix4x4* out);

extern const Matrix4x4 data_ov025_021ef180;
extern const Matrix4x4 data_ov025_021ef140;
extern const Matrix4x4 data_ov025_021ef100;
extern const Matrix4x4 data_ov025_021ef0c0;
extern const Matrix4x4 data_ov025_021ef080;
extern const Matrix4x4 data_ov025_021ef1c0;

// USA: func_ov025_021ee488
extern "C" ARM void func_ov025_021ee488(Vector3fix* result, const Vector3fix* points, int count, int time) {
    int frame = _ffix(_fdiv(_fflt(time), 4096.0f));
    int t = time - (frame << 12);
    int t2 = FIX32_MULTIPLY(t, t);
    int t3 = FIX32_MULTIPLY(t2, t);
    Vector3fix pos;

    if (frame == 0) {
        Matrix4x4 basis = data_ov025_021ef180;
        Matrix4x4 ctrl = data_ov025_021ef140;
        ctrl.rows[1].x = points[frame].x;
        ctrl.rows[1].y = points[frame].y;
        ctrl.rows[1].z = points[frame].z;
        ctrl.rows[2].x = points[frame + 1].x;
        ctrl.rows[2].y = points[frame + 1].y;
        ctrl.rows[2].z = points[frame + 1].z;
        ctrl.rows[3].x = points[frame + 2].x;
        ctrl.rows[3].y = points[frame + 2].y;
        ctrl.rows[3].z = points[frame + 2].z;
        Matrix4x4 blended;
        Mat4x4_Multiply(&basis, &ctrl, &blended);
        pos.x = (FIX32_MULTIPLY(t3, blended.rows[0].x) + FIX32_MULTIPLY(t2, blended.rows[1].x) + FIX32_MULTIPLY(t, blended.rows[2].x) + blended.rows[3].x) / 2;
        pos.y = (FIX32_MULTIPLY(t3, blended.rows[0].y) + FIX32_MULTIPLY(t2, blended.rows[1].y) + FIX32_MULTIPLY(t, blended.rows[2].y) + blended.rows[3].y) / 2;
        pos.z = (FIX32_MULTIPLY(t3, blended.rows[0].z) + FIX32_MULTIPLY(t2, blended.rows[1].z) + FIX32_MULTIPLY(t, blended.rows[2].z) + blended.rows[3].z) / 2;
    } else if (frame == count - 2) {
        Matrix4x4 basis = data_ov025_021ef100;
        Matrix4x4 ctrl = data_ov025_021ef0c0;
        ctrl.rows[0].x = points[frame - 1].x;
        ctrl.rows[0].y = points[frame - 1].y;
        ctrl.rows[0].z = points[frame - 1].z;
        ctrl.rows[1].x = points[frame].x;
        ctrl.rows[1].y = points[frame].y;
        ctrl.rows[1].z = points[frame].z;
        ctrl.rows[2].x = points[frame + 1].x;
        ctrl.rows[2].y = points[frame + 1].y;
        ctrl.rows[2].z = points[frame + 1].z;
        Matrix4x4 blended;
        Mat4x4_Multiply(&basis, &ctrl, &blended);
        pos.x = (FIX32_MULTIPLY(t3, blended.rows[0].x) + FIX32_MULTIPLY(t2, blended.rows[1].x) + FIX32_MULTIPLY(t, blended.rows[2].x) + blended.rows[3].x) / 2;
        pos.y = (FIX32_MULTIPLY(t3, blended.rows[0].y) + FIX32_MULTIPLY(t2, blended.rows[1].y) + FIX32_MULTIPLY(t, blended.rows[2].y) + blended.rows[3].y) / 2;
        pos.z = (FIX32_MULTIPLY(t3, blended.rows[0].z) + FIX32_MULTIPLY(t2, blended.rows[1].z) + FIX32_MULTIPLY(t, blended.rows[2].z) + blended.rows[3].z) / 2;
    } else {
        Matrix4x4 basis = data_ov025_021ef080;
        Matrix4x4 ctrl = data_ov025_021ef1c0;
        ctrl.rows[0].x = points[frame - 1].x;
        ctrl.rows[0].y = points[frame - 1].y;
        ctrl.rows[0].z = points[frame - 1].z;
        ctrl.rows[1].x = points[frame].x;
        ctrl.rows[1].y = points[frame].y;
        ctrl.rows[1].z = points[frame].z;
        ctrl.rows[2].x = points[frame + 1].x;
        ctrl.rows[2].y = points[frame + 1].y;
        ctrl.rows[2].z = points[frame + 1].z;
        ctrl.rows[3].x = points[frame + 2].x;
        ctrl.rows[3].y = points[frame + 2].y;
        ctrl.rows[3].z = points[frame + 2].z;
        Matrix4x4 blended;
        Mat4x4_Multiply(&basis, &ctrl, &blended);
        pos.x = (FIX32_MULTIPLY(t3, blended.rows[0].x) + FIX32_MULTIPLY(t2, blended.rows[1].x) + FIX32_MULTIPLY(t, blended.rows[2].x) + blended.rows[3].x) / 2;
        pos.y = (FIX32_MULTIPLY(t3, blended.rows[0].y) + FIX32_MULTIPLY(t2, blended.rows[1].y) + FIX32_MULTIPLY(t, blended.rows[2].y) + blended.rows[3].y) / 2;
        pos.z = (FIX32_MULTIPLY(t3, blended.rows[0].z) + FIX32_MULTIPLY(t2, blended.rows[1].z) + FIX32_MULTIPLY(t, blended.rows[2].z) + blended.rows[3].z) / 2;
    }

    *result = pos;
}
