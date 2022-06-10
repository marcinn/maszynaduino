#include "Arduino.h"
#include "outputs.h"
#include "PCF8574.h"

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
        this->expander->begin();
        this->expander->write8(0);
        initialized = true;
    }

    this->expander->write(pin, LOW);
}

ArduinoOutput::ArduinoOutput(ActiveStateMode activeStateMode) {
    this->_high = activeStateMode == ActiveStateMode::ACTIVE_HIGH ? HIGH : LOW;
    this->_low = activeStateMode == ActiveStateMode::ACTIVE_HIGH ? LOW : HIGH;
}

void ArduinoOutput::write(int pin, int value) {
    digitalWrite(pin, value ? _high : _low);
}

void ArduinoOutput::setup(int pin) {
    pinMode(pin, OUTPUT);
    write(pin, _low);
}

void ArduinoAnalogOutput::write(int pin, int value) {
    analogWrite(pin, value);
};
