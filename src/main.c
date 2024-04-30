#include <stdio.h>
#include "main.h"

f32 CalculateAngleOfVector(f32 x, f32 y) {
    s32 pad[2];
    f32 angle;
    f32 frac;
    f32 lookupTwo;
    f32 lookupOne;
    f32 absY;
    f32 slope;
    f32 absX;
    f32 floorF;
    s32 next_index;
    s32 floor;
    u32 quadrant = 0;
    
    // No input vector
    if (x == 0.0 && y == 0.0) {
        return 0.0f;
    }
    
    /* quadrant is used to store the quadrant of the angle [0,3]
       and later stores the sgn */
    if (x < 0.0f) {
        quadrant = 1;
        absX = -x;
    } else {
        absX = x;
    }
    
    if (y < 0.0f) {
        quadrant += 2;
        absY = -y;
    } else {
        absY = y;
    }
    
    /* quadrant += 4 iff |x| < |y| */
    if (absX < absY) {
        quadrant += 4;
        slope = (absX / absY) * 64.0f;  // between 0 and 64 (inclusive)
        floor = (s32) slope;            // int of slope
        next_index = floor + 1;         // floor + 1
        floorF = (f32) floor;           // floor (as float)
        frac = slope - floorF;          // frac of slope
    } else {
        slope = (absY / absX) * 64.0f;
        floor = (s32) slope;       
        next_index = floor + 1;    
        floorF = (f32) floor;      
        frac = slope - floorF;     
    }
    
    /* clamp floor and next_index to [0,64] */
    if (next_index > 64) {
        next_index = 64;
    }
    
    /* angle lookup table is 65 entries long, but the last entry is the same as the first */
    lookupOne = D_80108E7C[floor];
    lookupTwo = D_80108E7C[next_index];
    
    /* Lerp between lookup angle results */
    switch (quadrant) {
    case 0:
        // Q1, |x| >= |y|
        angle = ((1.0f - frac) * lookupOne) + (frac * lookupTwo);
        break;
    case 1:
        // Q2, |x| >= |y|
        angle = 180.0 - (f64) (((1.0f - frac) * lookupOne) + (frac * lookupTwo));
        break;
    case 2:
        // Q3, |x| >= |y|
        angle = MAX_DEGREES - (f64) (((1.0f - frac) * lookupOne) + (frac * lookupTwo));
        break;
    case 3:
        // Q4, |x| >= |y|
        angle = ((1.0f - frac) * lookupOne) + (frac * lookupTwo) + 180.0;
        break;
    case 4:
        // Q1, |x| < |y|
        angle = 90.0 - (f64) (((1.0f - frac) * lookupOne) + (frac * lookupTwo));
        break;
    case 5:
        // Q2, |x| < |y|
        angle = ((1.0f - frac) * lookupOne) + (frac * lookupTwo) + 90.0;
        break;
    case 6:
        // Q3, |x| < |y|
        angle = ((1.0f - frac) * lookupOne) + (frac * lookupTwo) + 270.0;
        break;
    case 7:
        // Q4, |x| < |y|
        angle = 270.0 - (f64) (((1.0f - frac) * lookupOne) + (frac * lookupTwo));
        break;
    }
    return angle;
}

f32 CalcAngleBetween2DPoints(f32 x1, f32 y1, f32 x2, f32 y2) {
    return CalculateAngleOfVector(x2 - x1, -(y2 - y1));
}

