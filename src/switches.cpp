#include "Arduino.h"
#include "switches.h"
#include "console.h"

void Switch::update() {
    bool value = probe();
    value = (invert ? !value : value);
    value = (mode == INPUT_PULLUP ? !value : value);
    this->state = value;
}

void Switch::respond(MaszynaState *state) {
    state->setOutputSwitch((frame << 3) + bitNum, state);
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

PinSwitch::PinSwitch(int pin, int frame, int bitNum, int mode, SwitchMode invert) {
    this->pin = pin;
    this->frame = frame;
    this->bitNum = bitNum;
    this->mode = mode;
    this->invert = invert;
};

PinSwitch::PinSwitch(int pin, int frame, int bitNum)
    : PinSwitch::PinSwitch(pin, frame, bitNum, INPUT_PULLUP, SwitchMode::NORMAL) {
};

PinSwitch::PinSwitch(int pin, int outputNumber, SwitchMode invert)
    : PinSwitch::PinSwitch(pin, outputNumber / 8, outputNumber % 8, INPUT_PULLUP, invert) {
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

MuxSwitch::MuxSwitch(Mux *mux, int channel, int frame, int bitNum, SwitchMode invert) {
    this->mux = mux;
    this->channel = channel;
    this->frame = frame;
    this->bitNum = bitNum;
    this->invert = invert;
    this->mode = INPUT_PULLUP;
};

MuxSwitch::MuxSwitch(Mux *mux, int channel, int frame, int bitNum) : MuxSwitch::MuxSwitch(mux, channel, frame, bitNum, SwitchMode::NORMAL) {
}

MuxSwitch::MuxSwitch(Mux *mux, int channel, int outputNumber, SwitchMode invert)
    : MuxSwitch::MuxSwitch(mux, channel, outputNumber / 8, outputNumber % 8, invert) { }

void MuxSwitch::setup() {
    update();
}

bool MuxSwitch::probe() {
    /*
    this->mux->setDataPinMode(this->mode);
    this->mux->channel(this->channel);
    this->mux->enable();
    */
    return mux->getState() & (1 << channel);
};
