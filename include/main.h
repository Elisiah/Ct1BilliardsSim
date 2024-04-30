#ifndef _MAIN_H_
#define _MAIN_H_

#define PI 3.141592653589793
#define MAX_DEGREES 360.0

typedef unsigned char			u8;	/* unsigned  8-bit */
typedef unsigned short			u16;	/* unsigned 16-bit */
typedef unsigned long			u32;	/* unsigned 32-bit */
typedef unsigned long long		u64;	/* unsigned 64-bit */

typedef signed char			s8;	/* signed  8-bit */
typedef short				s16;	/* signed 16-bit */
typedef long				s32;	/* signed 32-bit */
typedef long long			s64;	/* signed 64-bit */

typedef volatile unsigned char		vu8;	/* unsigned  8-bit */
typedef volatile unsigned short		vu16;	/* unsigned 16-bit */
typedef volatile unsigned long		vu32;	/* unsigned 32-bit */
typedef volatile unsigned long long	vu64;	/* unsigned 64-bit */

typedef volatile signed char		vs8;	/* signed  8-bit */
typedef volatile short			vs16;	/* signed 16-bit */
typedef volatile long			vs32;	/* signed 32-bit */
typedef volatile long long		vs64;	/* signed 64-bit */

typedef float				f32;	/* single prec floating point */
typedef double				f64;	/* double prec floating point */

typedef struct Vec2f {
    f32 x;
    f32 y;
} Vec2f;

typedef struct Vec2w {
    s32 x;
    s32 y;
} Vec2w;