void ActorTick_CueBall(Actor* arg0) {
    f32 temp_f0;
    Actor* actor = arg0;
    

    if (arg0->tongueBumpSeg != 0) {
        if ((gTongueOnePointer->segments == arg0->tongueBumpSeg) || gTongueOnePointer->segments == (arg0->tongueBumpSeg + 1)) {
            if (gTongueOnePointer->vaulting == 0) {
                temp_f0 = CalcAngleBetween2DPoints(gTongueOnePointer->tongueXs[gTongueOnePointer->segments - 1] + gCurrentActivePlayerPointer->pos.x, gTongueOnePointer->tongueZs[gTongueOnePointer->segments - 1] + gCurrentActivePlayerPointer->pos.z, arg0->pos.x, arg0->pos.z);
                arg0->vel.x = __cosf(temp_f0 * 2 * PI / MAX_DEGREES) * arg0->position._f32.x;
                arg0->vel.z = -__sinf(temp_f0 * 2 * PI / MAX_DEGREES) * arg0->position._f32.x;
                playSoundEffect(0xA7, &arg0->pos.x, &arg0->pos.y, &arg0->pos.z, 0, 0);
            }
        }
    }
    
    ActorTick_MinigameActor(arg0);
    
    if (arg0->pos.y < -arg0->unknownPositionThings[0].unk_10) {
        arg0->unk_98 = 1;
        arg0->pos.x = gCurrentActivePlayerPointer->pos.x;
        arg0->pos.y = arg0->unk_134[0] + 1000.0f;
        arg0->pos.z = gCurrentActivePlayerPointer->pos.z;
        arg0->unk_A0.unk_08 = 0;
        if (arg0->pos.x > 1800.0f) {
            arg0->pos.x = 1800.0f;
        }
        if (arg0->pos.x < -1800.0f) {
            arg0->pos.x = -1800.0f;
        }
        if (arg0->pos.z > 900.0f) {
            arg0->pos.z = 900.0f;
        }
        if (arg0->pos.z < -900.0f) {
            arg0->pos.z = -900.0f;
        }
        arg0->vel.z = 0.0f;
        arg0->vel.y = 0.0f;
        arg0->vel.x = 0.0f;
    }
}

void ActorTick_MinigameActor(Actor* arg0) {
    f32 temp_f16;
    f32 temp_f6;

    arg0->vel.x -= (arg0->vel.x * arg0->position._f32.y);
    arg0->vel.z -= (arg0->vel.z * arg0->position._f32.y);
    if (((arg0->vel.x * arg0->vel.x) + (arg0->vel.z * arg0->vel.z)) < 1.0f) {
        arg0->vel.x = arg0->vel.z = 0.0f;
    }
    if (arg0->unk_98 != 0) {
        // if (arg0->vel.y == 0) {
        //     PlaySoundEffect(0xA9, &arg0->pos.x, &arg0->pos.y, &arg0->pos.z, 0, 0);
        // }
        if (arg0->pos.y > 0.0f) {
            arg0->vel.y -= 3.2f + (arg0->vel.y * 0.05f);
        } else {
            arg0->vel.y -= 6.4f;
        }
    } else if (arg0->vel.y != 0) {
        arg0->vel.y = 0;
        arg0->unk_A0.unk_08 = 1;
    }
    if (arg0->unk_9C != 0) {
        temp_f16 = (arg0->unk_B4 * arg0->unk_B4) + (arg0->unk_BC * arg0->unk_BC);
        temp_f6 = (arg0->vel.x * arg0->unk_B4) + (arg0->vel.z * arg0->unk_BC);
        arg0->vel.x -= ((2 * temp_f6 * arg0->unk_B4) / temp_f16);
        arg0->vel.z -= ((2 * temp_f6 * arg0->unk_BC) / temp_f16);
    }
    // if (((arg0->vel.x * arg0->vel.x) + (arg0->vel.z * arg0->vel.z) > 8.0f) && (gTimer % 60 == 0)) {
    //     PlaySoundEffect(0xD9, &arg0->pos.x, &arg0->pos.y, &arg0->pos.z, 0, 0);
    // }
}

