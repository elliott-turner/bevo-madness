#include <SimpleRSLK.h>
#include <stdio.h>

#include "ultrasonic.h"
#include "ir.h"
#include "motion.h"
#include "turret.h"
#include "shooter.h"

void setup()
{
    Serial.begin(115200);
    setupRSLK();
    setupWaitBtn(LP_LEFT_BTN);
    pinMode(BP_SW_PIN_5, INPUT_PULLUP);
    pinMode(BP_SW_PIN_4, INPUT_PULLUP);

    ultrasonic_start();
    ir_start();
    motion_start();
    turret_start();
    shooter_start();
}

void loop()
{
    while (true) {
        if (!digitalRead(BP_SW_PIN_4)) {
            load();
        }
        if (!digitalRead(BP_SW_PIN_5)) {
            break;
        }
    }
    
    // NAVIGATION
    // find a point on back wall

    char output[64] = {0x0};
    Serial.println("STARTING");
    float dist = 0.0;

    int i = 0;
    int num_short_dist = 0;
    for (i=0; i<36; i++) {
        dist = measure_distance();
        Serial.println(dist);
        if (dist <= 16) { num_short_dist++; }
        if (num_short_dist > 3) { break; }
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

    bool on_left_side = true;
    if (check_for_line()) { move_turn(180); }
    // move to center line
    else {
        Serial.println("MOVING TO CENTER LINE");
        move_turn(90);
        if (measure_distance() < 36) {
            move_turn(-180);
            on_left_side = false;
        }
        bool found_line = false;
        for (i=0; i<64; i++) {
            move_straight(0.25);
            if (check_for_line()) {
                move_straight(0.5);
                if (on_left_side) { move_turn(90); }
                else { move_turn(-90); }
                found_line = true;
                break;
            }
        }
    }

    // TODO: what to do if line still not found
    Serial.println("FOLLOING TO INTERSECTION");
    follow_line_to_intersection(20);
    Serial.println("CROSSING INTERSECTION");
    move_straight(1.5);
    Serial.println("FOLLOING FOR 12 IN");
    follow_line_to_intersection(12);

    Serial.println("STRAIGHTENING");
    straighten_on_line(4.0);

    bool correct_distance = false;
    float avg_dist = 0.0;
    for (i=0; i<3; i++) {
        dist = measure_distance();
        if (dist < 29 || dist > 31) { avg_dist += dist; }
        else { correct_distance = true; }
    } 
    if (!correct_distance) {
        avg_dist = avg_dist / 3.0;
        move_straight(avg_dist - 30);
    }

    char last_basket = ' ';
    disableMotor(BOTH_MOTORS);
    while (true) {
        bool ir_l = detect_beacon('L');
        bool ir_m = detect_beacon('M');
        bool ir_r = detect_beacon('R');

        sprintf(output, "%d %d %d", ir_l, ir_m, ir_r);
        Serial.println(output);

        if (ir_l) {
            if (last_basket != 'L') {
                turret_set_angle(-28);
                shoot(780);
                last_basket = 'L';
            }
        }
        else if (ir_r) {
            if (last_basket != 'R') {
            turret_set_angle(28);
            shoot(780);
            last_basket = 'R';
            }
        }
        else if (ir_m) {
            if (last_basket != 'M') {
            turret_set_angle(0);
            shoot(730);
            last_basket = 'M';
            }
        }

        delay(1500);
    }
}
