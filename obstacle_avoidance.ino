#include "NewPing.h"
 
#define SONAR_NUM 4
#define SPEED_FORWARD 200
#define SPEED_BACKWARD 150
#define SPEED_TURN 180
#define MAX_DISTANCE 400
#define MIN_DISTANCE 30
#define MIN_DISTANCE_CRUCIAL 20


// NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
NewPing sonar[SONAR_NUM] = {
  NewPing(2, 6, MAX_DISTANCE),    //front
  NewPing(10, 11, MAX_DISTANCE),  //left
  NewPing(12, 13, MAX_DISTANCE),  //right
  NewPing(A0, A1, MAX_DISTANCE)   //back
};

int distances[SONAR_NUM] = {-1, -1, -1, -1};
uint8_t enA = 9;
uint8_t inA1 = 8;
uint8_t inA2 = 7;
uint8_t enB = 3;
uint8_t inB1 = 4;
uint8_t inB2 = 5;
// right motor: enA, inA1, inA2
// left motor: enB, inB1, inB2
 
void getDistance(uint8_t sonar_num)
{
  delay(50);
  int distance = int((sonar[sonar_num].ping_median() * 0.0343)/2.0);
  /*NewPing libray returns 0 if nothing is in front of ultrasound sensor. 
  Set to 500 to avoid confusion while avoiding obstacles.*/
  if(distance == 0) 
  {
    distance=500;
  }
  Serial.print("Sonar = ");
  Serial.print(sonar_num);
  Serial.print(" Distance (cm) = ");
  Serial.println(distance);
  distances[sonar_num] = distance;
}
 

void stopMotor()
{
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, LOW);
}


void move_forward(int delay_time)
{
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, HIGH);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);
  analogWrite(enA, SPEED_FORWARD);
  analogWrite(enB, SPEED_FORWARD);
  delay(delay_time);
}


void move_backward(int delay_time)
{
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, HIGH);
  digitalWrite(inB2, LOW);
  analogWrite(enA, SPEED_BACKWARD);
  analogWrite(enB, SPEED_BACKWARD);
  delay(delay_time);
}


void turn_right(int delay_time)
{
  digitalWrite(inA1, HIGH);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);
  analogWrite(enA, SPEED_TURN);
  analogWrite(enB, SPEED_TURN);
  delay(delay_time);
  
}


void turn_left(int delay_time)
{
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, HIGH);
  digitalWrite(inB1, HIGH);
  digitalWrite(inB2, LOW);
  analogWrite(enA, SPEED_TURN);
  analogWrite(enB, SPEED_TURN);
  delay(delay_time);
}


// Support fuunction to check motor functionality 
void motor_check()
{
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB1, LOW);
  digitalWrite(inB2, HIGH);
  analogWrite(enA, 0);
  analogWrite(enB, 255);
}
 
 
void setup()
{
  pinMode(enA, OUTPUT);
  pinMode(inA1, OUTPUT);
  pinMode(inA2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(inB1, OUTPUT);
  pinMode(inB2, OUTPUT);
 
  stopMotor();
  Serial.begin(9600);  
  delay(2000);
}
 
 
void loop()
{
  stopMotor();
  for(uint8_t i=0;i<SONAR_NUM;i++)
  {
    getDistance(i); //get distances for ultrasound sensor
  }
  // object is very close to either of front, left or right ultrasound sensors
  if (((distances[0] < MIN_DISTANCE_CRUCIAL) || (distances[1] < MIN_DISTANCE_CRUCIAL)) || (distances[2] < MIN_DISTANCE_CRUCIAL))
  {
    if (distances[3] > MIN_DISTANCE_CRUCIAL)
    {
      move_backward(500);
     if ((distances[1] > MIN_DISTANCE_CRUCIAL) && (distances[1] > distances[2]))
      {
        turn_left(150);
      }
      else if((distances[2] > MIN_DISTANCE_CRUCIAL) && (distances[2] >= distances[1]))
      {
        turn_right(150);
      }
    }
    else
    {
      // Extend for alarm sound 
      Serial.println("Stuck! Please help");
      delay(2000);
    }
  }
  else if (distances[0] > MIN_DISTANCE) //no object close to front sensor
  {
    move_forward(500);
  }
  else
  {
    if ((distances[1] > MIN_DISTANCE) && (distances[1] > distances[2])) //no object close to left sensor
    {
      move_backward(300);
      turn_left(500);
      move_forward(650);
      turn_right(450);
    }
    else if((distances[2] > MIN_DISTANCE) && (distances[2] >= distances[1])) //no object close to right sensor
    {
      move_backward(300);
      turn_right(500);
      move_forward(650);
      turn_left(450);
    }
    else
    {
      if (distances[3] > MIN_DISTANCE) //space for robot to move backward
      {
        move_backward(500);
      }
      if (random(0,1) == 0)
      {
        turn_left(500);
        // Extend for alarm sound 
        Serial.println("Stuck! Please help");
        delay(1000);
      }
      else
      {
        turn_right(500);
        // Extend for alarm sound 
        Serial.println("Stuck! Please help");
        delay(1000);
      }
    }
  }
}