#include "NAxisMotion.h"
#include <Wire.h>

const int greenLEDPin = 3;
int greenStatus = 0;
const int redLEDPin = 5;
int redStatus = 0;
const int buzzerPin = A0;
const int buttonPin = 2;
const int TXPin = 1;
const int RXPin = 0;
const int pingPinA = 9;
const int pingPinB = 11;
const int pingPinC = 13;
const int pingPinD = 7;
const int dataPinA = 8;
const int dataPinB = 10;
const int dataPinC = 12;
const int dataPinD = 6;

unsigned long lastRed = 0;
unsigned long lastGreen = 0;
unsigned long lastButtonClick = 0;
unsigned long lastBuzz = 0;
unsigned long lastAccell = 0;
unsigned long lastDist = 0;
const int blinkPeriod = 750;
const int switchPeriod = 1000;
const int buzzPeriod = 500;
const int accellPeriod = 100;
const int distPeriod = 100;

const float distTolerance = 10;
const float accellTolerance = 0.30;
const float gyroTolerance = 0.50;
const float maxScanDist = 50;

float accellX[10];
float accellY[10];
float accellZ[10];
int accellIterator = 0;//also used for gyro

float gyroX[10];
float gyroY[10];
float gyroZ[10];

float accellLockX = 0.0;
float accellLockY = 0.0;
float accellLockZ = 0.0;

float gyroLockX = 0.0;
float gyroLockY = 0.0;
float gyroLockZ = 0.0;

float distA[10];
float distB[10];
float distC[10];
float distD[10];
int distIterator = 0;

float distLockA = 0.0;
float distLockB = 0.0;
float distLockC = 0.0;
float distLockD = 0.0;

int mode = 0;
NAxisMotion mySensor;

void setup() 
{
  Serial.begin(115200);           //Initialize the Serial Port to view information on the Serial Monitor
  I2C.begin();                    //Initialize I2C communication to the let the library communicate with the sensor. 
  //Pin setup
  pinMode(buttonPin, INPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(TXPin, OUTPUT);
  pinMode(RXPin, INPUT);
  pinMode(pingPinA, OUTPUT);
  pinMode(pingPinB, OUTPUT);
  pinMode(pingPinC, OUTPUT);
  pinMode(pingPinD, OUTPUT);
  pinMode(dataPinA, INPUT);
  pinMode(dataPinB, INPUT);
  pinMode(dataPinC, INPUT);
  pinMode(dataPinD, INPUT);
  //End Setup
  //PinMode Setup
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);
  digitalWrite(TXPin, LOW);
  digitalWrite(dataPinA, LOW);
  digitalWrite(dataPinB, LOW);
  digitalWrite(dataPinC, LOW);
  digitalWrite(dataPinD, LOW);
  //End Setup
  //Init Accelerometer Shield
  //Sensor Initialization
  mySensor.initSensor();
  mySensor.setOperationMode(OPERATION_MODE_ACCGYRO);
  mySensor.setUpdateMode(MANUAL);
  mySensor.updateAccelConfig();  
}

