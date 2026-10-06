#include <globaldefs.h>

int GetBattleSubField0xbcForState(void);

extern int data_020fefcc;

struct TurnFlags_0202c04c {
    unsigned short type : 3;
    unsigned short state : 2;
    unsigned short actor : 3;
    unsigned short pad0 : 8;
    unsigned short pad1 : 5;
    unsigned short slotA : 2;
    unsigned short slotB : 2;
    unsigned short slotC : 2;
};

struct BattleTurn_0202c04c {
    char pad000[8];
    int swing;
    char pad00c[0x7b4];
    TurnFlags_0202c04c flags;
    char pad7c4[0x7fc];
    int guard;
    char padfc4[0x40];
    int actorId;
    int pendingActor;
    char pad100c[0x2c];
    char pad1038;
    signed char slotA;
    signed char slotB;
    signed char slotC;
};

// USA: func_0202c04c
ARM void InitTurnDisplayState_0202c04c(BattleTurn_0202c04c* turn) {
    if (turn->guard != 0) {
        return;
    }
    turn->flags.type = 1;
    int seq = data_020fefcc + 1;
    turn->flags.actor = (unsigned short)turn->actorId;
    data_020fefcc = seq;
    TurnFlags_0202c04c* flags = &turn->flags;
    turn->flags.state = (unsigned short)GetBattleSubField0xbcForState();

    int pending = turn->pendingActor;
    if (pending != -1) {
        flags->actor = (unsigned short)pending;
        turn->pendingActor = -1;
    }

    int swing = turn->swing;
    if (swing != 0) {
        if (swing > 0) {
            turn->swing = 1 - swing;
        } else {
            turn->swing = -1 - swing;
        }
    }

    flags->slotA = turn->slotA;
    flags->slotB = turn->slotB;
    flags->slotC = turn->slotC;
}
