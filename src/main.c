#include <stdio.h>
#include <math.h>

#include "main.h"

/* weird idea: miss a cue ball off spawn and jump above the triangle so it spawns there and falls with gravity */

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

void ActorInit_CueBall(Actor* cueBallActor) {
    cueBallActor->unk_134[0] = cueBallActor->pos.y;
}

/* Billiards balls aren't affected by gravity (y-vel) they fall when hititing a pot */
void ActorTick_BilliardBall(Actor* ball) {
    f32 velocityMagnitude;

    ActorTick_MinigameActor(ball);
    velocityMagnitude = NORM_2(ball->vel.x, ball->vel.z);
    ball->unk_94 = velocityMagnitude;
    // ball->unknownPositionThings[0].unk_0C => tYPos
    ball->unk_134[0] = ((180.0f * velocityMagnitude) / ( ball->unknownPositionThings[0].unk_0C * PI)) + ball->unk_134[0];
    ball->unk_90 = CalculateAngleOfVector(ball->vel.x, -ball->vel.z);
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

                // friction (x) of cue ball seems to always be 100 (something to do with intial velocity?)
                cueBall->vel.x = __cosf(DEGREES_TO_RADIANS_2PI(angleOfCollision)) * cueBall->friction._f32.x;
                cueBall->vel.z = -__sinf(DEGREES_TO_RADIANS_2PI(angleOfCollision)) * cueBall->friction._f32.x;
            }
        }
    }
    
    ActorTick_MinigameActor(cueBall);
    
    // If the cue ball falls out of bounds (foul shot), reset it to the player's position
    if (cueBall->pos.y < -cueBall->unknownPositionThings[0].unk_10) {
        cueBall->unk_98 = 1;
        cueBall->pos.x = gCurrentActivePlayerPointer->pos.x;
        cueBall->pos.y = cueBall->unk_134[0] + 1000.0f;
        cueBall->pos.z = gCurrentActivePlayerPointer->pos.z;
        cueBall->unk_A0.unk_08 = 0;

        // Respawn the ball to the closest set point on the table if it goes out of bounds
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

        // Reset the velocity of the ball
        cueBall->vel.z = 0.0f;
        cueBall->vel.y = 0.0f;
        cueBall->vel.x = 0.0f;
    }
}

/* Only called by cue ball */
void ActorTick_MinigameActor(Actor* actor) {
    f32 distToNextX;
    f32 temp_f6;

    // Friction-based slowdown over time
    // 0.009999999776482582f is the default value for friction (y)
    actor->vel.x -= (actor->vel.x * actor->friction._f32.y);
    actor->vel.z -= (actor->vel.z * actor->friction._f32.y);

    // If the magnitude of velocity falls below a threshold, set it to zero
    if ((SQ(actor->vel.x) + SQ(actor->vel.z)) < 1.0f) {
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
        distToNextX = (actor->unk_B4 * actor->unk_B4) + (actor->unk_BC * actor->unk_BC);
        temp_f6 = (actor->vel.x * actor->unk_B4) + (actor->vel.z * actor->unk_BC);

        // Update velocity components based on additional physics effects
        actor->vel.x -= ((2 * temp_f6 * actor->unk_B4) / distToNextX);
        actor->vel.z -= ((2 * temp_f6 * actor->unk_BC) / distToNextX);
    }
}

