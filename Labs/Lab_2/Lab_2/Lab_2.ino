/* 
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Simplified Bump Switch Example - MODIFIED
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) driving forward
 * until it hits an object (ie a bump switch is triggered) then it stops.
 *
 * How to run:
 * 1) Push left button on Launchpad to start the demo
 * 2) Robot will drive forward until the bump switches are triggered
 * 3) Once switches are triggered the robot will halt.
 * 4) Push left button again to start demo
 *
 * Learn more about the classes, variables and functions used in this library by going to:
 * https://fcooper.github.io/Robot-Library/
 *
 * Learn more about the TI RSLK by going to http://www.ti.com/rslk
 *
 * created by Franklin Cooper Jr.  
 * Modified by Ann Majewicz Fey
 *
 * ==============================================================
 * Lab 2 Objectives: 
 * ==============================================================
 * In this lab, you will start with my starter code and fill in 
 * behavior for the different robot states. Follow the outline in 
 * the lab description for how to create your states. The following
 * code is not a great way to program due to IF/ELSE statements
 * your job is to clean it up with case statements, enum types and
 * possibly state transition matrix. You will also need to implement 
 * code to acutally respond to the potential obstacles (e.g., back up 
 * and turn right for obstacles on the left). Challenge opportunities
 * for this lab are to program additional states associated with each 
 * bump sensor (the code below assigns robot states for adjacent pairs 
 * of sensors), and to implement some type of counter system to trigger 
 * a "I give up" state for the robot to just stop avoiding obstables if 
 * the navigation field is impassable. 
 */

#include "SimpleRSLK.h"
// NOTE: PIN DEFITIONS CAN BE FOUND IN RSLK_Pins.h

int motorSpeed = 30;
// Temp definitions for each sensor, you'll implement a better method
int FR = 0;
int R = 0;
int MR = 0;
int ML = 0;
int L = 0;
int FL = 0;

/*=== Example of what to try */
/*enum type for State Definition (note, the code below doesn't use this yet
You are not required to use the exact states below, just itended to be a guide/suggestion*/
enum states {
  KEEP_DRIVING,
  AVOID_HEADON,
  AVOID_LEFT,
  AVOID_RIGHT,
  STOP
  };

  states curr_state; /*Global variable to store current state*/


void setup() {
	Serial.begin(115200);

  setupRSLK();
  /* Left button on Launchpad */
  setupWaitBtn(LP_LEFT_BTN);
  /* Red led in rgb led */
  setupLed(RED_LED);

  /* Set up the pushbutton pins as inputs so we can read them*/
  /* NOTE: setupRSLK() already does this but I'm including the code here 
   *  for clarity about what is happening "under the hood" (meaning the 
   *  pinMode definitions below could be deleted). 
   *  The button states are encoded in the TOTAL_BP_SW array.  
   *  You can check all buttons using the following code: 
   *  ---------------------------------------------------
   *      for(int x = 0;x<TOTAL_BP_SW;x++){ 
   *       if(isBumpSwitchPressed(x) == true) 
   *        {DO STUFF
   *        }
   *  ---------------------------------------------------
   *  Dig into the SimpleRSLK.cpp, SimpleRSLK.h, and RSLK_Pins.h code to 
   *  improve the code below. It will be in a folder: Energia-RSLK-Library/src  
  */
	pinMode(BP_SW_PIN_0,INPUT_PULLUP);
	pinMode(BP_SW_PIN_1,INPUT_PULLUP);
	pinMode(BP_SW_PIN_2,INPUT_PULLUP);
	pinMode(BP_SW_PIN_3,INPUT_PULLUP);
	pinMode(BP_SW_PIN_4,INPUT_PULLUP);
	pinMode(BP_SW_PIN_5,INPUT_PULLUP);

  /* NOTE: this also isn't being used, just to give an idea of what you can do*/
  curr_state = KEEP_DRIVING; // Initalize the current state to the default, keep driving.
}

void avoid_headon() {
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_BACKWARD);
  delay(200);
  setMotorDirection(RIGHT_MOTOR,MOTOR_DIR_FORWARD);
  delay(600);
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_FORWARD);

  curr_state = KEEP_DRIVING;
}