void MinigameActors_PhysicsTick(void) {
    Actor* actor1;
    f32 temp_f0_3;
    f32 temp_f0_6;
    f32 temp_f0_7;
    f32 temp_f12_2;
    f32 temp_f12_4;
    f32 temp_f16;
    f32 temp_f16_2;
    f32 spCC;
    f32 spC8;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f18_2;
    f32 temp_f20_2;
    f32 temp_f20_3;
    f32 temp_f2_3;
    f32 temp_f2_5;
    f32 var_f20;
    f32 var_f22;
    f32 var_f24;
    f32 var_f26;
    f32 var_f28;
    f32 var_f2;
    s32 i, j;

    
    for (actor1 = gActors, i = 0; i < ACTORS_MAX; i++, actor1++) {
        if (actor1->actorID != CUE_BALL && actor1->actorID != BILLIARDS_BALL && actor1->actorID != BOWLING_BALL && actor1->actorID != BOWLING_PINS) {
            continue;
        }
        
        for (j = i + 1; j < ACTORS_MAX; j++) {
            temp_f20 = actor1->unknownPositionThings[0].unk_0C + gActors[j].unknownPositionThings[0].unk_0C;
            temp_f16 = actor1->pos.x - gActors[j].pos.x;
            temp_f18 = actor1->pos.z - gActors[j].pos.z;
            spCC = gActors[j].pos.x - actor1->pos.x;
            spC8 = gActors[j].pos.z - actor1->pos.z;
            if ((gActors[j].actorID == CUE_BALL) || (gActors[j].actorID == BILLIARDS_BALL) || (gActors[j].actorID == BOWLING_PINS)) {
                temp_f12_2 = (temp_f16 * temp_f16) + (temp_f18 * temp_f18);
                if (!((temp_f20 * temp_f20) < temp_f12_2)) {
                    if (!((actor1->unknownPositionThings[0].unk_10 + actor1->pos.y) < gActors[j].pos.y)) {
                        if (!((gActors[j].unknownPositionThings[0].unk_10 + gActors[j].pos.y) < actor1->pos.y)) {
                            var_f28 = 0.0f;
                            var_f22 = 0.0f;
                            var_f24 = 0.0f;
                            var_f26 = 0.0f;
                            temp_f0_3 = (actor1->vel.x * spCC) + (spC8 * actor1->vel.z);
                            temp_f2_3 = (spC8 * gActors[j].vel.z) + (gActors[j].vel.x * spCC);
                            temp_f20_2 = (spCC * spCC) + (spC8 * spC8);
                            if (0.0f != temp_f20_2) {
                                var_f22 = (temp_f0_3 * spCC) / temp_f20_2;
                                var_f24 = (temp_f0_3 * spC8) / temp_f20_2;
                                var_f26 = (temp_f2_3 * spCC) / temp_f20_2;
                                var_f28 = (temp_f2_3 * spC8) / temp_f20_2;
                            }
                            if (actor1->actorID == CUE_BALL) {
                                var_f20 = 2.5f;
                            } else {
                                var_f20 = 1.0f;
                            }
                            if (gActors[j].actorID == BOWLING_BALL) {
                                var_f2 = 2.5f;
                            } else {
                                var_f2 = 1.0f;
                            }
                            actor1->vel.x = actor1->vel.x - var_f22;
                            actor1->vel.z = actor1->vel.z - var_f24;
                            gActors[j].vel.x -= var_f26;
                            gActors[j].vel.z -= var_f28;
                            actor1->vel.x += (((var_f20 - var_f2) / (var_f20 + var_f2)) * var_f22) + (((2 * var_f2) / (var_f20 + var_f2)) * var_f26);
                            temp_f16_2 = (var_f2 - var_f20) / (var_f20 + var_f2);
                            actor1->vel.z += (((var_f20 - var_f2) / (var_f20 + var_f2)) * var_f24) + (((2 * var_f2) / (var_f20 + var_f2)) * var_f28);
                            temp_f18_2 = (2 * var_f20) / (var_f20 + var_f2);
                            gActors[j].vel.x += (temp_f16_2 * var_f26) + (temp_f18_2 * var_f22);
                            gActors[j].vel.z += (temp_f16_2 * var_f28) + (temp_f18_2 * var_f24);
                            if (actor1->vel.x == 0) {
                                if (actor1->vel.z == 0.0f) {
                                    if (gActors[j].vel.x == 0.0f) {
                                        if (gActors[j].vel.z == 0.0f) {
                                            temp_f0_6 = CalcAngleBetween2DPoints(actor1->vel.x, actor1->vel.z, gActors[j].vel.x, gActors[j].vel.z);
                                            temp_f2_5 = __cosf(temp_f0_6 * 2 * PI / 360);
                                            temp_f2_5 += temp_f2_5;
                                            gActors[j].vel.x = temp_f2_5;
                                            actor1->vel.x = -temp_f2_5;
                                            temp_f12_4 = -__sinf(temp_f0_6 * 2 * PI / 360);
                                            gActors[j].vel.z = temp_f12_4 + temp_f12_4;
                                            actor1->vel.z = -gActors[j].vel.z;
                                        }
                                    }
                                }
                            }
                            temp_f20_3 = __sqrtf(temp_f20_2);
                            temp_f0_7 = __sqrtf(temp_f12_2);
                            if (temp_f20_3 != 0.0f) {
                                actor1->pos.x -= (spCC * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                actor1->pos.z -= (spC8 * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                gActors[j].pos.x += (spCC * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                gActors[j].pos.z += (spC8 * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                            }
                            //Actor_PlaySound(actor1, 0xA8, 1, 1);
                        }
                    }
                }
            }                
        }
    }
}

void main(void) {

}