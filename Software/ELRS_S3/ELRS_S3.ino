#include <ESP32Servo.h>

#include <M5Unified.h>;
#include <Arduino.h>
#include "Arduino-CRSF.h"

Servo cugo1;
Servo cugo2;

int minUs = 890;   //PWM最小値
int maxUs = 2100;  //PWM最大値

int cugo1Pin = 7;
int cugo2Pin = 8;

int pos[2] = { 1500, 1500 };
ESP32PWM pwm;

CRSF crsf;

// ATOM Lite
// #define SRX 26
// #define STX 32
// #define LC 23
// #define RC 33

//ATOM S3
#define SRX 2
#define STX 1

int ch = 15;
int chData[16];

int oldCircleX[2];
int oldCircleY[2];
int nowCircleX[2];
int nowCircleY[2];

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);
  Serial1.begin(420000, SERIAL_8N1, SRX, STX);
  crsf.begin(&Serial1, 420000);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);

  cugo1.setPeriodHertz(50);  // Standard 50hz servo
  cugo2.setPeriodHertz(50);  // Standard 50hz servo

  cugo1.attach(cugo1Pin, minUs, maxUs);
  cugo2.attach(cugo2Pin, minUs, maxUs);

  cugo1.write(pos[0]);
  cugo2.write(pos[1]);

  crsf.onDataReceived([](const uint16_t channels[]) {
    char buffer[200];
    sprintf(buffer, "CH1: %d\tCH2: %d\tCH3: %d\tCH4: %d\tCH5: %d\tCH6: %d\tCH7: %d\tCH8: %d\tCH9: %d\tCH10: %d\tCH11: %d\tCH12: %d\tCH13: %d\tCH14: %d\tCH15: %d\tCH16: %d\n",
            channels[0],
            channels[1],
            channels[2],
            channels[3],
            channels[4],
            channels[5],
            channels[6],
            channels[7],
            channels[8],
            channels[9],
            channels[10],
            channels[11],
            channels[12],
            channels[13],
            channels[14],
            channels[15]);
    // Serial.print(buffer);
    for (int i = 0; i < ch; i++) {
      chData[i] = channels[i];
      // Serial.print(chData[i]);
    }
    // Serial.println("");
  });
}

void loop() {
  for (int i = 0; i < ch; i++) {
    int line = map(chData[i], 0, 2000, 0, 127);
    M5.Display.drawLine(0, 3 + (i * 3), line, 3 + (i * 3), YELLOW);
    M5.Display.drawLine(line + 1, 3 + (i * 3), 129, 3 + (i * 3), BLACK);
  }

  nowCircleX[0] = map(chData[3], 180, 1810, 3, 60);
  nowCircleX[1] = map(chData[0], 180, 1810, 3, 60);
  nowCircleY[0] = map(chData[2], 180, 1810, 60, 3);
  nowCircleY[1] = map(chData[1], 180, 1810, 60, 3);
  pos[0] = map(chData[2],180,1810,minUs,maxUs);
  pos[1] = map(chData[1],180,1810,minUs,maxUs);

  for (int i = 0; i < 2; i++) {
    if (nowCircleX[i] != oldCircleX[i] | nowCircleY[i] != oldCircleY[i]) {
      M5.Display.fillCircle(oldCircleX[i] + (i * 63), oldCircleY[i] + 64, 2, BLACK);
      M5.Display.fillCircle(nowCircleX[i] + (i * 63), nowCircleY[i] + 64, 2, GREEN);
      oldCircleX[i] = nowCircleX[i];
      oldCircleY[i] = nowCircleY[i];
      if(chData[2] < 50 | chData[2] > 2000) pos[0] = 1500;
      if(chData[1] < 50 | chData[2] > 2000) pos[1] = 1500;
      cugo1.write(pos[0]);
      cugo2.write(pos[1]);
    }
  }
}
void serialEvent1() {
  crsf.readPacket();
}