void avoid_left() {
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_BACKWARD);
  delay(200);
  setMotorDirection(LEFT_MOTOR,MOTOR_DIR_FORWARD);
  delay(300);
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_FORWARD);

  curr_state = KEEP_DRIVING;
}

void avoid_right() {
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_BACKWARD);
  delay(200);
  setMotorDirection(RIGHT_MOTOR,MOTOR_DIR_FORWARD);
  delay(300);
  setMotorDirection(BOTH_MOTORS,MOTOR_DIR_FORWARD);

  curr_state = KEEP_DRIVING;
}

void stop() {
  setMotorSpeed(BOTH_MOTORS,0);
}

void loop() {
	String btnMsg = "Push left button on Launchpad to start demo.\n";
	/* Wait until button is pressed to start robot */
	waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);
  /* Wait two seconds before starting to allow putting robot in a safe place to move*/
	delay(2000);
  //TIP: set your robot on the pink sidekick box if you don't want it running off your desk!

	while(true) {

    /* READ ALL PINS (note, this way is messier)*/
    FR = digitalRead(BP_SW_PIN_0);
    R = digitalRead(BP_SW_PIN_1);
    MR = digitalRead(BP_SW_PIN_2);
    ML = digitalRead(BP_SW_PIN_3);
    L = digitalRead(BP_SW_PIN_4);
    FL = digitalRead(BP_SW_PIN_5);

    /*PRINT PIN STATE FOR DEBUGGING PURPOSES (note, you can do better :)*/
    Serial.print("FR: "); 
    Serial.print(FR);
    Serial.print(" R: ");
    Serial.print(R);
    Serial.print(" MR: ");
    Serial.print(MR);
    Serial.print(" ML: ");
    Serial.print(ML);
    Serial.print(" L: ");
    Serial.print(L);
    Serial.print(" FL: ");
    Serial.println(FL);
    
    /*CHANGE THIS CODE TO USE CASE STATEMENTS AND ACTUALLY IMPLEMENT A BASIC AVOIDANCE CODE */
    /*TIP: For a cleaner loop () use a simple switch/case on states and program the functionality of 
            states and transitions into other states in separate state functions. Check out the code here: 
            https://github.com/bminch/PIE/blob/main/FSM1.ino
            and a video explanation of the code here: https://www.youtube.com/watch?v=TRgUtG_C3FI */
      enableMotor(BOTH_MOTORS);
      setMotorDirection(BOTH_MOTORS,MOTOR_DIR_FORWARD);
      setMotorSpeed(BOTH_MOTORS,motorSpeed); // NOTE: RIGHT_MOTOR and LEFT_MOTOR are also defined in SimpleRSLK.h
      if(curr_state = KEEP_DRIVING) {
        Serial.print("DRIVING!"); 
        
      }

      if(MR==0 || ML==0) {
        Serial.print("HEAD ON COLLISION!");
        //IN HEAD ON STATE
        curr_state = AVOID_HEADON;
      }
      else if(R==0 || FR==0) {
        Serial.print("AVOID OBSTACLE TO RIGHT!");
        //IN OBS RIGHT STATE
        curr_state = AVOID_RIGHT;
      }
      else if(L==0 || FL==0 ) {
        Serial.print("AVOID OBSTACLE TO LEFT!");
        //IN OBS LEFT STATE
        curr_state = AVOID_LEFT;
      }
      else if(MR==0 && ML==0 && R==0 && FR==0 && L==0 && FL==0) {
        Serial.print("STUCK!");
        //IN STUCK STATE
        curr_state = STOP;
      }
      else {
        Serial.print("DRIVING FORWARD");
        //IN KEEP DRIVING STATE
        curr_state = KEEP_DRIVING;
      }

      switch (curr_state) {
        case AVOID_HEADON:
          avoid_headon();
          break;
        case AVOID_RIGHT:
          avoid_right();
          break;
        case AVOID_LEFT:
          avoid_left();
          break;
        case STOP:
          stop();
          break;
      }  
	} // END WHILE TRUE
 Serial.println("Collision detected");
 disableMotor(BOTH_MOTORS);
 
    
} // END LOOP
