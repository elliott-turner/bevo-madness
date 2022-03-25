#include <stdio.h>

int IR_PIN = A15;

void setup() {
  Serial.begin(115200);
  Serial.print("SETUP");
  pinMode(IR_PIN, INPUT);
}

void loop() {
  int sensorValue = analogRead(IR_PIN);
  if (sensorValue > 500) { Serial.print("0\n"); }
  else { Serial.print("1\n"); }
}
