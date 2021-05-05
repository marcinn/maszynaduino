#include "Arduino.h"
#include "mux.h"

Mux::Mux(int pinEnable, int pinData, int pinS0, int pinS1, int pinS2, int pinS3) {
    this->pinEnable = pinEnable;
    this->pinData = pinData;
    this->pinS0 = pinS0;
    this->pinS1 = pinS1;
    this->pinS2 = pinS2;
    this->pinS3 = pinS3;

    this->setup();
}

void Mux::setup() {
    pinMode(this->pinS0, OUTPUT);
    pinMode(this->pinS1, OUTPUT);
    pinMode(this->pinS2, OUTPUT);
    pinMode(this->pinS3, OUTPUT);

    this->disable();
}

void Mux::setDataPinMode(int mode) {
    pinMode(this->pinData, mode);
}

void Mux::enable() {
    digitalWrite(this->pinEnable, LOW);
}

void Mux::disable() {
    digitalWrite(this->pinEnable, HIGH);
}

bool Mux::readDigital() {
    return digitalRead(this->pinData);
}

int Mux::readAnalog() {
    return analogRead(this->pinData);
}

void Mux::channel(int ch) {
    digitalWrite(this->pinS0, ch & 1);
    digitalWrite(this->pinS1, ch & 2);
    digitalWrite(this->pinS2, ch & 4);
    digitalWrite(this->pinS3, ch & 8);
    delayMicroseconds(1);
}
