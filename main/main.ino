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
    // NAVIGATION
    // find a point on back wall

    char output[64] = {0x0};
    Serial.println("STARTING");
    float dist = 0.0;

    int i = 0;
    for (i=0; i<36; i++) {
        dist = measure_distance();
        Serial.println(dist);
        if (dist <= 20) { break; }
        move_turn(10);
    }
    Serial.println("AIMING");
    // point directly at back wall
    float measurements[45] = {0.0};
    move_turn(-90);
    for (i=0; i<45; i++) {
        measurements[i] = measure_distance();
        Serial.println(measurements[i]);
        move_turn(4);
    }
    Serial.println("CLEANING");
    float cleaned[41] = {0x0};
    for (i=2; i<43; i++) {
        cleaned[i-2] = {measurements[i-2] + 
                        measurements[i-1] +
                        measurements[i]   + 
                        measurements[i+1] +
                        measurements[i+2]};
        cleaned[i-2] = cleaned[i-2] / 5.0;
        Serial.println(cleaned[i-2]);
    }
    int min_index = 0;
    float min = 20.0;
    for (i=0; i<41; i++) {
        if (cleaned[i] < min) {
            min = cleaned[i];
            min_index = i;
        }
    }
    sprintf(output, "MIN: %02f \tINDEX: %d", min, min_index);
    Serial.println(output);
    move_turn(-((45-min_index)*4));
    Serial.println("DONE");

    // move to center line
    move_turn(90);
    bool on_left_side = true;
    if (measure_distance() < 36) {
        move_turn(-180);
        on_left_side = false;
    }
    move_to_line(16);
    if (on_left_side) { move_turn(90); }
    else { move_turn(-90); }

    // TODO: what to do if line still not found
    move_straight(18);

    while (true) {}
}
