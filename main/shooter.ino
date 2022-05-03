int en1_position = 0;
int en2_position = 0;

void shooter_start() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pinMode(EN1A, INPUT_PULLUP);
    pinMode(EN1B, INPUT_PULLUP);
    pinMode(EN2A, INPUT_PULLUP);
    pinMode(EN2B, INPUT_PULLUP);

    pinMode(IR, INPUT_PULLDOWN);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    attachInterrupt(EN1A, en1a_isr, RISING);
    attachInterrupt(EN1A, en1a_isr, FALLING);
    attachInterrupt(EN1B, en1b_isr, RISING);
    attachInterrupt(EN1B, en1b_isr, FALLING);
    attachInterrupt(EN2A, en2a_isr, RISING);
    attachInterrupt(EN2A, en2a_isr, FALLING);
    attachInterrupt(EN2B, en2b_isr, RISING);
    attachInterrupt(EN2B, en2b_isr, FALLING);
}

void shoot(int power) {
    analogWrite(ENA, 255);
    int i=0;
    for (i=0; i<1000; i++) {
        en1_position = 0;
        delay(10);
        Serial.println(abs(en1_position));
        if (abs(en1_position) > power) { break; }
    }
    delay(100);
    en2_position = 0;
    analogWrite(ENB, 255);
    Serial.println("FIRING");

    while (abs(en2_position) < 300 && digitalRead(IR)) { delay(1); }
    Serial.print("BALL DETECTED: ");
    Serial.println(abs(en2_position));
    en2_position = 0;
    while (abs(en2_position) < 30) { delay(1); }
    
    analogWrite(ENB, 0);
    analogWrite(ENA, 0);
    Serial.print("DONE: ");
    Serial.println(abs(en2_position));
}

void en1a_isr() {
    if (digitalRead(EN1A) == HIGH) {
        if (digitalRead(EN1B) == LOW) { en1_position --; }
        else { en1_position++; }
    }
    else {
        if (digitalRead(EN1B) == LOW) { en1_position ++; }
        else { en1_position--; }
    }
}

void en1b_isr() {
    if (digitalRead(EN1B) == HIGH) {
        if (digitalRead(EN1A) == LOW) { en1_position ++; }
        else { en1_position--; }
    }
    else {
        if (digitalRead(EN1A) == LOW) { en1_position --; }
        else { en1_position++; }
    }
}

void en2a_isr() {
    if (digitalRead(EN2A) == HIGH) {
        if (digitalRead(EN2B) == LOW) { en2_position --; }
        else { en2_position++; }
    }
    else {
        if (digitalRead(EN2B) == LOW) { en2_position ++; }
        else { en2_position--; }
    }
}

void en2b_isr() {
    if (digitalRead(EN2B) == HIGH) {
        if (digitalRead(EN2A) == LOW) { en2_position ++; }
        else { en2_position--; }
    }
    else {
        if (digitalRead(EN2A) == LOW) { en2_position --; }
        else { en2_position++; }
    }
}
