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

#define BLACK_VAL 2000

uint16_t sensorVal[LS_NUM_SENSORS];
uint16_t sensorCalVal[LS_NUM_SENSORS];
uint16_t sensorMaxVal[LS_NUM_SENSORS];
uint16_t sensorMinVal[LS_NUM_SENSORS];    

void setup()
{
	Serial.begin(115200);

	setupRSLK();
	/* Left button on Launchpad */
	setupWaitBtn(LP_LEFT_BTN);
	/* Red led in rgb led */
	setupLed(RED_LED);
	clearMinMax(sensorMinVal,sensorMaxVal);
 
}

void floorCalibration() {
	/* Place Robot On Floor (no line) */
	delay(2000);
	String btnMsg = "Push left button on Launchpad to begin calibration.\n";
	btnMsg += "Make sure the robot is on the floor away from the line.\n";
	/* Wait until button is pressed to start robot */
	waitBtnPressed(LP_LEFT_BTN,btnMsg,RED_LED);

	delay(1000);

	Serial.println("Running calibration on floor");
	simpleCalibrate();
	Serial.println("Reading floor values complete");

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

float getLinePosition2(uint16_t * sensorVals)
{
  // 0.0-0.5 -> far left - center
  // 0.5-1.0 -> center - far right
  // 2.0 -> intersection

  float linePos = 0.0;

  // if all sensors are seeing black, return 0 to indicate bot is at an intersection
  bool atIntersection = true;
  for (int i=0; i<LS_NUM_SENSORS; i++)
  {
    if (sensorVals[i] < BLACK_VAL) {
      atIntersection = false;
    }
  }
  if (atIntersection)
  {
    return 2.0;
  }

  int maxIndex = 0;
  uint16_t maxVal = 0;
  for (int i=0; i<LS_NUM_SENSORS; i++)
  {
    if (sensorVals[i] > maxVal)
    {
      maxVal = sensorVals[i];
      maxIndex = i;
    }
  }
  return (float)(maxIndex)/7.0;
}

bool isCalibrationComplete = false;
void loop()
{
	uint16_t normalSpeed = 17;
	uint16_t fastSpeed = 24;

	/* Valid values are either:
	 *  DARK_LINE  if your floor is lighter than your line
	 *  LIGHT_LINE if your floor is darker than your line
	 */
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

//	uint32_t linePos = getLinePosition(sensorCalVal,lineColor);

  
  uint16_t rightSpeed;
  uint16_t leftSpeed;
  float linePos = getLinePosition2(sensorVal);

  Serial.println(linePos);

  if (linePos == 2.0) {
    Serial.println("reached intersection");
    delay(5000);
  }

  leftSpeed = (uint16_t)((float)(fastSpeed-normalSpeed)*linePos) + normalSpeed;
  rightSpeed = fastSpeed - leftSpeed + normalSpeed;

  setMotorSpeed(LEFT_MOTOR,leftSpeed);
  setMotorSpeed(RIGHT_MOTOR,rightSpeed);
  Serial.println(leftSpeed);
  Serial.println(rightSpeed);


  /*Uncomment the Serial Print code block below to observe the analog values for each of 
   * the 8 individual embedded IR sensors on the robot. You cann index into the sensor array
   * this way.
   */
  
    /*
    Serial.println(sensorVal[0]); //the left-most sensor if facing same direction as robot
    Serial.println(sensorVal[1]);
    Serial.println(sensorVal[2]);
    Serial.println(sensorVal[3]);
    Serial.println(sensorVal[4]); 
    Serial.println(sensorVal[5]);
    Serial.println(sensorVal[6]);
    Serial.println(sensorVal[7]); //the right-most sensor if facing same direction as robot
    Serial.println("---------------"); 
    */

  /*
   * Uncomment the Serial line below to read linePos which is an estimate of the lateral position of the black line.
   */
   
   //Serial.println(linePos);


  /*
   * Uncomment the Line follow code block below for a simple line following mechanism.
   */

	/*if(linePos > 0 && linePos < 3000) {
		setMotorSpeed(LEFT_MOTOR,normalSpeed);
		setMotorSpeed(RIGHT_MOTOR,fastSpeed);
	} else if(linePos > 3500) {
		setMotorSpeed(LEFT_MOTOR,fastSpeed);
		setMotorSpeed(RIGHT_MOTOR,normalSpeed);
	} else {
		setMotorSpeed(LEFT_MOTOR,normalSpeed);
		setMotorSpeed(RIGHT_MOTOR,normalSpeed);
	}*/


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
   

    
}
