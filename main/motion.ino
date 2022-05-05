#include <stdio.h>
#include "SimpleRSLK.h"

double line_position = 0;

uint16_t sensor_vals[LS_NUM_SENSORS];

double last_time, curr_time, elap_time;

struct PID_Data {
    double p_error;
    double i_error;
    double d_error;
    unsigned long last_input;
};

struct PID_Data pid_data_l = {0x0};
struct PID_Data pid_data_r = {0x0};
struct PID_Data line_pid_data = {0x0};

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

    int num_steps = (int)(abs(dist)*41.58036592);
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
        if (millis() - last_time >= 100) {
            if (left_done && !right_done) { pid_step(0, SPEED_NORMAL); }
            else if (!left_done && right_done) { pid_step(SPEED_NORMAL, 0); }
            else { pid_step(SPEED_NORMAL, SPEED_NORMAL); }
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
        if (millis() - last_time >= 100) {
            if (left_done && !right_done) { pid_step(0, SPEED_NORMAL); }
            else if (!left_done && right_done) { pid_step(SPEED_NORMAL, 0); }
            else { pid_step(SPEED_NORMAL, SPEED_NORMAL); }
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

void move_to_line(float dist) {
    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(BOTH_MOTORS, SPEED_SLOW);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);

    bool at_line = false;
    int j;

    int num_steps = (int)(abs(dist)*41.58036592);
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
        if (millis() - last_time >= 100) {
            if (left_done && !right_done) { pid_step(0, SPEED_NORMAL); }
            else if (!left_done && right_done) { pid_step(SPEED_NORMAL, 0); }
            else { pid_step(SPEED_NORMAL, SPEED_NORMAL); }
        }
        if (left_done && right_done) { break; }

        readLineSensor(sensor_vals);
        for (j=0; j<LS_NUM_SENSORS; j++) {
            if (sensor_vals[j] >= BLACK_VAL) {
                at_line = true;
                break;
            }
        }

        if (at_line) {
            if (getEncoderLeftCnt() < getEncoderRightCnt()) { num_steps = getEncoderLeftCnt(); }
            else { getEncoderLeftCnt(); }
        }
    }
    move_stop();
}

void follow_line_to_intersection(float dist) {
    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(BOTH_MOTORS, SPEED_NORMAL);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);

    bool at_intersection = false;
    bool at_line = false;
    double previous_line_position = 0;
    int j;

    int num_steps = (int)(abs(dist)*41.58036592);
    bool left_done = false;
    bool right_done = false;
    pid_start();
    while (true) {
        readLineSensor(sensor_vals);
        at_intersection = true;
        for (j=0; j<LS_NUM_SENSORS; j++) {
            if (sensor_vals[j] < BLACK_VAL) {
                at_intersection = false;
                break;
            }
        }

        at_line = false;
        for (j=0; j<LS_NUM_SENSORS; j++) {
            if (sensor_vals[j] >= BLACK_VAL) {
                at_line = true;
                break;
            }
        }

        if (at_intersection) {
            if (getEncoderLeftCnt() < getEncoderRightCnt()) { num_steps = getEncoderLeftCnt(); }
            else { getEncoderLeftCnt(); }
        }

        // estimate line position using center of mass equation
        calculate_line_position();

        if (at_line) { previous_line_position = line_position; }
        else { line_position = previous_line_position; }

        if (getEncoderLeftCnt() > num_steps) {
            left_done = true;
            setMotorSpeed(LEFT_MOTOR, 0);
        }
        if (getEncoderRightCnt() > num_steps) {
            right_done = true;
            setMotorSpeed(RIGHT_MOTOR, 0);
        }
        if (millis() - last_time >= 100) {
            curr_time = (double)millis();
            elap_time = curr_time - last_time;

            line_pid_data.d_error = (line_position - line_pid_data.p_error) / elap_time;
            line_pid_data.p_error = line_position;
            line_pid_data.i_error += line_pid_data.p_error * elap_time;
            double response = 
                line_pid_data.p_error * LINE_GAIN_P +
                line_pid_data.i_error * LINE_GAIN_I +
                line_pid_data.d_error * LINE_GAIN_D;

            Serial.println(line_pid_data.p_error);
            Serial.println(line_pid_data.i_error);
            Serial.println(line_pid_data.d_error);
            Serial.println(line_position);
            Serial.println(response);
            Serial.println();

            double speed_l = SPEED_NORMAL - (int)response;
            double speed_r = SPEED_NORMAL + (int)response;

            if (speed_l < 0) { speed_l = 0; }
            if (speed_r < 0) { speed_r = 0; }
            if (speed_l > 20) { speed_l = 20; }
            if (speed_r > 20) { speed_r = 20; }

            if (left_done && !right_done) { pid_step(0, speed_r); }
            else if (!left_done && right_done) { pid_step(speed_l, 0); }
            else { pid_step(speed_l, speed_r); }
        }
        if (left_done && right_done) { break; }
    }
    move_stop();
}