void loop() 
{
  switch(mode)
  {
    case 0://idle mode
      if(SwitchResponse() == HIGH)
      {//Start arming
        mode = 1;
        ResetLights();
        RedOn();//I want alternating lights
        GreenOff();
        accellIterator = 0;
        distIterator = 0;
        Serial.println("Entering Arming Mode");
      }
      else
      {//Still Idle
        BlinkGreen();
      }
      break;
      //END MODE 0 ======================================================================================
    case 1://arming mode
      if(SwitchResponse() == HIGH)
      {//Skip arm, go back to idle
        mode = 0;
        RedOff();
        GreenOff();
        Serial.println("Canceling Arming - Return to IDLE");
      }
      else
      { 
        BlinkRed();
        BlinkGreen();
        if(distIterator < 10)
        {
          CheckDistanceArming();
        }
        if(accellIterator < 10)
        {
          CheckAccellArming();
        }
        if(accellIterator >= 10 && distIterator >= 10)
        {//area check complete
          Serial.println("Verifying Values");
          bool valid = true;
          distLockA = distA[0];
          Serial.println(distA[0]);
          distLockB = distB[0];
          distLockC = distC[0];
          distLockD = distD[0];
          accellLockX = accellX[0];
          accellLockY = accellY[0];
          accellLockZ = accellZ[0];
          gyroLockX = gyroX[0];
          gyroLockY = gyroY[0];
          gyroLockZ = gyroZ[0];
          for(int i = 1; i < 10; i++)
          {
            //Distance ================================================================
            if(distA[i] > (distLockA + distTolerance) || (distA[i] < distLockA - distTolerance))
            {
              valid = false;
              Serial.println("Verification Failed DA");
              break;
            }
            else
            {
              distLockA = ((distLockA * ((float)i/((float)i+1))) + (distA[i] * (float)(1/((float)i+1))));
            }
            if(distB[i] > distLockB + distTolerance || distB[i] < distLockB - distTolerance)
            {
              valid = false;
            Serial.println("Verification Failed DB");
              break;
            }
            else
            {
              distLockB = ((distLockB * ((float)i/((float)i+1))) + (distB[i] * (float)(1/((float)i+1))));
            }
            if(distC[i] > distLockC + distTolerance || distC[i] < distLockC - distTolerance)
            {
              valid = false;
            Serial.println("Verification Failed DC");
              break;
            }
            else
            {
              distLockC = ((distLockC * ((float)i/((float)i+1))) + (distC[i] * (float)(1/((float)i+1))));
            }
            if(distD[i] > distLockD + distTolerance || distD[i] < distLockD - distTolerance)
            {
              valid = false;
            Serial.println("Verification Failed DD");
              break;
            }
            else
            {
              distLockD = ((distLockD * ((float)i/((float)i+1))) + (distD[i] * (float)(1/((float)i+1))));
            }
            //Accell and Gyro ==========================================================
            if(accellX[i] > accellLockX + accellTolerance || accellX[i] < accellLockX - accellTolerance)
            {
              valid = false;
            Serial.println("Verification Failed AX");
              break;
            }
            else
            {
              accellLockX = ((accellLockX * ((float)i/((float)i+1))) + (accellX[i] * (1/((float)i+1))));
            }
            if(accellY[i] > accellLockY + accellTolerance || accellY[i] < accellLockY - accellTolerance)
            {
              valid = false;
            Serial.println("Verification Failed AY");
              break;
            }
            else
            {
              accellLockY = ((accellLockY * ((float)i/((float)i+1))) + (accellY[i] * (1/((float)i+1))));
            }
            if(accellZ[i] > accellLockZ + accellTolerance || accellZ[i] < accellLockZ - accellTolerance)
            {
              valid = false;
            Serial.println("Verification Failed AZ");
              break;
            }
            else
            {
              accellLockZ = ((accellLockZ * ((float)i/((float)i+1))) + (accellZ[i] * (1/((float)i+1))));
            }
            //Gyro ==========
            if(gyroX[i] > gyroLockX + gyroTolerance || gyroX[i] < gyroLockX - gyroTolerance)
            {
              valid = false;
            Serial.println("Verification Failed GX");
              break;
            }
            else
            {
              gyroLockX = ((gyroLockX * ((float)i/((float)i+1))) + (gyroX[i] * (1/((float)i+1))));
            }
            if(gyroY[i] > gyroLockY + gyroTolerance || gyroY[i] < gyroLockY - gyroTolerance)
            {
              valid = false;
            Serial.println("Verification Failed GY");
              break;
            }
            else
            {
              gyroLockY = ((gyroLockY * ((float)i/((float)i+1))) + (gyroY[i] * (1/((float)i+1))));
            }
            if(gyroZ[i] > gyroLockZ + gyroTolerance || gyroZ[i] < gyroLockZ - gyroTolerance)
            {
              valid = false;
            Serial.println("Verification Failed GZ");
              break;
            }
            else
            {
              gyroLockZ = ((gyroLockZ * ((float)i/((float)i+1))) + (gyroZ[i] * (1/((float)i+1))));
            }
          }
          if(valid)
          {
            mode = 2;
            Serial.println("Verification Success");
            RedOn();
            GreenOff();
          }
          else
          {//something was invalid. Restart
            Serial.println("Verification Failed");
            accellIterator = 0;
            distIterator = 0;
          }
        }
      }
      break;
      //END MODE 1 ======================================================================================
    case 2://armed mode
      if(SwitchResponse() == HIGH)
      {
        mode = 0;
      }
      else if(CheckDistanceArmed() || CheckAccellArmed())
      {
        mode = 3;
      }
      break;
      //END MODE 2 ======================================================================================
    case 3://alarm activated
      if(SwitchResponse() == HIGH)
      {
        mode = 0;
        GreenOff();
        RedOff();
      }
      else
      {
        BuzzerPulse();
        BlinkRed();
      }
      break;
      //END MODE 3 ======================================================================================
  }
}

