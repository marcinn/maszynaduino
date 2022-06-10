#include "Arduino.h"
#include "switches.h"
#include "console.h"

extern ArduinoDigitalInput *arduinoDigitalInputs;

Switch::Switch(IInput *input, int pin, int outputNum, InputMode mode, SwitchMode invert) {
    this->pin = pin;
    this->outputNum = outputNum;
    this->invert = invert == SwitchMode::NORMAL ? false : true;
    this->mode = mode;
    this->input = input;
}

void Switch::setup() {
    input->setup(pin, mode);
    update();
}

bool Switch::probe() {
    return input->read(pin);
};

void Switch::update() {
    bool value = probe();
    bool changed = false;
    value = (invert ? !value : value);
    value = (mode == InputMode::PULLUP ? !value : value);
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

PinSwitch::PinSwitch(int pin, int outputNum, int mode, SwitchMode invert) : Switch(arduinoDigitalInputs, pin, outputNum, mode == INPUT_PULLUP ? InputMode::PULLUP : InputMode::FLOATING, invert) {
};

PinSwitch::PinSwitch(int pin, int outputNum, SwitchMode mode)
    : PinSwitch::PinSwitch(pin, outputNum, INPUT_PULLUP, mode) {
};
