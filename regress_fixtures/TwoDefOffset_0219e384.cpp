#include <globaldefs.h>

struct BattleStruct;
struct Vec3 { int v[3]; };

struct Seq0219e384 {
	unsigned char unk0;
	unsigned char done;
	unsigned char unk2[6];
	unsigned char state;
	unsigned char pending;
	unsigned char active;
	unsigned char expired;
	unsigned char unkc[2];
	unsigned short flagsE;
	unsigned short flags10;
	short v12;
	short v14;
	short v16;
	short v18;
	short v1a;
	short v1c;
	unsigned char v1e;
	unsigned char unk1f;
	unsigned char v20;
	unsigned char unk21;
	unsigned short timer;
	unsigned short warp;
	unsigned char unk26[2];
	int accum;
};

struct Field3f8 {
	unsigned short mapId;
	unsigned char unk2[8];
	unsigned char active;
	unsigned char unkb;
	unsigned char mode;
	unsigned char flagD;
	unsigned char flagE;
	unsigned char unkf;
	int vec10[3];
	short s1c;
	short s1e;
	int f20;
	int f24;
	int f28;
	int f2c;
	unsigned char unk30[0x33];
	unsigned char f63;
	unsigned char f64;
	unsigned char unk65[2];
	unsigned char f67;
	unsigned char unk68;
	unsigned char f69;
	unsigned char unk6a[2];
	short f6c;
	unsigned char flags6e;
};

struct Combatant0219e384 {
	short GetId() const {
		short id = s4;
		return id;
	}

	unsigned char unk0[4];
	short s4;
	unsigned char unk6[0x3e];
	int vec44[3];
	unsigned char unk50[4];
	int f54;
	unsigned char unk58[0x60];
	unsigned short b8;
	unsigned char unkba[4];
	unsigned char be;
	unsigned char unkbf[3];
	unsigned char c2;
	unsigned char unkc3[0x6d];
	unsigned int* p130;
	unsigned char unk134[0x2c];
	unsigned char unk160[6];
	unsigned short f166;
	unsigned char unk168[2];
	unsigned short f16a;
	unsigned char unk16c[0x46];
	unsigned short f1b2;
};

struct Npc0219e384 {
	unsigned char unk0[0xbe];
	unsigned char be;
	unsigned char unkbf[0x71];
	int mode;
	unsigned char unk134[0x32];
	unsigned short f166;
	unsigned char unk168[2];
	unsigned short f16a;
};

static inline int GetNpcMode(struct Npc0219e384* c) {
	return *(int*)((char*)c + 0x130);
}

struct HdrFlags4 {
	unsigned char noSkip : 1;
	unsigned char rest : 7;
};

struct GrottoRun {
	unsigned short mapId;
	unsigned short subId;
	unsigned char unk4[4];
	unsigned char* header;
	unsigned char unkc[0x28];
	short f34;
	unsigned char unk36[0x3ee];
	int f424;
};

struct GrottoSlot {
	unsigned char unk0[2];
	unsigned char used;
	unsigned char unk3[5];
	void* entry;
	unsigned char unkc[5];
	unsigned char kind;
	unsigned short value;
	unsigned char unk14[4];
};

struct InlineEntry {
	unsigned char unk0[2];
	unsigned short slotIndex : 2;
	unsigned short unk2b : 2;
	unsigned short group : 12;
	unsigned short f4;
	unsigned char unk6[7];
	unsigned char fd;
	unsigned char unke[0xa];
	int vec18[3];
	unsigned char unk24[0x10];
	int f34;
};

struct GrottoIds {
	unsigned char unk0[0xf78];
	unsigned char ids[4];
	unsigned char count;
	unsigned char flagF7d;
};

struct Msg021b6e70 {
	unsigned char kind;
	unsigned char actor;
	unsigned short id;
	unsigned short value;
	unsigned short f6;
	int f8;
	unsigned char fc;
	unsigned char fd;
	unsigned char fe;
	unsigned char ff;
	unsigned char f10;
	unsigned char f11;
	unsigned char f12;
	unsigned char f13;
	int f14;
};

struct NodeScratch {
	unsigned char unk0[0xc];
	int mapId;
	unsigned char unk10[0x24];
};

struct WorkLo {
	unsigned int size;
	unsigned int fileSize;
	void* buf;
	struct Msg021b6e70 msgF63;
	struct Msg021b6e70 msgF64;
	struct Vec3 warpVec;
	struct Vec3 memberVec;
	struct Vec3 fieldVec;
	struct Vec3 deltaVec;
};

struct WorkHi {
	struct Vec3 actorVec;
	signed char slot;
	unsigned char unk79[0x2b];
	struct NodeScratch node;
	struct Msg021b6e70 msg;
	struct Vec3 entryVec;
	struct Vec3 baseVec;
	char name[0x10];
	struct NodeScratch turnNode;
};

struct ListNode021bba08 {
	unsigned char unk0[2];
	unsigned char flag2;
	unsigned char unk3[5];
	unsigned short f8;
	unsigned char unka[0x91];
	unsigned char f9b;
	unsigned char f9c;
};

struct SafeAllocatorObj { unsigned char unk0[0x28]; };
struct Struct44C8;
struct SearchStruct;
struct BackgroundLoader;
struct SeqSlot { unsigned short a; unsigned short b; };

