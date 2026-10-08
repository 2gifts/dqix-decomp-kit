#include <globaldefs.h>

#include "Graphics/Vector.h"
#include "World/Object3D.h"

void StoreVec3AtField0x50(unsigned char *object, int x, int y, int z);

struct FacingControllerPrefix {
    Object3D object;
    unsigned short movementFlags;
    unsigned short unknownAe;
    fix32_t targetYaw;
};

// USA: func_02041378
extern "C" ARM void func_02041378(void *object) {
    FacingControllerPrefix *controller = (FacingControllerPrefix *) object;
    fix32_t target                     = controller->targetYaw;
    fix32_t current                    = controller->object.rotation_.y;
    if (current == target) {
        return;
    }

    // The equality return ensures that one of the signed-order arms assigns distance.
    fix32_t distance;
    if (current < target) {
        distance = target - current;
        if (distance > 0x3244) {
            distance -= FIX_2PI;
        }
    } else if (target < current) {
        distance = -(current - target);
        if (distance < -0x3244) {
            distance += FIX_2PI;
        }
    }

    int turning = 0;
    if (distance > 0) {
        if (distance < 0x333) {
            distance = target;
        } else {
            distance = fix32ReduceAngle0To2Pi(current + 0x333);
        }
        controller->movementFlags |= 2;
        turning = 1;
    } else if (distance < 0) {
        if (-distance < 0x333) {
            distance = target;
        } else {
            distance = fix32ReduceAngle0To2Pi(current - 0x333);
        }
        controller->movementFlags |= 2;
        turning = 1;
    }

    if (!turning) {
        controller->movementFlags &= ~2;
    }
    StoreVec3AtField0x50((unsigned char *) object, controller->object.rotation_.x, distance, controller->object.rotation_.z);
}
