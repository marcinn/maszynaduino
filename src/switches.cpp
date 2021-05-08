#include "Arduino.h"
#include "switches.h"
#include "console.h"

void Switch::update() {
    bool value = this->probe();
    value = (this->invert ? !value : value);
    value = (this->mode == INPUT_PULLUP ? !value : value);
    this->state = value;
}

void Switch::respond(Console *console) {
    console->setOutputSwitch((this->frame << 3) + this->bitNum, this->state);
}

bool Switch::getState() {
    return this->state;
}

bool Switch::isOn() {
    return this->state;
}

bool Switch::isOff() {
    return !this->state;
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
    pinMode(this->pin, INPUT);
    digitalWrite(this->pin, mode);
    this->update();
}

bool PinSwitch::probe() {
    return digitalRead(this->pin);
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
    this->update();
}

bool MuxSwitch::probe() {
    /*
    this->mux->setDataPinMode(this->mode);
    this->mux->channel(this->channel);
    this->mux->enable();
    */
    return this->mux->getState() & (1<<this->channel);
};