int SwitchResponse()
{
  if ((millis() - lastButtonClick) >= switchPeriod)
  {
    int switchVal = digitalRead(buttonPin);
    if(switchVal == HIGH)
      lastButtonClick = millis();
    return switchVal;
  }
  else 
    return LOW;
}

void BlinkRed()
{
  if ((millis() - lastRed) >= blinkPeriod)
  {//blinking for ready
    lastRed = millis();
    if(redStatus == 0)
    {
      redStatus = 1;
      digitalWrite(redLEDPin, HIGH);
    }
    else
    {
      redStatus = 0;
      digitalWrite(redLEDPin, LOW);
    }
  }
}

void RedOn()
{
  digitalWrite(redLEDPin, HIGH);
  redStatus = 1;
}

void RedOff()
{
  digitalWrite(redLEDPin, LOW);
  redStatus = 0;
}

void BlinkGreen()
{
  if ((millis() - lastGreen) >= blinkPeriod)
  {//blinking for ready
    lastGreen = millis();
    if(greenStatus == 0)
    {
      greenStatus = 1;
      digitalWrite(greenLEDPin, HIGH);
    }
    else
    {
      greenStatus = 0;
      digitalWrite(greenLEDPin, LOW);
    }
  }
}

void GreenOn()
{
  digitalWrite(greenLEDPin, HIGH);
  greenStatus = 1;
}

void GreenOff()
{
  digitalWrite(greenLEDPin, LOW);
  greenStatus = 0;
}

void BuzzerPulse()
{
  if ((millis() - lastBuzz) >= buzzPeriod)
  {
    tone(buzzerPin, 300, 250);
    tone(buzzerPin, 150, 250);
    lastBuzz = millis();
  }
}

void ResetLights()
{
  digitalWrite(greenLEDPin, LOW);
  digitalWrite(redLEDPin, LOW);
  lastGreen = 0;
  lastRed = 0;
}

void CheckDistanceArming()
{
  if ((millis() - lastDist) >= distPeriod)
  {
    long duration;
    float value;
    digitalWrite(pingPinA, LOW);
    digitalWrite(pingPinB, LOW);
    digitalWrite(pingPinC, LOW);
    digitalWrite(pingPinD, LOW);
    delayMicroseconds(2);
    
    digitalWrite(pingPinA, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinA, LOW);
    duration = pulseIn(dataPinA,HIGH);
    value = duration / 29.1 / 2 ;
    Serial.println(value);
    if(value > 50)
      distA[distIterator] = 50;
    else
      distA[distIterator] = value;
    Serial.println(distA[distIterator]);
  
    digitalWrite(pingPinB, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinB, LOW);
    duration = pulseIn(dataPinB,HIGH);
    value = duration / 29.1 / 2 ;
    if(value > 50)
      distB[distIterator] = 50;
    else
     3 distB[distIterator] = value;
        
    digitalWrite(pingPinC, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinC, LOW);
    duration = pulseIn(dataPinC,HIGH);
    value = duration / 29.1 / 2 ;
    if(value > 50)
      distC[distIterator] = 50;
    else
      distC[distIterator] = value;
  
    digitalWrite(pingPinD, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinD, LOW);
    duration = pulseIn(dataPinD,HIGH);
    value = duration / 29.1 / 2 ;
    if(value > 50)
      distD[distIterator] = 50;
    else
      distD[distIterator] = value;

    distIterator++;
  }
}

