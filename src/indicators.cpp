#include "Arduino.h"
#include "indicators.h"
#include "comm.h"

Indicator::Indicator(IOutput *output, int pin, Alert indicatorNumber) {
    this->output = output;
    this->pin = pin;
    this->alert = indicatorNumber;
    this->alertNumber = static_cast<uint8_t>(alert);
}

Indicator::Indicator(IOutput *output, int pin, uint8_t byteNumber, uint8_t bitNumber) {
    this->output = output;
    this->pin = pin;
    this->alertNumber = (byteNumber * 8) + bitNumber;
}

void Indicator::setup() {
    output->setup(pin);
}

bool Indicator::readState(MaszynaState *state) {
    return state->getIndicatorState(alertNumber);
}

uint8_t Indicator::getAlertNumber() {
    return alertNumber;
}

bool Indicator::getState() {
    return state;
}

bool Indicator::update(MaszynaState *state) {
    bool newState = this->readState(state);
    bool changed = this->state != newState;
    this->state = newState;
    return changed;
}

void Indicator::respond() {
    output->write(pin, state ? HIGH : LOW);
}

void Indicator::reset() {
    state = LOW;
}

Alert Indicator::getAlert() {
    return alert;
}
