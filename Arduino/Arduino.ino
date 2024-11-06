#include <TimerOne.h>
#include "GiaoLo.h"
#include "Led.h"
#include "TrafficLight.h"

#define x1 8
#define v1 9
#define d1 10
#define x2 11
#define v2 12
#define d2 13

#define SCLK 5  // Shift Clock Pin
#define RCLK 4  // Latch Clock Pin
#define DIO 3   // Data Input Pin

bool stopMode = false;
bool yellowFlashing = false;
bool lightMode = true;
byte colorLed1;
byte colorLed2;

int t = 0;
int t1, t2;

int t_vang = 3;
int t_xanh = 5;
int t_do = 8;
byte mode;

GiaoLo giaoLo(SCLK, RCLK, DIO);
Led ledOne(t_xanh, t_do, t_vang);
Led ledTwo(t_xanh, t_do, t_vang);
TrafficLight trafficLightOne(x1, d1, v1);
TrafficLight trafficLightTwo(x2, d2, v2);

void setup() {
  Serial.begin(9600);

  // Initialize TimerOne to call updateTimers every 1 second
  Timer1.initialize(1000000);            // 1,000,000 microseconds = 1 second
  Timer1.attachInterrupt(updateTimers);  // Attach the timer interrupt
}

// Mode: 0.Light mode 1.Stop mode   2.Night mode
void loop() {
  if (Serial.available() >= 3) {
    mode = Serial.read();

    if (mode == 0) {
      byte flag = Serial.read();
      if (flag == 0) {
        byte t_xanh = Serial.read();
        byte t_do = Serial.read();

        ledOne.setXanh(t_xanh);
        ledOne.setDo(t_do);
        if (t_do <= 3) {
          ledOne.setVang(0);
        }
        ledTwo.caculator(ledOne);

      } else if (flag == 1) {
        ledTwo.setXanh(t_xanh);
        ledTwo.setDo(t_do);
        if (t_do <= 3) {
          ledTwo.setVang(0);
        }
        ledOne.caculator(ledTwo);
      }

      lightMode = true;
      stopMode = false;
      yellowFlashing = false;
      t = 0;

    } else if (mode == 1) {
      colorLed1 = Serial.read();
      colorLed2 = Serial.read();

      lightMode = false;
      stopMode = true;
      yellowFlashing = false;

      t = 0;

    } else if (mode == 2) {
      colorLed1 = Serial.read();
      colorLed2 = Serial.read();
      yellowFlashing = true;
      stopMode = false;
      lightMode = false;
      t = 0;
    }
  }

  if (stopMode) {
    giaoLo.turnOff7Segment();
    trafficLightOne.setColor(colorLed1);
    trafficLightTwo.setColor(colorLed2);
  }

  // Traffic light control logic based on current time `t`
  if (yellowFlashing) {
    giaoLo.turnOff7Segment();
    trafficLightOne.toggleYellowLights();
    trafficLightTwo.toggleYellowLights();
  }

  if (lightMode) {
    displayLight();
    // Send t1, t2
    Serial.print(t1);
    Serial.print(",");
    Serial.println(t2);

    giaoLo.displayNumber(t1, t2);
  }
}

void updateTimers() {
  t++;
  t1--;
  t2--;
}

void displayLight() {
  if (t == 0) {
    trafficLightOne.setColor(0);
    trafficLightTwo.setColor(2);
    t1 = ledOne.getXanh();
    t2 = ledTwo.getDo();
    Serial.println("Green: LED 1 - Red: LED 2");

  } else if (t == ledOne.getXanh() && ledOne.getVang() > 0) {
    trafficLightOne.setColor(1);
    trafficLightTwo.setColor(2);
    t1 = ledOne.getVang();
    Serial.println("Yellow: LED 1 - Red: LED 2");

  } else if (t == ledTwo.getDo()) {
    trafficLightOne.setColor(2);
    trafficLightTwo.setColor(0);
    t1 = ledOne.getDo();
    t2 = ledTwo.getXanh();
    Serial.println("Green: LED 2 - Red: LED 1");

  } else if (t == ledOne.getXanh() + ledOne.getDo() && ledOne.getVang() > 0) {
    trafficLightOne.setColor(2);
    trafficLightTwo.setColor(1);
    Serial.println("Yellow: LED 1 - Red: LED 2");

    t2 = ledOne.getVang();

  } else if (t == ledOne.getXanh() + ledOne.getDo() + ledOne.getVang()) {
    t = 0;
  }
}