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
    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(RIGHT_MOTOR, SPEED_NORMAL);
    setMotorSpeed(LEFT_MOTOR, SPEED_NORMAL-0.2);
    if (dist > 0) {
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);
    }
    else {
        setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);
    }

    int num_steps = (int)(dist*40.0);
    bool left_done = false;
    bool right_done = false;
    while (true) {
        if (getEncoderLeftCnt() > num_steps) {
            left_done = true;
            setMotorSpeed(LEFT_MOTOR, 0);
        }
        if (getEncoderRightCnt() > num_steps) {
            right_done = true;
            setMotorSpeed(RIGHT_MOTOR, 0);
        }
        if (left_done && right_done) { break; }
    }
}

void move_turn(float deg) {
    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(RIGHT_MOTOR, SPEED_NORMAL);
    setMotorSpeed(LEFT_MOTOR, SPEED_NORMAL-0.2);
    if (deg > 0) {
        setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_BACKWARD);
        setMotorDirection(LEFT_MOTOR, MOTOR_DIR_FORWARD);
    }
    else {
        setMotorDirection(RIGHT_MOTOR, MOTOR_DIR_FORWARD);
        setMotorDirection(LEFT_MOTOR, MOTOR_DIR_BACKWARD);
    }
    
    int num_steps = (int)(deg*2.05);
    bool left_done = false;
    bool right_done = false;
    while (true) {
        if (getEncoderLeftCnt() > num_steps) {
            left_done = true;
            setMotorSpeed(LEFT_MOTOR, 0);
        }
        if (getEncoderRightCnt() > num_steps) {
            right_done = true;
            setMotorSpeed(RIGHT_MOTOR, 0);
        }
        if (left_done && right_done) { break; }
    }
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