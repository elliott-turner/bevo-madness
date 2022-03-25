/*
 * Energia Robot Library for Texas Instruments' Robot System Learning Kit (RSLK)
 * Line Following Example
 *
 * Summary:
 * This example has the TI Robotic System Learning Kit (TI RSLK) follow a line
 * using a basic line following algorithm. This example works on a dark floor with
 * a white line or a light floor with a dark line. The robot first needs to be calibrated
 * Then place the robot on the hit the left button again to begin the line following.
 *
 * How to run:
 * 1) Push left button on Launchpad to have the robot perform calibration.
 * 2) Robot will drive forwards and backwards by a predefined distance.
 * 3) Place the robot center on the line you want it to follow.
 * 4) Push left button again to have the robot begin to follow the line.
 *
 * Parts Info:
 * o Black eletrical tape or white electrical tape. Masking tape does not work well
 *   with IR sensors.
 *
 * Learn more about the classes, variables and functions used in this library by going to:
 * https://fcooper.github.io/Robot-Library/
 *
 * Learn more about the TI RSLK by going to http://www.ti.com/rslk
 *
 * created by Franklin Cooper Jr.
 * Edited by Perry Scott
 *
 * This example code is in the public domain.
 */

#include "SimpleRSLK.h"
#include <stdio.h>

#define BLACK_VAL 2000.0
#define WHITE_VAL 750.0
#define SENSOR_RANGE 2500.0

typedef struct {
  uint8_t type; // 0 -> drive to next intersection, 1 -> turn val degrees (right positive), 2 -> stop
  int val;
} INST;

INST instructions[] = { // list of instructions to end facing right basket
  {0, 0},   // drive to next intersection
  {1, 0},   // turn 0 deg (go straight at intersection)
  {0, 0},   // drive to next intersection
  {1, 90},  // turn 90 deg (take right at intersection)
  {0, 0},   // drive to next intersection
  {1, -90}, // turn -90 deg (take left at intersection)
  {2, 0}    // stop
};
int numInstructions = 7; // instruction count

//INST instructions[] = {
//  {0, 0},
//  {1, 90},
//  {0, 0},
//  {1, 90},
//  {0, 0},
//  {1, 180},
//  {0, 0},
//  {1, -90},
//  {0, 0},
//  {1, -90},
//  {0, 0},
//  {1, 180},
//};
//int numInstructions = 12;


int instructionIndex = 0;

uint16_t normalSpeed = 15;
uint16_t fastSpeed = 30;

uint16_t sensorVal[LS_NUM_SENSORS];
uint16_t sensorCalVal[LS_NUM_SENSORS];
uint16_t sensorMaxVal[LS_NUM_SENSORS];
uint16_t sensorMinVal[LS_NUM_SENSORS];
float linePos;  

void setup()
{
	Serial.begin(115200);

	setupRSLK();
	/* Left button on Launchpad */
	setupWaitBtn(LP_LEFT_BTN);
	/* Red led in rgb led */
	setupLed(RED_LED);
	clearMinMax(sensorMinVal,sensorMaxVal);

  linePos = 0;
}

void floorCalibration() {
	/* Place Robot On Floor (no line) */
	delay(2000);
	String btnMsg = "Push left button on Launchpad to begin calibration.\n";
	btnMsg += "Make sure the robot is on the floor away from the line.\n";
	/* Wait until button is pressed to start robot */
	waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);

	delay(1000);

	simpleCalibrate();

	btnMsg = "Push left button on Launchpad to begin line following.\n";
	btnMsg += "Make sure the robot is on the line.\n";
	/* Wait until button is pressed to start robot */
	waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);
	delay(1000);

	enableMotor(BOTH_MOTORS);
}

void simpleCalibrate() {
	/* Set both motors direction forward */
	setMotorDirection(BOTH_MOTORS,MOTOR_DIR_FORWARD);
	/* Enable both motors */
	enableMotor(BOTH_MOTORS);
	/* Set both motors speed 20 */
	setMotorSpeed(BOTH_MOTORS,20);

	for(int x = 0;x<100;x++){
		readLineSensor(sensorVal);
		setSensorMinMax(sensorVal,sensorMinVal,sensorMaxVal);
	}

	/* Disable both motors */
	disableMotor(BOTH_MOTORS);
}

