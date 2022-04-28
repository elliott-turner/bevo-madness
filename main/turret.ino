float current_angle = 0.0;
bool direction = true;
int steps = 0;

void turret_start() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

float turret_get_angle() {
    return current_angle;
}

void set_coils(int in1, int in2, int in3, int in4) {
    
    digitalWrite(IN1, in1);
    digitalWrite(IN2, in2);
    digitalWrite(IN3, in3);
    digitalWrite(IN4, in4);
}

void turret_set_angle(float angle) {
    float angle_delta = angle - current_angle;
    direction = angle_delta > 0;
    int num_steps = (int)(abs(angle_delta)*NG/NP*STEPS_PER_DEG);
    int delay = DELAY_MAX;
    int i;
    for (i=0; i<num_steps; i++) {
        switch (steps) {
            case 0: SET_COILS(1, 1, 0, 0); break;
            case 1: SET_COILS(0, 1, 0, 0); break;
            case 2: SET_COILS(0, 1, 1, 0); break;
            case 3: SET_COILS(0, 0, 1, 0); break;
            case 4: SET_COILS(0, 0, 1, 1); break;
            case 5: SET_COILS(0, 0, 0, 1); break;
            case 6: SET_COILS(1, 0, 0, 1); break;
            case 7: SET_COILS(1, 0, 0, 0); break;
            default: SET_COILS(0, 0, 0, 0); break;
        }
        if (direction) { steps++; }
        else { steps--; }
        if (steps > 7) { steps = 0; }
        if (steps < 0) { steps = 7; }
        if (i < num_steps/2) {
            if (delay > DELAY_MIN) { delay--; }
        }
        else {
            if (num_steps - i < DELAY_MAX - DELAY_MIN) { delay++; }
        }
        delayMicroseconds(delay);
    }
    current_angle = angle;
}

void turret_move_angle(float angle) {
    turret_set_angle(current_angle + angle);
}
