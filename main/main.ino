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
  float distance[360];
  for (int i = 0; i < 360; i++){
    distance[i] = measure_distance();
    move_turn(1);
    }
  float a = 0;
  int angle = 0;
  for (int i = 0; i < 360; i++){
    if (a >= distance[i]){
      a = distance[i];
      angle = i;
        }
    }
  move_turn(angle);
  move_turn(-180);
}
