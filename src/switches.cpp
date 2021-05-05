#include "Arduino.h"
#include "switches.h"

void Switch::updateOutputs(byte outputs[]) {

    if ((this->mode == INPUT_PULLUP && this->state) || (this->mode == LOW && this->state)) {
        outputs[this->frame + 4] |= 1 << this->bitNum;
    }
    else {
        outputs[this->frame + 4] &= ~(1 << this->bitNum);
    }
};


void Switch::update() {
    bool value = this->probe();
    this->state = this->invert == (SwitchMode::INVERT ? value : !value);
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

PinSwitch::PinSwitch(int pin, int frame, int bitNum) : PinSwitch::PinSwitch(pin, frame, bitNum, INPUT_PULLUP, SwitchMode::NORMAL) {
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

void MuxSwitch::setup() {
    this->mux->setDataPinMode(this->mode);
    this->update();
}

bool MuxSwitch::probe() {
    this->mux->enable();
    this->mux->channel(this->channel);
    delayMicroseconds(1);
    return this->mux->readDigital();
};
