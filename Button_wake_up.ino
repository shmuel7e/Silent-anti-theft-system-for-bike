#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <LowPower.h>
#include <Battery.h>
#include <Wire.h>
#include <Battery.h>
#include <avr/sleep.h>

int x=0;
void setup() {

  Serial.begin(9600);
  pinMode(2, INPUT);
  digitalWrite(2, HIGH); // set int0 = digital pin 2 HIGH

}

void loop() {
  x=0;
  Serial.println("Entering Sleep mode");
  delay(100);
 for (int i = 0; i < 10; i++) {
    attachInterrupt(0, pinInterrupt,FALLING);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    delay(100);
    sleep_enable();
    if (sleepflag==1)
    break;
    delay(100);
 }
 Serial.println("Done sleeping");
 delay(1000);
}

int pinInterrupt()

{ sleep_disable();

  detachInterrupt(0);
  x=1;
}
