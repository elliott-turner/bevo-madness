#include <stdio.h>
#include <msp432.h>
#include <SimpleRSLK.h>

#define NOP __asm__ __volatile__ ("nop\n\t")

void ultrasonic_start() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

float measure_distance() { // return distance in inches
    unsigned long start_time = micros();
    unsigned long end_time = 0;

    int i, j;

    digitalWrite(TRIG_PIN, HIGH);
    while (micros() < start_time + 6) { NOP; };
    digitalWrite(TRIG_PIN, LOW);

    for (i=0; i<10000; i++) {
        if (digitalRead(ECHO_PIN)) {
            start_time = micros();
            for (j=0; j<10000; j++) {
                if (!digitalRead(ECHO_PIN)) {
                    end_time = micros();
                    break;
                }
            }
            break;
        }
    }
    return (float)(end_time - start_time)/148.0;
}
