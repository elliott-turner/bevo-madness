#include <SimpleRSLK.h>
#include <stdio.h>

#include "ultrasonic.h"
#include "ir.h"
#include "motion.h"
#include "turret.h"

void setup()
{
    Serial.begin(115200);
    setupRSLK();
    setupWaitBtn(LP_LEFT_BTN);

    ultrasonic_start();
    ir_start();
    motion_start();
    turret_start();
}

void loop()
{
    float distance[360];
    for (i = 0; i < 360; i++){
        distance[i] = measure_distance();
        move_turn(1);
    }

    float a = 10;
    int angle = 0;
    for (i = 0; i < 360; i++){
        if a >= distance[i]{
            a = distance[i];
            angle = i;
        }
    }

    int leftAngle = angle + 90;
    int rightAngle = angle - 90;
    if angle > 269{
        leftAngle = -1 * (360 - (angle + 90));
    }
    if angle < 89{
        rightAngle = 360 + (angle - 90);
    }
    float distFromLeft = distance[leftAngle];
    float distFromRight = distance[rightAngle];
    float closestWall = distance[leftAngle];
    if distFromLeft > distFromRight{
        closestWall = distFromRight;
        move_turn(angle+90);
    }
    else{
        move_turn(angle-90);
    }
    float toCenter = 36 - closestWall;
    move_straight(toCenter);


}
