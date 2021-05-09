#include "Arduino.h"
#include "indicators.h"
#include "comm.h"


bool Indicator::readState(MaszynaState *state) {
    return state->getIndicatorState(indicatorNumber);
}

void Indicator::update(MaszynaState *state) {
    this->state = this->readState(state);
}

PinIndicator::PinIndicator(int pin, int indicatorNumber) {
    this->indicatorNumber = indicatorNumber;
    this->pin = pin;
}

void PinIndicator::setup() {
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, LOW);
}

void PinIndicator::respond() {
    digitalWrite(pin, state ? HIGH : LOW);
}


MuxIndicator::MuxIndicator(Mux *mux, int channel, int indicatorNumber) {
    this->indicatorNumber = indicatorNumber;
    this->mux = mux;
    this->channel = channel;
}

void MuxIndicator::setup() {
}

void MuxIndicator::respond() {
    /*
    this->mux->writeDigital(this->state);
    this->mux->setDataPinMode(OUTPUT);
    this->mux->channel(this->channel);
    this->mux->enable();
    */
}
