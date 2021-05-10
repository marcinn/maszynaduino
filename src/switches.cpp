#include "Arduino.h"
#include "switches.h"
#include "console.h"
#include "debugmonitor.h"

void Switch::update() {
    bool value = probe();
    value = (invert ? !value : value);
    value = (mode == INPUT_PULLUP ? !value : value);
    this->state = value;
}

void Switch::respond(MaszynaState *state) {
    state->setOutputSwitch(outputNum, this->state);
}

bool Switch::getState() {
    return state;
}

bool Switch::isOn() {
    return state;
}

bool Switch::isOff() {
    return !state;
}

void Switch::debugMonitor(DebugMonitor *d) {
    if(state) {
        d->print("[*]");
    } else {
        d->print("[ ]");
    }
}

/* Pin Switch */

PinSwitch::PinSwitch(int pin, int outputNum, int mode, SwitchMode invert) {
    this->pin = pin;
    this->outputNum = outputNum;
    this->mode = mode;
    this->invert = invert;
};

PinSwitch::PinSwitch(int pin, int outputNum, SwitchMode mode)
    : PinSwitch::PinSwitch(pin, outputNum, INPUT_PULLUP, mode) {
};

void PinSwitch::setup() {
    pinMode(pin, INPUT);
    digitalWrite(pin, mode);
    update();
}

bool PinSwitch::probe() {
    return digitalRead(pin);
};


/* MuxSwitch */

MuxSwitch::MuxSwitch(Mux *mux, int channel, int outputNum, SwitchMode invert) {
    this->mux = mux;
    this->channel = channel;
    this->outputNum = outputNum;
    this->invert = invert;
    this->mode = INPUT_PULLUP;
    mux->setChannelMode(channel, MuxChannelMode::pullup);
};


void MuxSwitch::setup() {
    update();
}

bool MuxSwitch::probe() {
    return mux->getChannelState(channel);
};