float getLinePosition3(uint16_t * sensorVals)
{
  // return line position (-1 to +1 with 0 being center)
  bool offLine = true;
  for (int i=0; i<8; i++) {
    if ((float)sensorVals[i] - WHITE_VAL > 0) {
      offLine = false;
      break;
    }
  }

  if (offLine) { return linePos; }
  
  float linePosition = 0;
  // use weighted sum to determine "weighted" line position
  linePosition += (((float)sensorVals[0]-WHITE_VAL)/SENSOR_RANGE) * -1.0;
  linePosition += (((float)sensorVals[1]-WHITE_VAL)/SENSOR_RANGE) * -0.7;
  linePosition += (((float)sensorVals[2]-WHITE_VAL)/SENSOR_RANGE) * -0.3;
  linePosition += (((float)sensorVals[3]-WHITE_VAL)/SENSOR_RANGE) * -0.1;
  linePosition += (((float)sensorVals[4]-WHITE_VAL)/SENSOR_RANGE) * 0.1;
  linePosition += (((float)sensorVals[5]-WHITE_VAL)/SENSOR_RANGE) * 0.3;
  linePosition += (((float)sensorVals[6]-WHITE_VAL)/SENSOR_RANGE) * 0.7;
  linePosition += (((float)sensorVals[7]-WHITE_VAL)/SENSOR_RANGE) * 1.0;

  if (linePosition*linePosition > 1) {
    if (linePosition > 0) { linePosition = 1; }
    else { linePosition = -1; }
  }

  return linePosition;
  
}

void turn(int deg)
{
  setMotorSpeed(BOTH_MOTORS,normalSpeed);
  delay(150);
  if (deg > 0) {
    setMotorDirection(RIGHT_MOTOR,MOTOR_DIR_BACKWARD);
    delay(deg*10);
    setMotorDirection(RIGHT_MOTOR,MOTOR_DIR_FORWARD);
  }
  if (deg < 0) {
    setMotorDirection(LEFT_MOTOR,MOTOR_DIR_BACKWARD);
    delay(deg*10*-1);
    setMotorDirection(LEFT_MOTOR,MOTOR_DIR_FORWARD);
  }
  delay(300);
}

bool isCalibrationComplete = false;
void loop()
{

	uint8_t lineColor = DARK_LINE;

	/* Run this setup only once */
	if(isCalibrationComplete == false) {
		floorCalibration();
		isCalibrationComplete = true;
	}

  //Get sensor array data and compute position
	readLineSensor(sensorVal);
	readCalLineSensor(sensorVal,
					  sensorCalVal,
					  sensorMinVal,
					  sensorMaxVal,
					  lineColor);
  
  uint16_t rightSpeed;
  uint16_t leftSpeed;
  uint16_t speedDelta;
  linePos = getLinePosition3(sensorVal);

  // set motor speeds to counteract any offset of line position from center
  speedDelta = (uint16_t)(((float)fastSpeed-(float)normalSpeed)/2.0*linePos + ((float)fastSpeed-(float)normalSpeed)/2.0);
  leftSpeed = normalSpeed + speedDelta;
  rightSpeed = fastSpeed - speedDelta;

  setMotorSpeed(LEFT_MOTOR,leftSpeed);
  setMotorSpeed(RIGHT_MOTOR,rightSpeed);


  /* For lab 3 you will need to develop a state machine which can accomplish the following task. 
   *  
   *      Your robot will need to navigate from the back wall of the 
   *      competition board (starting on the line facing forward) to right hash on the line closest to the basket. 
   *      A clear picture of this is shown in the Lab 3 Document. 
   *      
   *      Some things to keep in mind:
   *        -How will you know when an intersection has been reached?
   *        -Does linePos tell you all the information you need or will you need the analog data from individual sensors?
   *        -How do you deal with finding the rising edge of an intersection. Falling edge? 
   *        
   *      
   */


   //Develop your state machine here

  bool atIntersection = true;
  for (int i=0; i<8; i++)
  {
    if (sensorVal[i] < 2000) {
      atIntersection = false;
      break;
    }
  }

  Serial.print(instructionIndex);

  if (instructions[instructionIndex].type == 0 && instructionIndex != numInstructions) {
    if (atIntersection) { instructionIndex++; }
  }
  if (instructions[instructionIndex].type == 1 && instructionIndex != numInstructions) {
    turn(instructions[instructionIndex].val);
    instructionIndex++;
  }
  if (instructions[instructionIndex].type == 2 && instructionIndex != numInstructions) {
    disableMotor(BOTH_MOTORS);
    setup();
  }
  if (instructionIndex >= numInstructions)
  {
    instructionIndex = 0;
  }
    
}