void MinigameActors_PhysicsTick(void) {
    // Check for collisions between ball actors
    Actor* currentBall;
    Actor nextBall;
    f32 projVelVecCurrActor;
    f32 temp_f0_6;
    f32 distanceToNextActor;
    f32 distanceSquaredToNextActor;
    f32 temp_f12_4;
    f32 distToNextX;
    f32 temp_f16_2;
    f32 distFromNextX;
    f32 distFromNextZ;
    f32 distToNextZ;
    f32 tempY;
    f32 temp_f18_2;
    f32 distanceSquaredFromNextActor;
    f32 distanceFromNextActor;
    f32 projVelVecNextActor;
    f32 temp_f2_5;
    f32 var_f20;
    f32 projectedXVelCurrActor;
    f32 projectedZVelCurrActor;
    f32 projectedXVelNextActor;
    f32 projectedZVelNextActor;
    f32 var_f2;
    s32 i, j;

    
    for (currentBall = gActors, i = 0; i < ACTORS_MAX; i++, currentBall++) {
        // Skip actors that are not cue balls or billiards balls
        if (currentBall->actorID != CUE_BALL && currentBall->actorID != BILLIARDS_BALL) {
            continue;
        }
        
        // loop through remaining uncalculated actors
        // the naming here assumes an action is being performed ON the next actor (2)
        for (j = i + 1; j < ACTORS_MAX; j++) {
            nextBall = gActors[j];

            tempY = currentBall->unknownPositionThings[0].unk_0C + nextBall.unknownPositionThings[0].unk_0C;
            distToNextX = currentBall->pos.x - nextBall.pos.x;         // from actor 1 TO actor 2
            distToNextZ = currentBall->pos.z - nextBall.pos.z;
            distFromNextX = nextBall.pos.x - currentBall->pos.x;       // FROM actor 2 to actor 1
            distFromNextZ = nextBall.pos.z - currentBall->pos.z;

            if ((nextBall.actorID == CUE_BALL) || (nextBall.actorID == BILLIARDS_BALL)) {
                distanceSquaredToNextActor = SQ(distToNextX) + SQ(distToNextZ);
                if (!(SQ(tempY) < distanceSquaredToNextActor)) {
                    if (!((currentBall->unknownPositionThings[0].unk_10 + currentBall->pos.y) < nextBall.pos.y)) {
                        if (!((nextBall.unknownPositionThings[0].unk_10 + nextBall.pos.y) < currentBall->pos.y)) {

                            /* Project velocities of each actor onto the vector that lays between them */
                            // predefine as 0 incase of division by 0
                            projectedZVelNextActor = 0.0f;
                            projectedXVelCurrActor = 0.0f;
                            projectedZVelCurrActor = 0.0f;
                            projectedXVelNextActor = 0.0f;

                            // calculate the velocity of each actor along the relative vector between them
                            projVelVecCurrActor = (currentBall->vel.x * distFromNextX) + (distFromNextZ * currentBall->vel.z);
                            projVelVecNextActor = (distFromNextZ * nextBall.vel.z) + (nextBall.vel.x * distFromNextX);
                            distanceSquaredFromNextActor = SQ(distFromNextX) + SQ(distFromNextZ);
                            if (0.0f != distanceSquaredFromNextActor) {
                                projectedXVelCurrActor = (projVelVecCurrActor * distFromNextX) / distanceSquaredFromNextActor;
                                projectedZVelCurrActor = (projVelVecCurrActor * distFromNextZ) / distanceSquaredFromNextActor;
                                projectedXVelNextActor = (projVelVecNextActor * distFromNextX) / distanceSquaredFromNextActor;
                                projectedZVelNextActor = (projVelVecNextActor * distFromNextZ) / distanceSquaredFromNextActor;
                            }

                            if (currentBall->actorID == CUE_BALL) {
                                var_f20 = 2.5f;
                            } else {
                                var_f20 = 1.0f;
                            }        
                            var_f2 = 1.0f;

                            currentBall->vel.x = currentBall->vel.x - projectedXVelCurrActor;
                            currentBall->vel.z = currentBall->vel.z - projectedZVelCurrActor;
                            nextBall.vel.x -= projectedXVelNextActor;
                            nextBall.vel.z -= projectedZVelNextActor;
                            currentBall->vel.x += (((var_f20 - var_f2) / (var_f20 + var_f2)) * projectedXVelCurrActor) + (((2 * var_f2) / (var_f20 + var_f2)) * projectedXVelNextActor);
                            temp_f16_2 = (var_f2 - var_f20) / (var_f20 + var_f2);
                            currentBall->vel.z += (((var_f20 - var_f2) / (var_f20 + var_f2)) * projectedZVelCurrActor) + (((2 * var_f2) / (var_f20 + var_f2)) * projectedZVelNextActor);
                            temp_f18_2 = (2 * var_f20) / (var_f20 + var_f2);
                            nextBall.vel.x += (temp_f16_2 * projectedXVelNextActor) + (temp_f18_2 * projectedXVelCurrActor);
                            nextBall.vel.z += (temp_f16_2 * projectedZVelNextActor) + (temp_f18_2 * projectedZVelCurrActor);

                            // I dont understand this at all its always 0 vectors?
                            if (currentBall->vel.x == 0.0f) {
                                if (currentBall->vel.z == 0.0f) {
                                    if (nextBall.vel.x == 0.0f) {
                                        if (nextBall.vel.z == 0.0f) {
                                            temp_f0_6 = CalcAngleBetween2DPoints(currentBall->vel.x, currentBall->vel.z, nextBall.vel.x, nextBall.vel.z);
                                            temp_f2_5 = __cosf(temp_f0_6 * 2 * PI / 360);
                                            nextBall.vel.x = 2 * temp_f2_5;
                                            currentBall->vel.x = -temp_f2_5;
                                            temp_f12_4 = -__sinf(temp_f0_6 * 2 * PI / 360);
                                            nextBall.vel.z = 2 * temp_f12_4;
                                            currentBall->vel.z = -nextBall.vel.z;
                                        }
                                    }
                                }
                            }
                            
                            // sqrt to get magnitudes
                            distanceFromNextActor = sqrtf(distanceSquaredFromNextActor);
                            distanceToNextActor = sqrtf(distanceSquaredToNextActor);
                            if (distanceFromNextActor != 0.0f) {
                                currentBall->pos.x -= (distFromNextX * (tempY - distanceToNextActor)) / (distanceFromNextActor * 2);
                                currentBall->pos.z -= (distFromNextZ * (tempY - distanceToNextActor)) / (distanceFromNextActor * 2);
                                nextBall.pos.x += (distFromNextX * (tempY - distanceToNextActor)) / (distanceFromNextActor * 2);
                                nextBall.pos.z += (distFromNextZ * (tempY - distanceToNextActor)) / (distanceFromNextActor * 2);
                            }
                        }
                    }
                }
            }                
        }
    }
}

int main(void) {
    int tick = 0;
    printf("%d", tick);
    // Loop Ticks
    /* while (1) {
        MinigameActors_PhysicsTick();

        // Increment tick
        tick++;
    }*/

    return 0;
}