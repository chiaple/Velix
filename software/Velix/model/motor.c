//
// Created by 18135 on 26-1-17.
//

#include "motor.h"

void motor_Init(MOTOR *p,int Num,int dir,int polePairs){
    p->Number = Num;
    p->dir = dir;
    p->polePairs = polePairs;
}
