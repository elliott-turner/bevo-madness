#include <stdio.h>
#include "SimpleRSLK.h"

float line_position = 0;

uint16_t sensor_vals[LS_NUM_SENSORS];

double last_time, curr_time, elap_time;

struct PID_Data {
    double p_error;
    double i_error;
    double d_error;
    unsigned long last_encoder;
};

struct PID_Data pid_data_l = {0x0};
struct PID_Data pid_data_r = {0x0};

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

    int num_steps = (int)(abs(dist)*40.0);
    bool left_done = false;
    bool right_done = false;
    pid_start();
    while (true) {
        if (getEncoderLeftCnt() > num_steps) {
            left_done = true;
            setMotorSpeed(LEFT_MOTOR, 0);
        }
        if (getEncoderRightCnt() > num_steps) {
            right_done = true;
            setMotorSpeed(RIGHT_MOTOR, 0);
        }
        if (!left_done && !right_done && millis() - last_time >= 100) {
            pid_step(SPEED_NORMAL, SPEED_NORMAL);
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
    
    int num_steps = (int)(abs(deg)*2.05);
    bool left_done = false;
    bool right_done = false;
    pid_start();
    while (true) {
        if (getEncoderLeftCnt() > num_steps) {
            left_done = true;
            setMotorSpeed(LEFT_MOTOR, 0);
        }
        if (getEncoderRightCnt() > num_steps) {
            right_done = true;
            setMotorSpeed(RIGHT_MOTOR, 0);
        }
        if (!left_done && !right_done && millis() - last_time >= 100) {
            pid_step(SPEED_NORMAL, SPEED_NORMAL);
        }
        if (left_done && right_done) { break; }
    }
}

void move_stop(void) {
    setMotorSpeed(BOTH_MOTORS, 0);
}

bool check_for_line(void) {
    bool at_line = false;
    int j;
    readLineSensor(sensor_vals);
    for (j=0; j<LS_NUM_SENSORS; j++) {
        if (sensor_vals[j] >= BLACK_VAL) {
            at_line = true;
            break;
        }
    }
    return at_line;
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

void pid_start() {
    last_time = millis();
    memset(&pid_data_l, 0x0, sizeof(pid_data_l));
    memset(&pid_data_r, 0x0, sizeof(pid_data_r));
}

void pid_step(float set_l, float set_r) {
    curr_time = (double)millis();
    elap_time = curr_time - last_time;

    double error, response, steps;

    steps = (double)getEncoderLeftCnt() - pid_data_l.last_encoder;
    pid_data_l.last_encoder += steps;
    error = steps - set_l * elap_time * 0.0175;
    error = -1*error;
    pid_data_l.d_error = (error - pid_data_l.p_error) / elap_time;
    pid_data_l.p_error = error;
    pid_data_l.i_error += pid_data_l.p_error * elap_time;
    response =  pid_data_l.p_error * MOTOR_GAIN_P +
                pid_data_l.i_error * MOTOR_GAIN_I +
                pid_data_l.d_error * MOTOR_GAIN_D;
    setMotorSpeed(LEFT_MOTOR, response / elap_time / 0.0175);

    steps = (double)getEncoderRightCnt() - pid_data_r.last_encoder;
    pid_data_r.last_encoder += steps;
    error = steps - set_l * elap_time * 0.0175;
    error = -1*error;
    pid_data_r.d_error = (error - pid_data_r.p_error) / elap_time;
    pid_data_r.p_error = error;
    pid_data_r.i_error += pid_data_r.p_error * elap_time;
    response =  pid_data_r.p_error * MOTOR_GAIN_P +
                pid_data_r.i_error * MOTOR_GAIN_I +
                pid_data_r.d_error * MOTOR_GAIN_D;
    setMotorSpeed(RIGHT_MOTOR, response / elap_time / 0.0175);
    
    // TODO: repeat above calculations for right motor

    last_time = curr_time;
}
