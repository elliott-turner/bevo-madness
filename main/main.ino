#include <SimpleRSLK.h>
#include <stdio.h>

#include "ultrasonic.h"
#include "ir.h"
#include "motion.h"
#include "turret.h"

void setup()
{
    Serial.begin(115200);
    setupRSLK();
    setupWaitBtn(LP_LEFT_BTN);

    ultrasonic_start();
    ir_start();
    motion_start();
    turret_start();
}

void loop()
{
}
