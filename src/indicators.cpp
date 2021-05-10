#include "Arduino.h"
#include "indicators.h"
#include "comm.h"
#include "debugmonitor.h"

bool Indicator::readState(MaszynaState *state) {
    return state->getIndicatorState(indicatorNumber);
}

void Indicator::debugMonitor(DebugMonitor *d) {
    d->print(AlertNames[indicatorNumber]);
    if(state) {
        d->print(" [*]");
    } else {
        d->print(" [ ]");
    }
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
    mux->setChannelMode(channel, MuxChannelMode::output);
}

void MuxIndicator::setup() {
}

void MuxIndicator::respond() {
    mux->requestChannelState(channel, state);
}