typedef struct Vec3f {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

typedef struct actorSubArray { //starts at 0x40
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
} actorSubArray; //sizeof 0x14

typedef struct unk_8010AA28 {
    s32 unk_00;
    s32 unk_04;
    s32 unk_08;
    s32 unk_0C;
} unk_8010AA28; //sizeof 0x10

typedef struct Actor {
    /* 0x000 */ s32 actorID;
    /* 0x004 */ s32 actorIndex;
    /* 0x008 */ s32 actorState;//enum, 0 == idle, 1 == on tongue, 2 == in mouth, 3 == spit
    /* 0x00C */ s32 posOnTongue;//the segment iirc
    /* 0x010 */ u32 globalTimer;
    /* 0x014 */ s32 touched; //0 == false. 1 == true.
    /* 0x018 */ s32 hit; //triggered when hit
    /* 0x01C */ s32 tongueBumpSeg; //the segment at which the tongue was bumped back
    /* 0x020 */ s32 eaten;//0 == false. 1 == true. does not reset.
    /* 0x024 */ Vec3f pos;
    /* 0x030 */ Vec3f vel;
    /* 0x03C */ f32 tScale;
    /* 0x040 */ f32 tYPos;
    /* 0x044 */ Vec3f tOffset;
    /* 0x050 */ s32 tongueCollision; //tongue collision enum. 0 == none. 1 == toungable.

    /* 0x054 */ actorSubArray unknownPositionThings[3]; //usually 2?
    //actorSubArray is these offsets in the struct
        // /* 0x054 */ f32 unk_54;//cos x used by FISH
        // /* 0x058 */ f32 unk_58;//presumably y
        // /* 0x05C */ f32 unk_5C;//sin z used by FISH
        // /* 0x060 */ f32 unk_60;//hitboxScale
        // /* 0x064 */ f32 unk_64;//hitboxYstretch
        
        // /* 0x068 */ f32 unk_68;
        // /* 0x06C */ f32 unk_6C;
        // /* 0x070 */ f32 unk_70;
        // /* 0x074 */ f32 unk_74;
        // /* 0x078 */ f32 unk_78;

        // /* 0x07C */ f32 unk_7C;
        // /* 0x080 */ f32 unk_80;
        // /* 0x084 */ f32 unk_84;
        // /* 0x088 */ f32 unk_88;
        // /* 0x08C */ f32 unk_8C;

    /* 0x090 */ f32 unk_90;
    /* 0x094 */ f32 unk_94;
    /* 0x098 */ s32 unk_98;
    /* 0x09C */ s32 unk_9C;
    /* 0x0A0 */ unk_8010AA28 unk_A0;
    /* 0x0B0 */ f32 unk_B0;
    /* 0x0B4 */ f32 unk_B4;
    /* 0x0B8 */ f32 unk_B8;
    /* 0x0BC */ f32 unk_BC;
    /* 0x0C0 */ f32 unk_C0;
    /* 0x0C4 */ s32 unk_C4;
    /* 0x0C8 */ s32 unk_C8;
    /* 0x0CC */ f32 sizeScalar;
    /* 0x0D0 */ f32 unk_D0;
    /* 0x0D4 */ f32 unk_D4;
    /* 0x0D8 */ f32 unk_D8;
    /* 0x0DC */ f32 unk_DC;
    /* 0x0E0 */ s32 unk_E0;
    /* 0x0E4 */ s32 unk_E4;
    /* 0x0E8 */ f32 unk_E8;
    /* 0x0EC */ s32 unk_EC;
    /* 0x0F0 */ u32 unk_F0;
    /* 0x0F4 */ f32 unk_F4;
    /* 0x0F8 */ f32 unk_F8;
    /* 0x0FC */ f32 unk_FC;
    /* 0x100 */ f32 unk_100;
    /* 0x104 */ f32 unk_104;
    /* 0x108 */ f32 unk_108;
    /* 0x10C */ s32 userVariables[5];
    /* 0x120 */ s32 unk_120;
    /* 0x124 */ s32 unk_124;
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ s32 unk_12C;
    /* 0x130 */ s32 unk_130;
    /* 0x134 */ f32 unk_134[8];
        /* 0x154 */ union {
        Vec2f _f32;
        Vec2w _s32;
    } position;
    /* 0x15C */ f32 unk_15C;
    /* 0x160 */ f32 unk_160;
    /* 0x164 */ f32 unk_164;
    /* 0x168 */ f32 unk_168;
    /* 0x16C */ f32 unk_16C;
    /* 0x170 */ f32 unk_170;
} Actor; //sizeof 0x174

#define ACTORS_MAX 64
#define PLAYERS_MAX 4

enum actorIDs {
	ACTOR_NULL = 0,
	RED_ANT = 1,
	GREEN_ANT = 2,
	GREY_ANT = 3,
	BULLET_HELL_ANT = 4,
	ANT_TRIO = 5,
	YELLOW_ANT = 6,
	ANT_QUEEN = 7,
	ANT_QUEEN_ANT = 8,
	GREY_ANT_SPAWNER = 9,
	ANT_TRIO_SPAWNER = 10,
	BULLET_HELL_ANT_SPAWNER = 11,
	RED_ANT_SPAWNER = 12,
	WHITE_BOMB_SNAKE = 13,
	GRENADE = 14,
	MISSILE_SPAWNER = 15,
	MISSILE = 16,
	CANNON = 17,
	CANNONBALL = 18,
	BL_BOSS_SEGMENT = 19,
	EXPLOSION = 20, //immune to stopwtch powerup
	BL_BOSS_BOMBS = 21,
	BLACK_CHAMELEON_PROJECTILE_SPAWNER = 22,
	BLACK_CHAMELEON_PROJECTILE = 23,
	CHOMPER = 24,
	SAND_CRAB = 25,
	VULTURE = 26,
	ARROW_SPAWNER = 27,
	ARROWS = 28,
	BOULDER = 29,
	ARMADILLO = 30,
	UNK_1F = 31,
	POPCORN = 32,
	POGO = 33,
	UNK_22 = 34,
	UNK_23 = 35,
	ICE_CREAM_SANDWICH = 36,
	CHOCO_KID = 37,
	CAKE_BOSS = 38,
	CAKE_BOSS_STRAWBERRY = 39,
	UNK_28 = 40,
	CAKE_BOSS_CHOCO_KID = 41,
	BOWLING_BALL = 42,
	BOWLING_PINS = 43,
	CUE_BALL = 44,
	BILLIARDS_BALL = 45,
	UNK_2E = 46,
	UNK_2F = 47,
	CUP = 48,
	SAUCER = 49,
	METAL_SHEET = 50,
	SCROLL = 51,
	RNG_ROOM_SPAWNER = 52,
	MIRROR = 53,
	BARREL_JUMP_FIRE_SPAWNER = 54,
	BARREL_JUMP_FIRE = 55,
	FIRE_SPITTER = 56,
	CANDLES = 57,
	FIRE_SPAWNER = 58,
	FIRE = 59,
	SANDAL = 60,
	PILE_OF_BOOKS = 61,
	PILE_OF_BOOKS_ARM_SEGMENTS = 62,
	PILE_OF_BOOKS_ARM_SPITTER = 63,
	PILE_OF_BOOKS_PROJECTILE = 64,
	SPIDER_SPAWNER = 65,
	SPIDER = 66,
	SPIDER_TRIO = 67,
	GOLEM = 68,
	HEDGEHOG = 69,
	FISH = 70,
	LIZARD_KONG_BUTTERFLY = 71,
	GOLEM_ROOM_SPIDER_SPAWNER = 72,
	LIZARD_KONG_BUTTERFLY_SPAWNER = 73,
	LIZARD_KONG_BOULDER = 74,
	LIZARD_KONG = 75,
	POPCORN_BUCKET_SPAWNER = 76,
	POPCORN_BUCKET = 77,
	UNK_4E = 78,
	CHOCO_KID_SPAWNER = 79,
	SPAWNED_CHOCO_KID = 80,									// KL specific(?)
	GREY_ANT_SPAWNER_WRAPPER = 81,
	UNK_52 = 82,
	BATTLE_MODE_SAND_CRAB_SPAWNER = 83,
	BATTLE_MODE_SAND_CRAB = 84,
	BATTLE_MODE_FIRE_SPAWNER = 85,
	BATTLE_MODE_FIRE = 86,
	BATTLE_MODE_SAUCER_SPAWNER = 87,
	BATTLE_MODE_SAUCER = 88,
	UNK_59 = 89,
	UNK_5A = 90,
	POWER_UP_SPAWNER = 91,
	FALLING_GREY_ANT_SPAWNER = 92,
	FALLING_GREY_ANT = 93,
	UNK_FIRE_SPAWNER = 94,
	R_HEART = 95,
	FALLING_R_HEART = 96,
	O_HEART = 97,
	Y_HEART = 98,
	CROWN = 99,
	CARROT = 100,
	UNK_65 = 101,
	TIME_STOP_POWER_UP = 102,
	BIG_FEET_POWER_UP = 103,
	BIG_HEAD_POWER_UP = 104,
	SHRINK_POWER_UP = 105,
	SHRINK_ENEMY_POWER_UP = 106
};

#endif //_MAIN_H_