void straighten_on_line(float dist) {
    float front_dist = 0.0;
    float back_dist = 0.0;
    float front_offset = 0.0;
    float back_offset = 0.0;
    int i, num_black, offset_index_sum, num_steps;
    float offset_index;
    bool found_line, left_done, right_done;

    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(BOTH_MOTORS, SPEED_SLOW);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_FORWARD);

    num_steps = (int)(abs(dist)*41.58036592);
    left_done = false;
    right_done = false;
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
        if (millis() - last_time >= 100) {
            if (left_done && !right_done) { pid_step(0, SPEED_NORMAL); }
            else if (!left_done && right_done) { pid_step(SPEED_NORMAL, 0); }
            else { pid_step(SPEED_NORMAL, SPEED_NORMAL); }
        }
        if (left_done && right_done) { break; }

        num_black = 0;
        readLineSensor(sensor_vals);
        for (i=0; i<LS_NUM_SENSORS; i++) {
            if (sensor_vals[i] >= BLACK_VAL) {
                num_black++;
            }
        }
        if (num_black <= 1) {
            if (getEncoderLeftCnt() < getEncoderRightCnt()) { num_steps = getEncoderLeftCnt(); }
            else { getEncoderLeftCnt(); }
        }
    }

    front_dist = (float)num_steps / 41.58036592;
    calculate_line_position();
    front_offset = line_position * 7;

    move_straight(-dist);

    resetLeftEncoderCnt();
	resetRightEncoderCnt();

    setMotorSpeed(BOTH_MOTORS, SPEED_SLOW);
    setMotorDirection(BOTH_MOTORS, MOTOR_DIR_BACKWARD);

    num_steps = (int)(abs(dist)*41.58036592);
    left_done = false;
    right_done = false;
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
        if (millis() - last_time >= 100) {
            if (left_done && !right_done) { pid_step(0, SPEED_NORMAL); }
            else if (!left_done && right_done) { pid_step(SPEED_NORMAL, 0); }
            else { pid_step(SPEED_NORMAL, SPEED_NORMAL); }
        }
        if (left_done && right_done) { break; }

        num_black = 0;
        readLineSensor(sensor_vals);
        for (i=0; i<LS_NUM_SENSORS; i++) {
            if (sensor_vals[i] >= BLACK_VAL) {
                num_black++;
            }
        }
        if (num_black <= 1) {
            if (getEncoderLeftCnt() < getEncoderRightCnt()) { num_steps = getEncoderLeftCnt(); }
            else { getEncoderLeftCnt(); }
        }
    }

    back_dist = (float)num_steps / 41.58036592;
    calculate_line_position();
    back_offset = line_position * 7;

    float y = front_dist + back_dist;
    float x = front_offset - back_offset;
    float angle = atan(x/y)*180.0/2/3.1415*-1;

    Serial.print(y);
    Serial.print(" ");
    Serial.print(x);
    Serial.print(" ");
    Serial.print(angle);
    Serial.println();

    move_straight(dist);
    delay(150);
    move_turn(angle);
}

void pid_start() {
    last_time = millis();
    memset(&pid_data_l, 0x0, sizeof(pid_data_l));
    memset(&pid_data_r, 0x0, sizeof(pid_data_r));
    memset(&line_pid_data, 0x0, sizeof(line_pid_data));
}

void pid_step(float set_l, float set_r) {
    curr_time = (double)millis();
    elap_time = curr_time - last_time;

    double error, response, steps;

    steps = (double)getEncoderLeftCnt() - pid_data_l.last_input;
    pid_data_l.last_input += steps;
    error = steps - set_l * elap_time * 0.0175;
    error = -1*error;
    pid_data_l.d_error = (error - pid_data_l.p_error) / elap_time;
    pid_data_l.p_error = error;
    pid_data_l.i_error += pid_data_l.p_error * elap_time;
    response =  pid_data_l.p_error * MOTOR_GAIN_P +
                pid_data_l.i_error * MOTOR_GAIN_I +
                pid_data_l.d_error * MOTOR_GAIN_D;
    setMotorSpeed(LEFT_MOTOR, response / elap_time / 0.0175);

    steps = (double)getEncoderRightCnt() - pid_data_r.last_input;
    pid_data_r.last_input += steps;
    error = steps - set_r * elap_time * 0.0175;
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

void calculate_line_position() {
    line_position = (
            1.00 * sensor_vals[0] + 
            0.71 * sensor_vals[1] +
            0.43 * sensor_vals[2] +
            0.14 * sensor_vals[3] +
            -0.14 * sensor_vals[4] +
            -0.43 * sensor_vals[5] +
            -0.71 * sensor_vals[6] +
            -1.00 * sensor_vals[7]
        ) / (
            sensor_vals[0] +
            sensor_vals[1] +
            sensor_vals[2] +
            sensor_vals[3] +
            sensor_vals[4] +
            sensor_vals[5] +
            sensor_vals[6] +
            sensor_vals[7]
        );
}
