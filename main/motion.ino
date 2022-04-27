#include <stdio.h>
#include "SimpleRSLK.h"

float line_position = 0;

uint16_t sensor_vals[LS_NUM_SENSORS];

void motion_start() {
    line_position = 0;
    enableMotor(BOTH_MOTORS);
    setMotorSpeed(BOTH_MOTORS, 0);
}

void move_straight(float dist) {
    setMotorSpeed(BOTH_MOTORS, SPEED_NORMAL);
    if (dist > 0) {
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
    }
    else {
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);
    }
    delay(abs(dist)*100.0);
    move_stop();
}

void move_turn(float deg) {
    setMotorSpeed(BOTH_MOTORS, SPEED_NORMAL);
    if (deg > 0) {
        setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
        setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    }
    else {
        setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
        setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    }
    delay(abs(deg)*10.0);
    move_stop();
}

void move_stop(void) {
    setMotorSpeed(BOTH_MOTORS, 0);
}

void move_to_line(void) {
    setMotorSpeed(BOTH_MOTORS, SPEED_NORMAL);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);

    bool at_line = false;
    int i, j;
    i = 0;
    while (i < 1000) {
        readLineSensor(sensor_vals);
        for (j=0; j<LS_NUM_SENSORS; j++) {
            if (sensor_vals[j] >= BLACK_VAL) {
                at_line = true;
                break;
            }
        }
        if (at_line) { break; }
    }
    move_stop();
}