#include "Arduino.h"
#include "console.h"


Console::Console(HardwareSerial *serial) {
    this->serial = serial;
}

Console::Console(HardwareSerial *serial, Switch **switches, int switchesCount) {
    this->serial = serial;

    for (int i = 0; i < switchesCount; i++) {
        this->addSwitch(switches[i]);
    }
}

Console::Console(HardwareSerial *serial, Switch **switches, int switchesCount, Indicator **indicators, int indicatorsCount) : Console(serial, switches, switchesCount) {
    for (int i=0; i<indicatorsCount; i++) {
        this->addIndicator(indicators[i]);
    }
}

void Console::setup() {
    this->serial->begin(57600, SERIAL_8N1);

    for (int i = 0; i < this->switchesCount; i++) {
        this->switches[i]->setup();
    }
    for (int i = 0; i < this->indicatorsCount; i++) {
        this->indicators[i]->setup();
    }
    while (!this->serial) {}
}
void Console::update() {
    for (int i = 0; i < this->switchesCount; i++) {
        this->switches[i]->update();
        this->switches[i]->respond(this);
    }
    for (int i = 0; i < this->indicatorsCount; i++) {
        this->indicators[i]->update(&this->input);
        this->indicators[i]->respond();
    }
}
void Console::transmit() {
    this->serial->readBytes((byte *) &this->input, sizeof(this->input));
    this->serial->write((byte *) &this->output, sizeof(this->output));
    this->serial->flush();
}
InputFrame* Console::getInputs() {
    return &this->input;
}
OutputFrame* Console::getOutputs() {
    return &this->output;
}

void Console::addSwitch(Switch *_switch) {
    this->switches[this->switchesCount] = _switch;
    this->switchesCount++;
}

void Console::addIndicator(Indicator *indicator) {
    this->indicators[this->indicatorsCount] = indicator;
    this->indicatorsCount++;
}

int Console::getSwitchesCount() {
    return this->switchesCount;
}

int Console::getIndicatorsCount() {
    return this->indicatorsCount;
}

void Console::setOutputSwitch(uint8_t num, bool state) {
    if(num > 47) {
        return;
    }

    uint8_t const byteNum = (num / 8);
    uint8_t const bitNum = num % 8;

    this->setOutputBit(byteNum, bitNum, state);
}

void Console::setOutputBit(uint8_t byteNum, uint8_t bitNum, bool state) {
    if(state) {
        ((uint8_t *) &this->output)[byteNum+4] |= 1 << bitNum;
    } else {
        ((uint8_t *) &this->output)[byteNum+4] &= ~(1 << bitNum);
    }
}
