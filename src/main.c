#include <stdio.h>
#include <math.h>

#include "main.h"

float __cosf( float x );
float __sinf( float x );
void ActorTick_MinigameActor(Actor* arg0);

//this bss needs to be initialized to something, but for now we wont so it at least builds
//this means currently, the input and output of the program is random garbage
Actor gActors[ACTORS_MAX]; 
Tongue* gTongueOnePointer;
PlayerActor* gCurrentActivePlayerPointer;

f32 angleLookup[] = {0.0f, 0.8951740265f, 1.789911032f, 2.683774948f, 3.576334f, 4.467158794f, 5.355824947f, 6.241913795f, 7.125016212f, 8.004729271f, 8.880659103f, 9.752425194f, 10.61965466f, 11.48199081f, 12.33908653f, 13.19061089f, 14.03624344f, 14.87568188f, 15.70863819f, 16.53483772f, 17.35402489f, 18.1659565f, 18.97040749f, 19.76716995f, 20.55604553f, 21.33685875f, 22.10944748f, 22.87366486f, 23.62937737f, 24.37646866f, 25.11483574f, 25.84438705f, 26.56505013f, 27.27676392f, 27.97947311f, 28.6731472f, 29.35775375f, 30.03327942f, 30.69972229f, 31.35708427f, 32.00538254f, 32.64464188f, 33.27488708f, 33.89616776f, 34.50852203f, 35.11201096f, 35.70669174f, 36.29262924f, 36.86989975f, 37.43857193f, 37.99873352f, 38.55046463f, 39.09386063f, 39.62900543f, 40.15599823f, 40.6749382f, 41.18592453f, 41.68906021f, 42.18444443f, 42.67218399f, 43.15238953f, 43.62516403f, 44.09061813f, 44.54886246f, 45.0f};

f32 CalculateAngleOfVector(f32 x, f32 y) {
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
    lookupOne = angleLookup[floor];
    lookupTwo = angleLookup[next_index];
    
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

void ActorTick_CueBall(Actor* cueBall) {
    f32 angleOfCollision;
    f32 segXPos;
    f32 segZPos;
    
    // if the collision was hit by a tongue segment
    if (cueBall->tongueBumpSeg != 0) {
        // if the player's tongue length is that of the segment that hit the ball 
        //(approximately adjusting for the fact the hit may have retracted one seg)
        if ((gTongueOnePointer->segments == cueBall->tongueBumpSeg) || gTongueOnePointer->segments == (cueBall->tongueBumpSeg + 1)) {
            // if the player is not vaulting
            if (gTongueOnePointer->vaulting == 0) {
                // the coordinate of the last tongue segment assuming tongue pos is player relative
                segXPos = gTongueOnePointer->tongueXs[gTongueOnePointer->segments - 1] + gCurrentActivePlayerPointer->pos.x;
                segZPos = gTongueOnePointer->tongueZs[gTongueOnePointer->segments - 1] + gCurrentActivePlayerPointer->pos.z;

                angleOfCollision = CalcAngleBetween2DPoints(segXPos, segZPos, cueBall->pos.x, cueBall->pos.z);

                cueBall->vel.x = __cosf(DEGREES_TO_RADIANS_2PI(angleOfCollision)) * cueBall->friction._f32.x;
                cueBall->vel.z = -__sinf(DEGREES_TO_RADIANS_2PI(angleOfCollision)) * cueBall->friction._f32.x;
            }
        }
    }
    
    ActorTick_MinigameActor(cueBall);
    
    if (cueBall->pos.y < -cueBall->unknownPositionThings[0].unk_10) {
        cueBall->unk_98 = 1;
        cueBall->pos.x = gCurrentActivePlayerPointer->pos.x;
        cueBall->pos.y = cueBall->unk_134[0] + 1000.0f;
        cueBall->pos.z = gCurrentActivePlayerPointer->pos.z;
        cueBall->unk_A0.unk_08 = 0;
        if (cueBall->pos.x > 1800.0f) {
            cueBall->pos.x = 1800.0f;
        }
        if (cueBall->pos.x < -1800.0f) {
            cueBall->pos.x = -1800.0f;
        }
        if (cueBall->pos.z > 900.0f) {
            cueBall->pos.z = 900.0f;
        }
        if (cueBall->pos.z < -900.0f) {
            cueBall->pos.z = -900.0f;
        }
        cueBall->vel.z = 0.0f;
        cueBall->vel.y = 0.0f;
        cueBall->vel.x = 0.0f;
    }
}

void ActorTick_MinigameActor(Actor* actor) {
    f32 temp_f16;
    f32 temp_f6;

    // Friction-based slowdown over time
    actor->vel.x -= (actor->vel.x * actor->friction._f32.y);
    actor->vel.z -= (actor->vel.z * actor->friction._f32.y);

    // If the magnitude of velocity falls below a threshold, set it to zero
    if (((actor->vel.x * actor->vel.x) + (actor->vel.z * actor->vel.z)) < 1.0f) {
        actor->vel.x = actor->vel.z = 0.0f;
    }

    // Handling vertical velocity when affected by gravity
    if (actor->unk_98 != 0) {
        if (actor->pos.y > 0.0f) {
            actor->vel.y -= 3.2f + (actor->vel.y * 0.05f);
        } else {
            actor->vel.y -= 6.4f;
        }
    } else if (actor->vel.y != 0) {
        actor->vel.y = 0;
        actor->unk_A0.unk_08 = 1;
    }

    // Handling additional physics effects
    if (actor->unk_9C != 0) {
        // Calculate some temporary values for further physics calculations
        temp_f16 = (actor->unk_B4 * actor->unk_B4) + (actor->unk_BC * actor->unk_BC);
        temp_f6 = (actor->vel.x * actor->unk_B4) + (actor->vel.z * actor->unk_BC);

        // Update velocity components based on additional physics effects
        actor->vel.x -= ((2 * temp_f6 * actor->unk_B4) / temp_f16);
        actor->vel.z -= ((2 * temp_f6 * actor->unk_BC) / temp_f16);
    }
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
        // Skip actors that are not cue balls or billiards balls
        if (actor1->actorID != CUE_BALL && actor1->actorID != BILLIARDS_BALL) {
            continue;
        }
        
        for (j = i + 1; j < ACTORS_MAX; j++) {
            temp_f20 = actor1->unknownPositionThings[0].unk_0C + gActors[j].unknownPositionThings[0].unk_0C;
            temp_f16 = actor1->pos.x - gActors[j].pos.x;
            temp_f18 = actor1->pos.z - gActors[j].pos.z;
            spCC = gActors[j].pos.x - actor1->pos.x;
            spC8 = gActors[j].pos.z - actor1->pos.z;
            if ((gActors[j].actorID == CUE_BALL) || (gActors[j].actorID == BILLIARDS_BALL)) {
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
                            temp_f20_3 = sqrtf(temp_f20_2);
                            temp_f0_7 = sqrtf(temp_f12_2);
                            if (temp_f20_3 != 0.0f) {
                                actor1->pos.x -= (spCC * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                actor1->pos.z -= (spC8 * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                gActors[j].pos.x += (spCC * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                                gActors[j].pos.z += (spC8 * (temp_f20 - temp_f0_7)) / (temp_f20_3 * 2);
                            }
                        }
                    }
                }
            }                
        }
    }
}

int main(void) {
    return 0;
}