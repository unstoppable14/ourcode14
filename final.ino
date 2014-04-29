#include <HUBeeBMDWheel.h>

HUBeeBMDWheel leftWheel;
HUBeeBMDWheel rightWheel;
unsigned long previousMillis = 0;
long interval = 88000;
int leftSpeed = 250, rightSpeed = 250;
volatile int count = 0;

int leftQeiAPin  = 2; //external interrupt 0
int leftQeiBPin  = 4;
int rightQeiAPin = 3; //external interrupt 1
int rightQeiBPin = 7;

volatile int leftQeiCounts = 0;
volatile int rightQeiCounts = 0;
int leftElapsedTime = 0, rightElapsedTime = 0;
int leftOldElapsedTime = 0, rightOldElapsedTime = 0;
int serialTimer = 0;
int travel = 0; 

void setup()
{
  pinMode(leftQeiAPin, INPUT_PULLUP);
  pinMode(rightQeiAPin, INPUT_PULLUP);
  pinMode(leftQeiBPin, INPUT_PULLUP);
  pinMode(rightQeiBPin, INPUT_PULLUP);
  leftWheel.setupPins(8, 11, 9); //setup using pins 12 and 2 for direction control, and 3 for PWM speed control
  rightWheel.setupPins(12, 13, 10); //setup using pins 13 and 4 for direction control, and 11 for PWM speed control
  leftWheel.setDirectionMode(0); //Set the direction mode to 1
  rightWheel.setDirectionMode(1); //set the direction mode to 1
  /*
  The counter for each wheel (leftQeiCounts and rightQeiCounts) will go up when the motor direction is set to 1 and motor power is positive
   It will go down when motor power is negative
   The leftElapsedTime and rightElapsedTime will give you an indication of speed - The bigger the number the lower the speed
   BE WARNED - The speed variables will give innacurate readings at low speed because the values get too big
   */
  attachInterrupt(0, QEI_LeftWheel, CHANGE);
  attachInterrupt(1, QEI_RightWheel, CHANGE);
  Serial.begin(9600);
  //start the wheels
}

void loop()
{
  //do this forever - spin the wheels and reverse direction when the count gets to + or - 64
  // There are 64 'ticks' per revolution
  unsigned long currentMillis = millis();
  if (currentMillis <interval) 

  {
    goforward(800);
    gobackward(400);
    goleft(80);
    goforward(300);

  }

}

void QEI_LeftWheel()
{
  //work out the elapsed time since the last interrupt
  leftElapsedTime = micros() - leftOldElapsedTime;
  leftOldElapsedTime = micros();
  //ChA has changed state
  //Check the state of ChA
  if (digitalRead(leftQeiAPin))
  {
    //pin has gone high
    //check chanel B
    if (digitalRead(leftQeiBPin))
    {
      //both are high
      leftQeiCounts++;
      return;
    }
    //ChB is low
    leftQeiCounts--;
    return;
  }
  //if you get here then ChA has gone low, check ChB
  if (digitalRead(leftQeiBPin))
  {
    //ChB is high
    leftQeiCounts--;
    return;
  }
  //if you get here then A has gone low and B is low
  leftQeiCounts++;
}

//Left Wheel pin interrupt function
void QEI_RightWheel()
{

  if (digitalRead(rightQeiAPin))
  {
    //pin has gone high
    //check chanel B
    if (digitalRead(rightQeiBPin))
    {
      //both are high
      rightQeiCounts--;
      return;
    }
    //ChB is low
    rightQeiCounts++;
    return;
  }

  if (digitalRead(rightQeiBPin))
  {
    rightQeiCounts++;
    return;
  }

  rightQeiCounts--;
}

void goforward (int distance_mm)
{
  int start_left_counts = leftQeiCounts;
  int start_right_counts = rightQeiCounts;

  leftWheel.setMotorPower(leftSpeed);
  rightWheel.setMotorPower(rightSpeed);

  int distance_ticks = (int) distance_mm * 64.0 / 188.5;

  while ((leftQeiCounts < (distance_ticks + start_left_counts)) && (rightQeiCounts < (distance_ticks + start_right_counts))) {

    int right_sensor= analogRead(A0); 

    if (A0 > 550){
      obstructionDetected();
      leftWheel.setMotorPower(leftSpeed);
      rightWheel.setMotorPower(-rightSpeed);
    }
  }
  leftWheel.stopMotor();
  rightWheel.stopMotor();
}

void gobackward(int distance_mm)
{
  int start_left_counts = leftQeiCounts;
  int start_right_counts = rightQeiCounts;

  leftWheel.setMotorPower(-leftSpeed);
  rightWheel.setMotorPower(-rightSpeed);

  int distance_ticks = (int) distance_mm * 64.0 / 188.5;

  while ((leftQeiCounts > (start_left_counts - distance_ticks)) && (rightQeiCounts > (start_right_counts - distance_ticks))) {

  }

  leftWheel.stopMotor();
  rightWheel.stopMotor();
}


void goright(int angle_degrees)
{
  int start_left_counts = leftQeiCounts;
  int start_right_counts = rightQeiCounts;

  leftWheel.setMotorPower(leftSpeed);
  rightWheel.setMotorPower(-rightSpeed);
  int distance_mm =  3.14 * 115 * angle_degrees / 360 ;

  int distance_ticks = (int) distance_mm * 64.0 / 188.5;
  while ((leftQeiCounts  > (start_left_counts - distance_ticks)) && (rightQeiCounts < (distance_ticks + start_right_counts))) {


  }

  leftWheel.stopMotor();
  rightWheel.stopMotor();
}

void goleft(int angle_degrees)
{
  int start_left_counts = leftQeiCounts;
  int start_right_counts = rightQeiCounts;

  leftWheel.setMotorPower(-leftSpeed);
  rightWheel.setMotorPower(rightSpeed);
  int distance_mm =  3.14 * 115 * angle_degrees / 360 ;

  int distance_ticks = (int) distance_mm * 64.0 / 188.5;
  while ((leftQeiCounts  < (start_left_counts + distance_ticks)) && (rightQeiCounts > (start_right_counts- distance_ticks  ))) {

  }
  leftWheel.stopMotor();
  rightWheel.stopMotor();
}
void obstructionDetected(){
  leftWheel.stopMotor();
  rightWheel.stopMotor();
  //wait for obstruction to move   
  while(analogRead(A0>140))
  {

  } 
}

