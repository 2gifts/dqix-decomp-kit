#include <globaldefs.h>
#include "std_library_functions.h"
#include "Grotto/Main/FloorMap.h"

struct Obj020196fc;
struct List0201e434;
struct Struct02019f24;

extern "C" void* func_0200f374(void* dst, int count);
extern "C" void func_020c1180(void* dst);
extern "C" void func_020c1280(void* dst, int s, int c);

extern void* GetNodeAtDepth020196fc(Obj020196fc* obj, int count);
extern void* FindEntryByNameSubstr0201e434(List0201e434* list, const char* substr);
extern void* CopyStruct02019f24(Struct02019f24* dst, Struct02019f24* src);
extern void RotateTileGrid3x3(void* obj, unsigned char* buf, int mode);

struct Mat9_02019728 { unsigned int v[9]; };

struct CellInstance02019728 {
    void* node;
    unsigned char info[16];
    Mat9_02019728 matrix;
    int rotation;
    int worldX;
    int worldY;
    int worldZ;
};

// SCRATCH-USA: func_02019728
ARM int func_02019728(void* obj, int unused1, List0201e434* listArg, unsigned char* gridBuf, FloorMap* mapArg)
{
    char nameBuf[8];
    List0201e434* list = listArg ? listArg : (List0201e434*)((char*)obj + 0x6c);
    FloorMap* map = mapArg ? mapArg : (FloorMap*)((char*)obj + 0x1b4 + 0x2400);
    int n;
    int row, col;

    func_0200f374(nameBuf, 8);
    n = 0;

    for (row = 0; row < 16; row++) {
        int rowX16 = row * 16;
        int rowByteOff = rowX16 * 0x48;
        unsigned char* gridRow = gridBuf + rowX16 * 16;
        int west = -0x1000;
        int eastRot = 0x3243 >> 1;

        for (col = 0; col < 16; col++) {
            unsigned char adjPair[2];
            CellInstance02019728* cell;
            adjPair[0] = (unsigned char)map->GetAdjacencyBits(col, row);
            adjPair[1] = adjPair[0] & 0xff;
            cell = (CellInstance02019728*)((char*)(*(void**)((char*)obj + 0x420)) + rowByteOff) + col;
            Mat9_02019728 localMatrix;
            int rotation;
            int mode;
            void* entry;

            func_020c1180(&localMatrix);

            switch (adjPair[1]) {
            case 0xff:
                n = 0xc;
                sprintf(nameBuf, "F01A");
                break;
            case 0xbb: case 0xee:
                n = 3;
                sprintf(nameBuf, "W01A");
                break;
            case 0xaf: case 0xbe: case 0xeb: case 0xfa:
                n = 2;
                sprintf(nameBuf, "W02A");
                break;
            case 0xaa:
                n = 1;
                sprintf(nameBuf, "W03A");
                break;
            case 0xab: case 0xae: case 0xba: case 0xea:
                n = 0;
                sprintf(nameBuf, "W04A");
                break;
            case 0xbf: case 0xef: case 0xfb: case 0xfe:
                n = 0xd;
                sprintf(nameBuf, "E01A");
                break;
            case 0xe: case 0x38: case 0x84: case 0xe0:
                n = 0xa;
                sprintf(nameBuf, "R01A");
                break;
            case 0x3e: case 0xe3: case 0xf8:
                n = 9;
                sprintf(nameBuf, "R02A");
                break;
            case 0x2: case 0x8: case 0x20: case 0x80:
                n = 8;
                sprintf(nameBuf, "R03A");
                break;
            case 0x0:
                n = 7;
                sprintf(nameBuf, "R04A");
                break;
            case 0x22: case 0x89:
                n = 6;
                sprintf(nameBuf, "R05A");
                break;
            case 0xa: case 0x28: case 0x82: case 0xa0:
                n = 0x11;
                sprintf(nameBuf, "D01A");
                break;
            case 0x3a: case 0x8f: case 0xa3: case 0xe8:
                n = 0x10;
                sprintf(nameBuf, "D02A");
                break;
            case 0x2e: case 0x8c: case 0xb8: case 0xe2:
                n = 0xf;
                sprintf(nameBuf, "D03A");
                break;
            case 0x2a: case 0x8b: case 0xa2: case 0xa8:
                n = 0xe;
                sprintf(nameBuf, "D04A");
                break;
            }

            if (gridBuf == 0) {
                cell->node = GetNodeAtDepth020196fc((Obj020196fc*)obj, n);
                entry = FindEntryByNameSubstr0201e434(list, nameBuf);
                if (entry != 0) {
                    CopyStruct02019f24((Struct02019f24*)&cell->info, (Struct02019f24*)entry);
                }
            } else {
                entry = FindEntryByNameSubstr0201e434(list, nameBuf);
                if (entry != 0 && gridBuf != 0) {
            mode = 0;
                    CopyStruct02019f24((Struct02019f24*)(gridRow + col * 16), (Struct02019f24*)entry);
                }
            }

            rotation = 0;
            switch (adjPair[1]) {
            case 0x2: case 0x82: case 0x83: case 0x89: case 0x8b: case 0x8f:
            case 0xab: case 0xaf: case 0xe2: case 0xee: case 0xef:
                rotation = eastRot;
                func_020c1280(&localMatrix, 0x1000, 0);
                mode = 1;
                break;
            case 0x8: case 0xa: case 0xe: case 0x2a: case 0x3a: case 0x3e:
            case 0x8c: case 0xae: case 0xbe: case 0xbf:
                rotation = 0x3243;
                func_020c1280(&localMatrix, 0, west);
                mode = 2;
                break;
            case 0x20: case 0x28: case 0x2e: case 0x38: case 0xa8: case 0xba:
            case 0xe8: case 0xf8: case 0xfa: case 0xfe:
                rotation = 0x4b65;
                func_020c1280(&localMatrix, west, 0);
                mode = 3;
                break;
            case 0xff:
                break;
            default:
                rotation = 0;
                break;
            }

            if (!(gridBuf != 0)) {
                cell->rotation = rotation;
                RotateTileGrid3x3(obj, (unsigned char*)cell + 6, mode);
                cell->worldX = (int)((float)col * 8.0f * 4096.0f);
                cell->worldY = 0;
                cell->worldZ = (int)((float)row * 8.0f * 4096.0f);
                cell->matrix = localMatrix;
            } else {
                RotateTileGrid3x3(obj, gridRow + col * 16 + 2, mode);
            }
        }
    }

    return 1;
}
