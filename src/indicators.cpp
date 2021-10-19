#include "Arduino.h"
#include "indicators.h"
#include "comm.h"

Indicator::Indicator(IOutput *output, int pin, Alert indicatorNumber) {
    this->output = output;
    this->pin = pin;
    this->alert = indicatorNumber;
}

void Indicator::setup() {
    output->setup(pin);
}

bool Indicator::readState(MaszynaState *state) {
    return state->getIndicatorState(alert);
}

uint8_t Indicator::getAlertNumber() {
    return static_cast<int>(alert);
}

bool Indicator::getState() {
    return state;
}

void Indicator::update(MaszynaState *state) {
    this->state = this->readState(state);
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
