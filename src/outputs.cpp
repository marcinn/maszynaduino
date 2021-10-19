#include "Arduino.h"
#include "outputs.h"
#include "PCF8574.h"
#include "Wire.h"

PCF8574Output::PCF8574Output(int address) {
    this->expander = new PCF8574(address);
    initialized = false;

}

void PCF8574Output::write(int pin, int value) {
    // TODO: change to modify internal state, add send() to send fast at
    // once
    this->expander->write(pin, value);
    delay(5);
}

void PCF8574Output::setup(int pin) {
    if(!initialized) {
        Wire.setClock(400000);
        this->expander->begin();
        this->expander->write8(0);
        initialized = true;
    }

    this->expander->write(pin, LOW);
}

void ArduinoOutput::write(int pin, int value) {
    digitalWrite(pin, value);
}

void ArduinoOutput::setup(int pin) {
    pinMode(pin, OUTPUT);
    write(pin, LOW);
}

void ArduinoAnalogOutput::write(int pin, int value) {
    analogWrite(pin, value);
};
