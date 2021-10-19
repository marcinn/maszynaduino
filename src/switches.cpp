#include "Arduino.h"
#include "switches.h"
#include "console.h"

void Switch::update() {
    bool value = probe();
    bool changed = false;
    value = (invert ? !value : value);
    value = (mode == INPUT_PULLUP ? !value : value);
    changed = !(state == value);
    this->state = value;
    if(changed) {
        onChange();
    }
}

void Switch::onChange() {}

int Switch::getOutputNumber() {
    return outputNum;
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

/* Pin Switch */

PinSwitch::PinSwitch(int pin, int outputNum, int mode, SwitchMode invert) {
    this->pin = pin;
    this->outputNum = outputNum;
    this->mode = mode;
    this->invert = invert == SwitchMode::NORMAL ? false : true;
};

PinSwitch::PinSwitch(int pin, int outputNum, SwitchMode mode)
    : PinSwitch::PinSwitch(pin, outputNum, INPUT_PULLUP, mode) {
};

void PinSwitch::setup() {
    pinMode(pin, mode);
    digitalWrite(pin, mode == INPUT_PULLUP ? HIGH : LOW);
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
    this->invert = invert == SwitchMode::NORMAL ? false : true;
    this->mode = INPUT_PULLUP;
    mux->setChannelMode(channel, MuxChannelMode::pullup);
};


void MuxSwitch::setup() {
    update();
}

bool MuxSwitch::probe() {
    return mux->getChannelState(channel);
};