bool CheckDistanceArmed()
{
  if ((millis() - lastDist) >= distPeriod)
  {
    long duration, A, B, C, D;
    digitalWrite(pingPinA, LOW);
    digitalWrite(pingPinB, LOW);
    digitalWrite(pingPinC, LOW);
    digitalWrite(pingPinD, LOW);
    delayMicroseconds(2);
    
    digitalWrite(pingPinA, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinA, LOW);
    duration = pulseIn(dataPinA,HIGH);
    A = duration / 29.1 / 2 ;
    if(A > 50)
      A = 50;
    if(A > distLockA+distTolerance || A < distLockA-distTolerance)
    {
      Serial.println("Distance A Tripped!");
      Serial.println(A);
      Serial.println(distLockA);
      return true;
    }
    
    digitalWrite(pingPinB, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinB, LOW);
    duration = pulseIn(dataPinB,HIGH);
    B = duration / 29.1 / 2 ;
    if(B > 50)
      B = 50;
    if(B > distLockB+distTolerance || B < distLockB-distTolerance)
    {
      Serial.println("Distance B Tripped!");
      Serial.println(B);
      Serial.println(distLockB);
      return true;
    }
      
    digitalWrite(pingPinC, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinC, LOW);
    duration = pulseIn(dataPinC,HIGH);
    C = duration / 29.1 / 2 ;
    if(C > 50)
      C = 50;
    if(C > distLockC+distTolerance || C < distLockC-distTolerance)
    {
      Serial.println("Distance C Tripped!");
      Serial.println(C);
      Serial.println(distLockC);
      return true;
    }
      
    digitalWrite(pingPinD, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPinD, LOW);
    duration = pulseIn(dataPinD,HIGH);
    D = duration / 29.1 / 2 ;
    if(D > 50)
      D = 50;
    if(D > distLockD+distTolerance || D < distLockD-distTolerance)
    {
      Serial.println("Distance D Tripped!");
      Serial.println(D);
      Serial.println(distLockD);
      return true;
    }
  }
  return false;
}

void CheckAccellArming()
{
  if ((millis() - lastAccell) >= accellPeriod)
  {
    mySensor.updateAccel();        //Update the Accelerometer data
    mySensor.updateGyro();
    accellX[accellIterator] = mySensor.readAccelX();
    Serial.println(accellX[accellIterator]);
    accellY[accellIterator] = mySensor.readAccelY();
    accellZ[accellIterator] = mySensor.readAccelZ();
    gyroX[accellIterator] = mySensor.readGyroX();
    gyroY[accellIterator] = mySensor.readGyroY();
    gyroZ[accellIterator] = mySensor.readGyroZ();
    accellIterator++;
  }
}

bool CheckAccellArmed()
{
  if ((millis() - lastAccell) >= accellPeriod)
  {
    Serial.println("Reading Accell");
    float aX, aY, aZ, gX, gY, gZ;
    mySensor.updateAccel();        //Update the Accelerometer data
    mySensor.updateGyro();
    aX = mySensor.readAccelX();
    if(aX > accellLockX+accellTolerance || aX < accellLockX - accellTolerance)
    {
      Serial.println("Accell X Tripped!");
      return true;
    }
    aY = mySensor.readAccelY();
    if(aY > accellLockY+accellTolerance || aY < accellLockY - accellTolerance)
    {
      Serial.println("Accell Y Tripped!");
      return true;
    }
    aZ = mySensor.readAccelZ();
    if(aZ > accellLockZ+accellTolerance || aZ < accellLockZ - accellTolerance)
    {
      Serial.println("Accell Z Tripped!");
      return true;
    }
    gX = mySensor.readGyroX();
    if(gX > gyroLockX+gyroTolerance || gX < gyroLockX - gyroTolerance)
    {
      Serial.println("Gyro X Tripped!");
      return true;
    }
    gY = mySensor.readGyroY();
    if(gY > gyroLockY+gyroTolerance || gY < gyroLockY - gyroTolerance)
    {
      Serial.println("Gyro Y Tripped!");
      return true;
    }
    gZ = mySensor.readGyroZ();
    if(gZ > gyroLockZ+gyroTolerance || gZ < gyroLockZ - gyroTolerance)
    {
      Serial.println("Gyro Z Tripped!");
      return true;   
    }
  }
  return false;
}