extern "C" struct BattleStruct* _ZN9GameState11GetInstanceEv(void);
extern "C" struct Struct44C8* func_ov017_0218b5b0(void);
extern "C" void* _ZN9GameState14GetProtagonistEv(struct BattleStruct* battle);
extern "C" struct Combatant0219e384* _ZN9GameState20GetUnknownGameObjectEv(struct BattleStruct* battle);
extern "C" struct Field3f8* _Z20GetField0x3f8AddressP9GameState(struct BattleStruct* battle);
extern "C" struct BackgroundLoader* _ZN16BackgroundLoader11GetInstanceEv(void);
extern "C" struct GrottoRun* func_02012fe4(void);
void* GetPtrField0x468(void* battle);
struct InlineEntry* GetEntryTableBase(void);
extern "C" void* func_0205ec34(void);
extern "C" struct GrottoIds* _Z17GetPtrField0x2a04P9GameState(struct BattleStruct* battle);
extern "C" struct SearchStruct* func_0202ae18(void);
extern "C" char* _ZN24LootableContainerManager15GetMainInstanceEv(void);
void* GetDataPtr02114e04_020d6c00(void);
extern "C" int* _ZN15LightingManager11GetInstanceEv(void);
unsigned char* GetFieldPtrAt0x26c(void* obj);
int GetByte0x26c(char* obj);
extern "C" void _ZN8Vector3iaSERKS_(int* dst, int* src);
extern "C" void func_02097cac(void* obj);
extern "C" void _Z34HandleGrottoTurnTransition0208f944P11Obj0208f944(void* obj);
void ConfigureFieldVec3ForDateWindow(void* obj, int a, int b);
extern "C" void* _Z22FindEntryByHalfwordKeyP11SearchTablei(void* table, int key);
extern "C" int func_0209cd50(int id);
extern "C" int _Z14GetShortAt0xb8P23ShortField0xb8_0209cae8(void* obj);
extern "C" void _Z30DispatchContextByState0209c678P13Actor0209c678i(void* actor, int v);
extern "C" int _Z29IsScaledFieldPositive0203adfcPvS_(void* a, void* b);
extern "C" void _Z22ScaleAndNotify0203ad88PvS_i(void* a, void* b, int c);
extern "C" int _Z13IsAxisIntZeroP18AxisFloats0203b5f8i(void* obj, int axis);
extern "C" int _Z29GetBrightnessTransitionStatesP13GameResources(int* obj);
extern "C" int _Z15GetAxisIntValueP18AxisFloats0203b57ci(void* obj, int axis);
extern "C" void _Z17SetMainBrightnessP13GameResourcesii(void* obj, int a, int b);
extern "C" void _Z13SetBrightnessP13GameResourcesii(void* obj, int a, int b);
extern "C" void _Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(void* obj, int a, int b);
extern "C" void _Z27CancelPendingAction020397ccP11Obj020397cci(void* obj, int a);
extern "C" void func_ov017_021c4664(int mapId, struct Vec3 v, int w);
extern "C" int func_0202c540(void* search);
extern "C" int _Z19GetField0x397cValueP9GameState(struct BattleStruct* battle);
extern "C" void _Z27EnqueueEventTag150_021d035chhh(unsigned char a, unsigned char b, unsigned char c);
extern "C" void _Z26EnqueueEventTag31_021d257chi(unsigned char id, int a);
extern "C" int _Z28LookupAndForEachNode020649b0PviS_(void* obj, int kind, void* out);
extern "C" void func_0206f81c(void* node);
extern "C" int _Z28IsBrightnessTransitionActiveP13GameResources(int* obj);
int CheckSubstructByte0x7cPositive(signed char* obj);
extern "C" void func_020531f0(void* obj);
extern "C" int _ZN16BackgroundLoader17GetNumQueuedTasksEv(struct BackgroundLoader* self);
int IsField0Null(void** obj);
void* GetGlobalContext020daf90(void);
extern "C" int _Z14ListContainsIdP16ListHead02046b60i(void* list, int id);
int GetField0x50(void* obj);
extern "C" void _Z33DispatchWithGlobalContext020daf9ciiii(int a, int b, int c, int d);
int CheckField0NonZero(int* obj);
extern "C" void* _Z30GetSearchStructCurrentArrEntryP20SearchStruct0202c1a4(struct SearchStruct* search);
extern "C" int _Z15GetBitsInField0Pjj(void* obj, unsigned int mask);
extern "C" int _ZNK9GameState21GetEffectiveDeltaTimeEv(struct BattleStruct* battle);
extern "C" void _Z15SetFlagBitAt0xeP12SearchStructi(struct SearchStruct* search, int v);
extern "C" void _Z15SetBitsInField4Pjj(void* obj, unsigned int mask);
extern "C" void _Z16OrBitsIntoField0Pjj(void* obj, unsigned int mask);
extern "C" void _Z13SetBitsInWordPjj(void* obj, unsigned int mask);
extern "C" void func_ov017_0219d250(void* obj, unsigned char v);
int CountEntriesType2WithIds(int a, int b);
extern "C" void func_ov017_0219c598(int a, int b, int c);
extern "C" int func_02028a54(struct InlineEntry* base, int id);
extern "C" struct InlineEntry* _Z19FindInlineEntryByIdP14Entry_02028bd0i(struct InlineEntry* base, int id);
extern "C" signed char* _ZN9GameState15GetGrottoStructEv(struct BattleStruct* battle);
int IsInRange0201b588(int v);
extern "C" void _Z19InitStruct_021b46d8P14Struct021b46d8(struct GrottoSlot* slot);
extern "C" int _Z22IsValueInRange0201b5d8i(int v);
extern "C" unsigned char* _ZN17ActiveGrottoClass15GetDetailedDataEv(void* self);
extern "C" unsigned short _ZNK23DetailedTreasureMapData17LegacyBossMapData26MaybeGetCurrentAlternateIDEv(void* self);
extern "C" void _Z16AppendNodeToTailP16TailList020469b4P16TailNode020469b4(void* list, void* node);
extern "C" int func_0202c508(void* search);
extern "C" void func_ov017_021c4854(struct GrottoRun* run, int a, int b, int* vec, int c, int d);
extern "C" void* _ZN9GameState21GetPartyMemberByIndexEi(struct BattleStruct* battle, int index);
extern "C" int _Z23GetByteFieldMasked0x1cePvi(void* obj, int mask);
extern "C" void func_ov017_021bd4d0(void* obj, unsigned char index, int a, int b);
extern "C" void func_ov017_021ce400(unsigned char v, int mapId);
extern "C" void* func_0208e0a8(void);
extern "C" void func_0208e520(void* a, void* b);
extern "C" void _ZN13SafeAllocator5ResetEv(void* self);
extern "C" void _ZN24LootableContainerManager5ResetEv(char* p);
extern "C" int _ZN16BackgroundLoader13QueueLoadFileEPKcP13SafeAllocator(struct BackgroundLoader* self, const char* name, void* alloc);
extern "C" int _ZN16BackgroundLoader13GetTaskStatusEi(struct BackgroundLoader* self, int id);
extern "C" int _ZN16BackgroundLoader21GetDetailedTaskStatusEi(struct BackgroundLoader* self, int id);
extern "C" void _ZN16BackgroundLoader17GetLoadedFileByIDEiPPvPj(struct BackgroundLoader* self, int id, void** buf, unsigned int* size);
extern "C" void func_0201afd0(struct GrottoRun* run, void* buf, unsigned int size, int a);
extern "C" int sprintf(char* dst, const char* fmt, ...);
extern "C" char* strcpy(char* dst, const char* src);
extern "C" void _ZN24LootableContainerManager18LoadZoneContainersEPKvjPKcP13SafeAllocator(char* region, void* buf, unsigned int size, char* name, int a);
extern "C" void func_0201647c(struct GrottoRun* run);
extern "C" void _ZN16BackgroundLoader10RemoveTaskEi(struct BackgroundLoader* self, int id);
extern "C" int _Z19IsIdInRange020981e4ii(void* obj, int id);
extern "C" int _Z17IsIdInSet02098210ii(void* obj, int id);
void* GetData02109020(void);
extern "C" signed char* _Z15GetFieldIfFlag4Pc(struct BattleStruct* battle);
extern "C" void _Z27SetStateAndDispatch0209c3b4P13Actor0209c3b4i(void* actor, int v);
extern "C" int _Z26AreListedCombatantsBit0SetP11Obj02086b98(void* obj);
extern "C" void _Z27SetFlag0x2AndNotify0209caa0P11Obj0209caa0i(void* obj, int v);
extern "C" void _Z36ClassifyPriceCategoryAndNotifyBattlePhi(void* obj, int v);
extern "C" void _Z18ClearFlags020466f4P16FlagWord020466f4j(void* obj, unsigned int mask);
extern "C" void _Z23ClearFieldAt0xc0208f3a8P11Obj0208f3a8(void* obj);
extern "C" void func_0206461c(void* a, void* b);
void* GetGlobal02109418(void);
extern "C" void func_02095b30(void* obj, int v);
extern "C" void _Z31SetElementFlagsFromMask02013cb0P9S02013cb0j(struct GrottoRun* run, unsigned int mask);
extern "C" void _Z33UpdateFlaggedType2Entries02018c74Phj(struct GrottoRun* run, unsigned int mask);
extern "C" void func_02097ec8(void* obj);
extern "C" void func_ov017_021a2fa0(void* ov);
extern "C" void func_ov017_0219e180(void* ov, int mapId);
extern "C" void func_0208f168(void* obj, void* alloc);
extern "C" void _ZN8Object3D10SetField06Et(void* obj, int v);
extern "C" void func_0208fc30(void* obj);
extern "C" void _Z21ClearField0x8020733c8P11Obj020733c8(struct InlineEntry* entry);
extern "C" int func_02028460(int* vec, int* other);
extern "C" void _Z17ClearBitsInField4Pjj(void* obj, unsigned int mask);
extern "C" void _Z15ClearBitsInWordPjj(void* obj, unsigned int mask);
extern "C" void _Z34SetupCombatantAllocations_021901acv(void* ov);
extern "C" void func_020a6aac(void* ov, int a, int b);
extern "C" void func_020acecc(void* ov, int a, int b);
extern "C" int _ZNK9GameState12GetTimeOfDayEv(struct BattleStruct* battle);
extern "C" int _Z30FindAndApplyEntryVec3_021a01f4PviiS_Pi(void* ov, int mapId, int index, void* combatant, int* out);
extern "C" void func_ov017_0219fc88(void* self, int v);
extern "C" void func_ov017_021b6f18(void* node);
extern "C" void _Z19InitStruct_02196c08Ph(struct Msg021b6e70* fnMsg);
extern "C" int _Z18GetField0x3acValueP9GameState(struct BattleStruct* battle);
extern "C" void func_ov017_021b6e70(struct Msg021b6e70* fnMsg, int v);
extern "C" void func_ov017_021b7104(void* node, struct Msg021b6e70* fnMsg);
extern "C" int _Z16Dispatch020649f4PvS_(void* obj, void* out);
extern "C" void _ZN16BackgroundLoader13AddLockGlobalEv(void);
extern "C" int _Z18LoadFileIntoMemoryPKcPvPj(const char* name, void* dst, unsigned int* size);
extern "C" int _Z24InitStreamAndRun0208dc00P12StreamHeaderii(void* header, int a, int b);
extern "C" int _Z18TestBitInByteArrayiPhi(int a, unsigned char* arr, int bit);
extern "C" void func_ov017_021bba08(struct ListNode021bba08* node, void* header);
extern "C" void _Z20SetOrClearBitInArrayPvPhii(void* a, unsigned char* arr, int bit, int set);
extern "C" void _ZN16BackgroundLoader16RemoveLockGlobalEv(void);
extern "C" void _Z27EnqueueEventTag158_021d23bct(unsigned short v);
extern "C" void _Z13SetField0x246Pvs(void* obj, short v);
extern "C" void _Z33SetVec3ComponentsAndApply020a27ccP14Struct020a27cc(void* obj);
extern "C" void _Z29ConfigureVec3AndApply020a2800P14Struct020A2800(void* obj);
extern "C" void _Z25InitVec3ParamsVariant8b33Pv(void* obj);
extern "C" void _Z33SetVec3ComponentsAndApply020a2868P14Struct020a2868(void* obj);
extern "C" int _Z13GetField0x218P9S020a3570(void* obj);
extern "C" void Vector3fix_Subtract(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern "C" void Vector3fix_Add(struct Vec3* a, struct Vec3* b, struct Vec3* c);
extern "C" void _Z34CopyVec3ToField0x10AndCall0202e5d8PcPi(void* obj, int* vec);
extern "C" void func_ov017_021982ac(void* ov);
extern "C" void _Z22ApplyVecFromField0x246Pc(void* obj);
extern "C" void _Z29SetCombatModeFromCase020dc2d0i(int v);
extern "C" void _Z14SetByteAt0x2c5P23ByteField0x2c5_020a2cbch(void* obj, int v);
extern "C" void func_020ae53c(int v);
extern "C" void _Z23NotifyOv017OnFlag0x1130v(void);
extern "C" void _Z32MarkActiveCombatantSlots02026b7cPh(void* obj);
extern "C" void _Z37SyncVec3ForFlaggedCombatants_021a3338v(void* ov);
void* GetGlobal02109400(void);
extern "C" void func_02094ab0(void* obj);
extern "C" void _Z21BlankFunction02094b34v(void* obj, int a, int b, int c, int d);
extern "C" int _Z18AlwaysTrue02094b4cv(void* obj);
extern "C" void _Z29InitAndAppendState61_021a65c4Pvhi(void* ov, unsigned char a, int b);
extern "C" void _Z27EnqueueEventTag147_021cdaa0v(void);
extern "C" void _Z17SetByteField0x253Pv(void* obj);
extern "C" void _ZN8Object3D10EnableFlagEi(void* obj, unsigned int mask);
extern "C" void _Z26EnqueueEventTag35_021d2ad0hiiih(short id, int x, int y, int z, unsigned char f);
void* GetGlobalField0x1c020421a0(void);
extern "C" void func_ov017_021a7a74(void* ov, struct Msg021b6e70* fnMsg);
extern "C" struct Combatant0219e384* _Z25GetCombatantWithFlag0x100P9GameStatei(struct BattleStruct* battle, int index);
extern "C" struct Npc0219e384* _ZN9GameState27GetMaybeFieldMonsterByIndexEi(struct BattleStruct* battle, int index);
extern "C" int _ZNK8Object3D9IsVisibleEv(void* obj);
extern "C" int _ZNK8Object3D10GetField06Ev(void* obj);
int TrySetMode02076ccc(void* obj, int mode);
extern "C" void func_020a72ac(void* obj, unsigned char v);
extern "C" void VectorizedMemset(void* dst, int v, int size);
void* GetGlobal02109030(void);
extern "C" void func_02094030(void* obj, int a, int b, int c);

extern int data_02109bf4;
extern int data_02108760;
extern const char data_ov017_021d75fc[];
extern int data_ov017_021d75b8;
extern const char data_ov017_021d7619[];
extern const char data_ov017_021d761e[];
extern char data_0211e33c[];

// USA: func_ov017_0219e384
extern "C" ARM void func_ov017_0219e384(struct Seq0219e384* self, void* list) {
	struct BattleStruct* battle = _ZN9GameState11GetInstanceEv();
	char* ov = (char*)func_ov017_0218b5b0();
	void* c3ac = _ZN9GameState14GetProtagonistEv(battle);
	struct Combatant0219e384* actor = _ZN9GameState20GetUnknownGameObjectEv(battle);
	struct Field3f8* fld = _Z20GetField0x3f8AddressP9GameState(battle);
	struct BackgroundLoader* loader = _ZN16BackgroundLoader11GetInstanceEv();
	struct GrottoRun* run = func_02012fe4();
	char* runSub = (char*)run + 0x23ec;
	void* table468 = GetPtrField0x468(battle);
	struct InlineEntry* entryTable = GetEntryTableBase();
	void* obj5ec34 = func_0205ec34();
	struct GrottoIds* ids = _Z17GetPtrField0x2a04P9GameState(battle);
	struct SearchStruct* search = func_0202ae18();
	char* region = _ZN24LootableContainerManager15GetMainInstanceEv();
	void* flags14e04 = GetDataPtr02114e04_020d6c00();
	int* data7930 = _ZN15LightingManager11GetInstanceEv();
	struct WorkHi workHi;
	struct WorkLo workLo;

	if (self->state == 0) {
		unsigned char* byte26c = GetFieldPtrAt0x26c(actor);
		if (GetByte0x26c((char*)actor) != 0 && byte26c[1] != 2 && fld->f6c != 9999) {
			if (fld->f67 != 0) {
				_ZN8Vector3iaSERKS_(fld->vec10, actor->vec44);
			}
			return;
		}
		{
			int delta;
			int axis;
			int limit;
			void* obj;
			int runMapId;
			int cnt;
			unsigned short* warpSlot;
			void* found;
			unsigned char* hdr;

			func_02097cac((char*)run + 0x840);
			_Z34HandleGrottoTurnTransition0208f944P11Obj0208f944(runSub);
			runMapId = run->mapId;
			ConfigureFieldVec3ForDateWindow(ov, fld->mapId, runMapId);
			found = _Z22FindEntryByHalfwordKeyP11SearchTablei(table468, fld->mapId);
			hdr = run->header;
			self->active = 1;
			if (found != 0 && hdr != 0) {
				if (func_0209cd50(fld->mapId) == _Z14GetShortAt0xb8P23ShortField0xb8_0209cae8(&data_02109bf4)) {
					self->active = 0;
				}
			}
			if (fld->f20 > 0) {
				self->active = 0;
			}
			if (self->active != 0) {
				_Z30DispatchContextByState0209c678P13Actor0209c678i(&data_02109bf4, 0);
			}
			if (_Z29IsScaledFieldPositive0203adfcPvS_(&data_02108760, 0) != 0) {
				_Z22ScaleAndNotify0203ad88PvS_i(&data_02109bf4, 0, 0);
				self->active = 1;
			}
			cnt = *(unsigned short*)((char*)actor->p130 + 4);
			limit = 15;
			if (cnt <= 0) {
				limit = 60;
			}
			delta = -16;
			axis = 1;
			if (_Z13IsAxisIntZeroP18AxisFloats0203b5f8i(ov, 0) == 0 &&
			    _Z13IsAxisIntZeroP18AxisFloats0203b5f8i(ov, 1) == 0 &&
			    _Z29GetBrightnessTransitionStatesP13GameResources((int*)ov) != 0x101) {
				axis = 0;
				if (_Z15GetAxisIntValueP18AxisFloats0203b57ci(ov, axis) > 0) {
					delta = 16;
				}
				limit = 0;
			}
			if ((fld->flags6e & 0x10) == 0) {
				if ((fld->flags6e & 1) != 0) {
					_Z17SetMainBrightnessP13GameResourcesii(ov, delta, limit);
				} else {
					_Z13SetBrightnessP13GameResourcesii(ov, delta, limit);
				}
			}
			if (fld->mode == 1) {
				_Z28DispatchWithShortB4_0205eaa0P11Obj0205eaa0ii(&data_02108760, 15, 0);
			}
			_Z27CancelPendingAction020397ccP11Obj020397cci(actor, 1);
			actor->c2 &= ~0x40;
			warpSlot = (unsigned short*)(ov + 0xae);
			warpSlot = (unsigned short*)((char*)warpSlot + 0x4400);
			*(unsigned short*)(ov + 0x44ae) = 0;
			warpSlot[1] = 0;
			func_ov017_021c4664(fld->mapId, *(struct Vec3*)fld->vec10, fld->s1c);
			if (func_0202c540(search) != 0 && fld->f67 != 0 && *(unsigned short*)(ov + 0x44ac) != 0) {
				*(unsigned short*)(ov + 0x44ac) = 0;
			}
			if (fld->flagE != 0) {
				_Z27EnqueueEventTag150_021d035chhh(0, _Z19GetField0x397cValueP9GameState(battle), 1);
			}
			runMapId = run->mapId;
			if (fld->mapId != runMapId) {
				struct GrottoIds* list2a04 = _Z17GetPtrField0x2a04P9GameState(battle);
				int i;
				for (i = 0; i < list2a04->count; i++) {
					_Z26EnqueueEventTag31_021d257chi(list2a04->ids[i], 0);
				}
			}
			if (*(unsigned char*)(ov + 0x42e9) != 0) {
				fld->active = 0;
			}
			obj = func_0205ec34();
			workHi.turnNode.mapId = run->mapId;
			if (fld->active == 0 && _Z28LookupAndForEachNode020649b0PviS_(obj, 0x1e, &workHi.turnNode) != 0) {
				func_0206f81c(&workHi.turnNode);
			}
			if (axis == 0) {
				self->state = 2;
				return;
			}
			self->state = 1;
		}
		return;
	}
	if (self->state == 1) {
		if (_Z28IsBrightnessTransitionActiveP13GameResources((int*)ov) == 0) {
			self->state = 2;
		}
		return;
	}
	if (self->state == 2) {
		void* context;
		void** tail;
		if (actor != 0 && CheckSubstructByte0x7cPositive((signed char*)actor) != 0) {
			func_020531f0(actor);
		}
		if (_ZN16BackgroundLoader17GetNumQueuedTasksEv(loader) > 0) {
			return;
		}
		if (IsField0Null(*(void***)(ov + 0x3700)) == 0) {
			return;
		}
		context = GetGlobalContext020daf90();
		tail = *(void***)(ov + 0x3704);
		if (IsField0Null(tail) == 0) {
			if (_Z14ListContainsIdP16ListHead02046b60i(tail, 0x4c) == 0) {
				return;
			}
			if (GetField0x50(context) != 0) {
				return;
			}
			_Z33DispatchWithGlobalContext020daf9ciiii(1, 0, 1, 0);
			_Z13SetBrightnessP13GameResourcesii(ov, -16, 0);
			return;
		}
		if (CheckField0NonZero((int*)search) != 0 &&
		    _Z30GetSearchStructCurrentArrEntryP20SearchStruct0202c1a4(search) != 0 &&
		    _Z15GetBitsInField0Pjj(ov, 2) == 0) {
			self->accum = self->accum + _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);
			if (self->accum > 15000) {
				self->expired = 1;
				_Z15SetFlagBitAt0xeP12SearchStructi(search, 0);
			}
			if (self->expired == 0) {
				return;
			}
			self->accum = 0;
		}
		if (fld->flags6e != 1) {
			_Z15SetBitsInField4Pjj(ov, 1);
			_Z16OrBitsIntoField0Pjj(flags14e04, 1);
		}
		_Z13SetBitsInWordPjj(ov, 0x10);
		func_ov017_0219d250(ov, self->pending);
		self->pending = 0;
		self->state = 3;
		return;
	}
	if (self->state == 3) {
		struct InlineEntry* table;
		int found;
		struct InlineEntry* entry;
		signed char* grotto;
		unsigned char kind;
		unsigned short value;
		char* runSub2;
		int hasFlag;

		if (CountEntriesType2WithIds(0x64, 15) > 0) {
			return;
		}
		if (run->f424 != 0) {
			return;
		}
		if (_ZN16BackgroundLoader17GetNumQueuedTasksEv(loader) > 0) {
			return;
		}
		func_ov017_0219c598(0, 0, 0);
		table = GetEntryTableBase();
		found = func_02028a54(table, run->mapId);
		entry = _Z19FindInlineEntryByIdP14Entry_02028bd0i(table, run->mapId);
		*(struct InlineEntry**)(ov + 0x36f8) = entry;
		grotto = _ZN9GameState15GetGrottoStructEv(battle);
		if (IsInRange0201b588(run->mapId) != 0) {
			entry->fd = grotto[8];
		}
		kind = 0;
		value = 0;
		runSub2 = &((char*)run)[0x23ec];
		if (found != 0 && entry != 0) {
			void* tail = *(void**)(ov + 0x3704);
			struct GrottoSlot* slot = (struct GrottoSlot*)(ov + 0x3a9c) + entry->slotIndex;
			if (slot->used == 0) {
				_Z19InitStruct_021b46d8P14Struct021b46d8(slot);
				slot->entry = entry;
				if (_Z22IsValueInRange0201b5d8i(run->mapId) != 0 && ((unsigned char*)grotto)[3] == 0) {
					unsigned char* detail = _ZN17ActiveGrottoClass15GetDetailedDataEv(runSub2);
					if (detail != 0) {
						unsigned char tag = detail[1];
						if (tag == 1) {
							slot->kind = tag;
							slot->value = *(unsigned short*)(detail + 0x54);
						} else if (tag == 2) {
							slot->kind = tag;
							slot->value = _ZNK23DetailedTreasureMapData17LegacyBossMapData26MaybeGetCurrentAlternateIDEv(detail + 0x4c);
						}
						kind = slot->kind;
						value = slot->value;
					}
				}
				_Z16AppendNodeToTailP16TailList020469b4P16TailNode020469b4(tail, slot);
			}
			self->state = 4;
		} else {
			if (_Z22IsValueInRange0201b5d8i(run->mapId) != 0 && ((unsigned char*)grotto)[3] == 0) {
				unsigned char* detail = _ZN17ActiveGrottoClass15GetDetailedDataEv(runSub2);
				if (detail != 0) {
					unsigned char tag = detail[1];
					if (tag == 1) {
						kind = tag;
						value = *(unsigned short*)(detail + 0x54);
					} else if (tag == 2) {
						kind = tag;
						value = _ZNK23DetailedTreasureMapData17LegacyBossMapData26MaybeGetCurrentAlternateIDEv(detail + 0x4c);
					}
				}
			}
			self->state = 5;
		}
		hasFlag = 0;
		if (func_0202c508(search) != 0 && *(unsigned char*)((char*)run + 0x281f) != 0) {
			hasFlag = 1;
		}
		*(unsigned char*)((char*)run + 0x281f) = 0;
		if (fld->f20 > 0 || fld->f24 > 0) {
			_Z27EnqueueEventTag150_021d035chhh(1, actor->s4, 1);
		}
		func_ov017_021c4854(run, kind, value, actor->vec44, (short)actor->f54, hasFlag);
		{
			void* target = *(void**)(ov + 0x3734);
			if (CheckField0NonZero((int*)search) != 0) {
				int i;
				for (i = 0; i < 4; i++) {
					void* c = _ZN9GameState21GetPartyMemberByIndexEi(battle, i);
					if (c != 0 && _Z23GetByteFieldMasked0x1cePvi(c, 0x20) != 0) {
						func_ov017_021bd4d0(target, i, 0, 1);
					}
				}
			}
		}
		if (func_0202c508(search) != 0 && ids->flagF7d != 0) {
			func_ov017_021ce400(ids->flagF7d, run->mapId);
		}
		return;
	}
	if (self->state == 4) {
		if (IsField0Null(*(void***)(ov + 0x3704)) != 0) {
			self->state = 5;
		}
		return;
	}
	if (self->state == 5) {
		func_0208e520(func_0208e0a8(), run->header + 5);
		_ZN13SafeAllocator5ResetEv(ov + 0x18c);
		_ZN24LootableContainerManager5ResetEv(region);
		*(&data_ov017_021d75b8 + 1) = _ZN16BackgroundLoader13QueueLoadFileEPKcP13SafeAllocator(loader, data_ov017_021d75fc, 0);
		self->state = 6;
		return;
	}
	if (self->state == 6) {
		if (_ZN16BackgroundLoader13GetTaskStatusEi(loader, *(&data_ov017_021d75b8 + 1)) == 0) {
			return;
		}
		if (_ZN16BackgroundLoader21GetDetailedTaskStatusEi(loader, *(&data_ov017_021d75b8 + 1)) == 2) {
			_ZN16BackgroundLoader17GetLoadedFileByIDEiPPvPj(loader, *(&data_ov017_021d75b8 + 1), &workLo.buf, &workLo.fileSize);
			if (workLo.buf != 0) {
				unsigned char* header = run->header;
				if (*(unsigned char*)((char*)run + 0x23b8) != 0) {
					func_0201afd0(run, workLo.buf, workLo.fileSize, 0);
				} else {
					unsigned short id = *(unsigned short*)header;
					if (id >= 0xc546 && id <= 0xc546 + 0x15) {
						sprintf(workHi.name, data_ov017_021d7619, (char*)header + 5, id + (0x61 - 0xc546));
					} else {
						strcpy(workHi.name, (char*)header + 5);
					}
					_ZN24LootableContainerManager18LoadZoneContainersEPKvjPKcP13SafeAllocator(region, workLo.buf, workLo.fileSize, workHi.name, 0);
					func_0201647c(run);
				}
			}
		}
		_ZN16BackgroundLoader10RemoveTaskEi(loader, *(&data_ov017_021d75b8 + 1));
		{
			unsigned short id = run->mapId;
			*(&data_ov017_021d75b8 + 1) = -1;
			if (_Z19IsIdInRange020981e4ii((char*)run + 0x840, id) != 0 &&
			    _Z17IsIdInSet02098210ii((char*)run + 0x840, id) == 0 &&
			    func_0202c540(search) != 0) {
				unsigned char* sub = (unsigned char*)((int)run + 0x1840);
				if (sub[0xb62] == 0) {
					_Z33DispatchWithGlobalContext020daf9ciiii(1, 0, 1, 0);
					_Z13SetBrightnessP13GameResourcesii(ov, -16, 0);
					self->state = 7;
					return;
				}
			}
		}
		self->state = 8;
		return;
	}
	if (self->state == 7) {
		unsigned char* sub = (unsigned char*)((int)run + 0x1840);
		if (sub[0xb62] != 0) {
			self->state = 8;
		}
		return;
	}
	if (self->state == 8) {
		void* tailList;
		void* obj09020;
		int mapId;
		int subId;
		unsigned char* header;
		char* runVars;
		struct InlineEntry* entry;
		signed char* flag4;
		int moving;
		struct ListNode021bba08* node;

		if (_ZN16BackgroundLoader17GetNumQueuedTasksEv(loader) > 0) {
			return;
		}
		if (func_0202c540(search) != 0 && fld->f67 != 0 && self->v20 == 0 && fld->f24 <= 0) {
			if (self->warp != 0xffff) {
				if (self->warp != 0) {
					fld->f24 = self->warp;
					fld->flagD = 1;
				} else if (*(unsigned short*)(ov + 0x44ac) != 0) {
					fld->f24 = *(unsigned short*)(ov + 0x44ac);
					*(unsigned short*)(ov + 0x44ac) = 0;
				} else {
					self->timer = self->timer + _ZNK9GameState21GetEffectiveDeltaTimeEv(battle);
					if (self->timer < 1000) {
						return;
					}
				}
			}
		}
		tailList = *(void**)(ov + 0x36fc);
		obj09020 = GetData02109020();
		mapId = run->mapId;
		subId = run->subId;
		header = run->header;
		runVars = (char*)run + 0xc;
		entry = _Z19FindInlineEntryByIdP14Entry_02028bd0i(entryTable, mapId);
		flag4 = _Z15GetFieldIfFlag4Pc(battle);
		workHi.baseVec = *(struct Vec3*)actor->vec44;
		moving = (fld->f20 > 0) ? 1 : 0;
		if (moving != 0) {
			self->active = 0;
		}
		node = *(struct ListNode021bba08**)(ov + 0x3734);
		if (node->flag2 != 0 && node->f9b != 0) {
			self->active = 0;
			node->f9b = 0;
		} else if (*(volatile int*)&fld->f24 >= 0) {
			int hasBit = ((*actor->p130 & 1) != 0);
			if (hasBit != 0) {
				if (fld->f24 < 21000 || fld->f24 >= 40000) {
					self->active = 1;
				}
			}
		}
		if (self->active != 0) {
			_Z27SetStateAndDispatch0209c3b4P13Actor0209c3b4i(&data_02109bf4, func_0209cd50(mapId));
			if (CheckField0NonZero((int*)search) != 0 && _Z26AreListedCombatantsBit0SetP11Obj02086b98(ids) != 0) {
				_Z30DispatchContextByState0209c678P13Actor0209c678i(&data_02109bf4, 0);
			}
		}
		_Z27SetFlag0x2AndNotify0209caa0P11Obj0209caa0i(&data_02109bf4, ((unsigned int)header[0xd] << 24) >> 31);
		_Z36ClassifyPriceCategoryAndNotifyBattlePhi(obj5ec34, *(unsigned short*)header);
		_Z18ClearFlags020466f4P16FlagWord020466f4j(flags14e04, 0x200000);
		if (fld->active == 0) {
			_Z23ClearFieldAt0xc0208f3a8P11Obj0208f3a8(obj09020);
			func_0206461c(obj5ec34, run->header);
			func_02095b30(GetGlobal02109418(), 0);
		}
		if (func_0202c508(search) != 0) {
			struct InlineEntry* e2 = _Z19FindInlineEntryByIdP14Entry_02028bd0i(entryTable, run->mapId);
			if (e2 != 0) {
				_Z31SetElementFlagsFromMask02013cb0P9S02013cb0j(run, e2->group);
				_Z33UpdateFlaggedType2Entries02018c74Phj(run, e2->f4);
			}
		} else if (moving == 0 && *(unsigned char*)((char*)battle + 0x5cac) == 0) {
			_Z31SetElementFlagsFromMask02013cb0P9S02013cb0j(run, self->flagsE);
			_Z33UpdateFlaggedType2Entries02018c74Phj(run, self->flags10);
		}
		func_02097ec8((char*)run + 0x840);
		if ((fld->flags6e & 2) == 0) {
			func_ov017_021a2fa0(ov);
		}
		func_ov017_0219e180(ov, mapId);
		if (fld->active == 0) {
			struct SafeAllocatorObj* alloc = (struct SafeAllocatorObj*)(ov + 0x18c) + 1;
			_ZN13SafeAllocator5ResetEv(alloc);
			func_0208f168(obj09020, alloc);
		}
		_ZN8Object3D10SetField06Et(c3ac, mapId);
		func_0208fc30(runSub);
		if (entry != 0) {
			_Z21ClearField0x8020733c8P11Obj020733c8(entry);
			if (entry->f34 != 0) {
				actor->b8 = func_02028460(entry->vec18, actor->vec44);
				actor->c2 |= 0x40;
			}
		}
		_Z17ClearBitsInField4Pjj(ov, 1);
		_Z15ClearBitsInWordPjj(ov, 0x10);
		_Z34SetupCombatantAllocations_021901acv(ov);
		func_020a6aac(ov, mapId, subId);
		func_020acecc(ov, mapId, subId);
		if ((fld->flags6e & 2) == 0 && func_0202c540(search) != 0 && actor != 0) {
			if (_Z30FindAndApplyEntryVec3_021a01f4PviiS_Pi(ov, mapId, _ZNK9GameState12GetTimeOfDayEv(battle), actor, (int*)&workHi.entryVec) != 0 && actor != 0) {
				_ZN8Vector3iaSERKS_(actor->vec44, (int*)&workHi.entryVec);
			}
		}
		if (moving != 0) {
			func_ov017_0219fc88(self, fld->f20);
		} else if (fld->f28 >= 0 && fld->f2c == run->mapId && fld->s1e != fld->f28) {
			if (_Z22IsValueInRange0201b5d8i(run->mapId) == 0) {
			void* n = *(void**)(ov + 0x3718);
			func_ov017_021b6f18(n);
			_Z19InitStruct_02196c08Ph(&workHi.msg);
			workHi.msg.kind = 1;
			workHi.msg.actor = _Z18GetField0x3acValueP9GameState(battle);
			workHi.msg.value = fld->f28 + 5 + 0x8000;
			workHi.msg.id = 0xffff;
			workHi.msg.f8 = fld->f28;
			workHi.msg.f12 = data7930[0x26];
			func_ov017_021b6e70(&workHi.msg, workHi.msg.id);
			func_ov017_021b7104(n, &workHi.msg);
			_Z16AppendNodeToTailP16TailList020469b4P16TailNode020469b4(tailList, n);
			fld->f28 = -1;
			fld->flags6e |= 1;
			_Z15ClearBitsInWordPjj(ov, 0x800);
			}
		} else if (fld->f24 >= 0) {
			func_ov017_0219fc88(self, fld->f24);
		} else {
			void* obj2 = func_0205ec34();
			int listed;
			workHi.node.mapId = mapId;
			if (fld->active == 0 && _Z16Dispatch020649f4PvS_(obj2, &workHi.node) != 0) {
				func_0206f81c(&workHi.node);
			}
			listed = _Z14ListContainsIdP16ListHead02046b60i(tailList, 4);
			if (CheckField0NonZero((int*)search) == 0 || func_0202c508(search) != 0) {
				_ZN16BackgroundLoader13AddLockGlobalEv();
				if (_Z18LoadFileIntoMemoryPKcPvPj(data_ov017_021d761e, data_0211e33c, &workLo.size) != 0 &&
				    _Z24InitStreamAndRun0208dc00P12StreamHeaderii(data_0211e33c, workLo.size, (int)&workHi.slot) != 0 &&
				    _Z18TestBitInByteArrayiPhi((int)obj2, (unsigned char*)obj2 + 0x8c, workHi.slot + 0xea + 0xb00) == 0) {
					if (listed == 0) {
						struct ListNode021bba08* n2 = *(struct ListNode021bba08**)(ov + 0x3734);
						func_ov017_021bba08(n2, run->header);
						n2->f8 = fld->f20;
						_Z16AppendNodeToTailP16TailList020469b4P16TailNode020469b4(tailList, n2);
						fld->f20 = -1;
					} else {
						_Z20SetOrClearBitInArrayPvPhii(obj2, (unsigned char*)obj2 + 0x8c, workHi.slot + 0xea + 0xb00, 1);
					}
				}
				_ZN16BackgroundLoader16RemoveLockGlobalEv();
			}
		}
		fld->s1e = -1;
		node = *(struct ListNode021bba08**)(ov + 0x3734);
		if (node->flag2 == 0 || node->f9c != 0 || node->f8 == 0) {
			_Z27EnqueueEventTag158_021d23bct(0xffff);
		}
		if (fld->active == 0) {
			if (runVars != 0) {
				_Z13SetField0x246Pvs(flag4, *(short*)(runVars + 0x34));
			}
			switch (((unsigned int)header[0xc] << 25) >> 30) {
			case 0:
				_Z33SetVec3ComponentsAndApply020a27ccP14Struct020a27cc(flag4);
				break;
			case 1:
				_Z29ConfigureVec3AndApply020a2800P14Struct020A2800(flag4);
				break;
			case 2:
				_Z25InitVec3ParamsVariant8b33Pv(flag4);
				break;
			case 3:
				_Z33SetVec3ComponentsAndApply020a2868P14Struct020a2868(flag4);
				break;
			default:
				_Z29ConfigureVec3AndApply020a2800P14Struct020A2800(flag4);
				break;
			}
			if (_Z13GetField0x218P9S020a3570(flag4) != 0) {
				workHi.actorVec = *(struct Vec3*)actor->vec44;
				workLo.fieldVec = *(struct Vec3*)(flag4 + 0x10);
				Vector3fix_Subtract(&workHi.actorVec, &workHi.baseVec, &workLo.deltaVec);
				Vector3fix_Add(&workLo.fieldVec, &workLo.deltaVec, &workLo.fieldVec);
				_Z34CopyVec3ToField0x10AndCall0202e5d8PcPi(flag4, (int*)&workLo.fieldVec);
			}
			func_ov017_021982ac(ov);
			_Z22ApplyVecFromField0x246Pc(flag4);
		}
		if ((fld->flags6e & 0x40) != 0) {
			_Z22ApplyVecFromField0x246Pc(flag4);
		}
		if ((fld->flags6e & 1) == 0) {
			_Z18ClearFlags020466f4P16FlagWord020466f4j(flags14e04, 1);
			_Z29SetCombatModeFromCase020dc2d0i(0);
		}
		_Z14SetByteAt0x2c5P23ByteField0x2c5_020a2cbch(flag4, ((struct HdrFlags4*)(header + 4))->noSkip == 0 ? 1 : 0);
		func_020ae53c(1);
		_Z23NotifyOv017OnFlag0x1130v();
		_Z32MarkActiveCombatantSlots02026b7cPh(*(void**)(ov + 0x36d0));
		_Z37SyncVec3ForFlaggedCombatants_021a3338v(ov);
		if ((fld->flags6e & 8) != 0) {
			self->state = 9;
			return;
		}
		{
			void* g400 = GetGlobal02109400();
			func_02094ab0(g400);
			_Z21BlankFunction02094b34v(g400, 0x6f, 5000, 1, 1);
		}
		self->state = 10;
		return;
	}
	if (self->state == 10) {
		if (_Z18AlwaysTrue02094b4cv(GetGlobal02109400()) != 0) {
			self->state = 9;
		}
		return;
	}
	if (self->state == 9) {
		int wantCounter;
		int wantByte;
		int i;

		if (fld->f69 != 0 && _Z14ListContainsIdP16ListHead02046b60i(list, 0x3d) == 0) {
			_Z29InitAndAppendState61_021a65c4Pvhi(ov, 1, 0);
		}
		wantCounter = 1;
		wantByte = 1;
		if (_Z14ListContainsIdP16ListHead02046b60i(list, 4) != 0 ||
		    _Z14ListContainsIdP16ListHead02046b60i(list, 0xa) != 0 ||
		    _Z14ListContainsIdP16ListHead02046b60i(list, 0x3d) != 0) {
			wantCounter = 0;
			wantByte = 0;
		}
		if (_Z14ListContainsIdP16ListHead02046b60i(list, 0x2a) != 0 ||
		    _Z14ListContainsIdP16ListHead02046b60i(list, 0xf) != 0) {
			wantByte = 0;
		}
		if (_Z14ListContainsIdP16ListHead02046b60i(list, 0x2d) != 0) {
			wantCounter = 0;
		}
		if (func_0202c508(search) != 0) {
			_Z27EnqueueEventTag147_021cdaa0v();
		}
		if (wantCounter != 0) {
			_Z17SetMainBrightnessP13GameResourcesii(ov, 0, 15);
		}
		if (wantByte != 0) {
			_Z17SetByteField0x253Pv(actor);
		}
		for (i = 0; i < ids->count; i++) {
			struct Combatant0219e384* c = (struct Combatant0219e384*)
				_ZN9GameState21GetPartyMemberByIndexEi(battle, ids->ids[i]);
			if (c != 0 && GetByte0x26c((char*)c) != 0) {
				continue;
			}
			if (c != 0) {
				_ZN8Object3D10EnableFlagEi(c, 0x4000000);
			}
			if (i > 0 && fld != 0 && fld->active == 0) {
				workLo.memberVec = *(struct Vec3*)actor->vec44;
				_ZN8Vector3iaSERKS_(c->vec44, (int*)&workLo.memberVec);
				if (fld->mapId == 10000) {
					workLo.memberVec.v[0] = 0xff000;
					workLo.memberVec.v[1] = 0;
					workLo.memberVec.v[2] = 0xff000;
				}
				_Z26EnqueueEventTag35_021d2ad0hiiih(c->GetId(), workLo.memberVec.v[0], workLo.memberVec.v[1], workLo.memberVec.v[2], 0);
			}
		}
		if (func_0202c508(search) != 0 && fld != 0 && fld->active == 0) {
			void* c;
			_ZN8Vector3iaSERKS_((int*)((char*)battle + 0x7f60), actor->vec44);
			c = _ZN9GameState21GetPartyMemberByIndexEi(battle, 0xce);
			if (c != 0) {
				int* src = (int*)((char*)battle + 0x7f60);
				int z = src[2];
				int y = src[1];
				int x = *(int*)((char*)battle + 0x7f60);
				((struct Combatant0219e384*)c)->vec44[0] = x;
				((struct Combatant0219e384*)c)->vec44[1] = y;
				((struct Combatant0219e384*)c)->vec44[2] = z;
			}
		}
		if (func_0202c508(search) != 0) {
			int* src = (int*)((char*)battle + 0x7f60);
			_Z26EnqueueEventTag35_021d2ad0hiiih(0xce, *(int*)((char*)battle + 0x7f60), src[1], src[2], 0);
		}
		if (fld != 0 && (fld->flags6e & 0x20) != 0) {
			func_ov017_0219c598(0, 0, 1);
		}
		*(int*)((char*)GetGlobalField0x1c020421a0() + 0x2d8) = 0;
		if (fld->f64 != 0) {
			struct BattleStruct* battle2 = _ZN9GameState11GetInstanceEv();
			int* data2 = _ZN15LightingManager11GetInstanceEv();
			char* ov2 = (char*)func_ov017_0218b5b0();
			struct Combatant0219e384* c;
			func_ov017_021b6f18(*(void**)(ov2 + 0x3718));
			c = (struct Combatant0219e384*)_ZN9GameState21GetPartyMemberByIndexEi(battle2, 0);
			_Z19InitStruct_02196c08Ph(&workLo.msgF64);
			workLo.msgF64.kind = 1;
			workLo.msgF64.actor = 0;
			workLo.msgF64.value = c->f1b2;
			workLo.msgF64.id = -1;
			workLo.msgF64.fe = 1;
			workLo.msgF64.f14 = 1;
			workLo.msgF64.f12 = data2[0x26];
			func_ov017_021b6e70(&workLo.msgF64, -1);
			func_ov017_021a7a74(ov2, &workLo.msgF64);
		} else if (fld->f63 != 0) {
			int j;
			int found2;
			unsigned short want;
			struct InlineEntry* slot;
			unsigned short id;
			int mapId2;
			int* data2;
			struct BattleStruct* battle2;
			char* ov2;
			struct Combatant0219e384* lead;
			battle2 = _ZN9GameState11GetInstanceEv();
			data2 = _ZN15LightingManager11GetInstanceEv();
			ov2 = (char*)func_ov017_0218b5b0();
			lead = _Z25GetCombatantWithFlag0x100P9GameStatei(battle2, 0);
			if (lead != 0 && lead->be == 2) {
				want = lead->f1b2;
				if (want != 0 && want >= 1 && want <= 0x7fff) {
					slot = *(struct InlineEntry**)(ov2 + 0x36f8);
					if (slot != 0) {
						mapId2 = *(unsigned short*)slot;
						found2 = 0x7fff - 0x8000;
						for (j = 0; j < 12; j++) {
							struct Npc0219e384* c;
							found2 = j + (slot->slotIndex * 12 + 0x70);
							c = _ZN9GameState27GetMaybeFieldMonsterByIndexEi(battle2, found2);
							if (c == 0) {
								continue;
							}
							id = c->f16a;
							if (id != want || c->be != 2) {
								continue;
							}
							if (c->be == 6) {
								continue;
							}
							if (_ZNK8Object3D9IsVisibleEv(c) == 0) {
								continue;
							}
							if (mapId2 != _ZNK8Object3D10GetField06Ev(c)) {
								continue;
							}
							if (GetNpcMode(c) != 8 && GetNpcMode(c) != 9 && GetNpcMode(c) != 0) {
								c->f166 = _Z18GetField0x3acValueP9GameState(battle2);
								TrySetMode02076ccc(c, 8);
							}
							want = id;
							break;
						}
						if (found2 >= 0) {
							func_ov017_021b6f18(*(void**)(ov2 + 0x3718));
							_Z19InitStruct_02196c08Ph(&workLo.msgF63);
							workLo.msgF63.kind = 1;
							workLo.msgF63.actor = _Z18GetField0x3acValueP9GameState(battle2);
							workLo.msgF63.value = want;
							workLo.msgF63.id = found2;
							func_ov017_021b6e70(&workLo.msgF63, found2);
							workLo.msgF63.f12 = data2[0x26];
							func_ov017_021a7a74(ov2, &workLo.msgF63);
						}
					}
				}
			}
		}
		if (self->v12 != 0) {
			struct GrottoRun* run2 = func_02012fe4();
			*(unsigned short*)((char*)run2 + 0x2786) = self->v12;
			if (((self->v14 << 18) >> 18) != -1) {
				*(unsigned short*)((char*)run2 + 0x2784) = (self->v14 << 18) >> 18;
			}
			*(unsigned char*)((char*)run2 + 0x2788) = (self->v14 << 16) >> 30;
			workLo.warpVec.v[0] = self->v16 << 7;
			workLo.warpVec.v[1] = self->v18 << 7;
			workLo.warpVec.v[2] = self->v1a << 7;
			_ZN8Vector3iaSERKS_((int*)((char*)run2 + 0x2774), (int*)&workLo.warpVec);
			*(int*)((char*)run2 + 0x2780) = self->v1c;
			func_020a72ac(run2, self->v1e);
			VectorizedMemset(&self->v12, 0, 0xe);
		}
		if (*(short*)(ov + 0x44b2) >= 0) {
			void* g030 = GetGlobal02109030();
			func_02094030(g030, *(short*)(ov + 0x44b2), -1, 0);
			*(short*)(ov + 0x44b2) = -1;
		}
		self->done = 1;
	}
}
