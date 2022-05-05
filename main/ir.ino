#include <stdio.h>
#include "ir.h"

void ir_start() {
    pinMode(IR_L_PIN, INPUT);
    pinMode(IR_M_PIN, INPUT);
    pinMode(IR_R_PIN, INPUT);
}

bool detect_beacon(char beacon) {
    bool detected = false;
    int pin = IR_M_PIN;
    if (beacon == 'L') { pin = IR_L_PIN; }
    if (beacon == 'R') { pin = IR_R_PIN; }

    bool result = true;

    
    int i = 0;
    for (i=0; i<100; i++)
    {
        if (!digitalRead(pin)) {
            result = false;
            break;
        }
    }
    return !result;
}