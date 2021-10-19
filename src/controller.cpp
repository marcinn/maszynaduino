#include "controller.h"

ArduinoAnalogInput::ArduinoAnalogInput(int pin, int max, bool invert) {
    this->pin = pin;
    this->invert = invert;
    this->scale = 1024 / max;
}

void ArduinoAnalogInput :: setup() {
    pinMode(pin, INPUT);
};


int ArduinoAnalogInput :: read() {
    int x = analogRead(pin);
    return (invert ? 1023 - x : x) / scale;
};

Controller::Controller(AnalogInput *input, ControllerType type) {
    this->input = input;
    this->type = type;
}

void Controller::setup() {
    input->setup();
}

void Controller::update() {
    this->value = input->read();
}

void Controller::respond(MaszynaState *state) {
    state->setControllerValue(type, value);
}
