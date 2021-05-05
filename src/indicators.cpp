#include "Arduino.h"
#include "indicators.h"


bool Indicator::readState(InputFrame *inputs) {
    return ((uint8_t *) inputs)[this->frame+4] & (1 << this->bitNum);
}

void Indicator::update(InputFrame *inputs) {
    this->state = this->readState(inputs);
}

PinIndicator::PinIndicator(int pin, int frame, int bitNum) {
    this->frame = frame;
    this->bitNum = bitNum;
    this->pin = pin;
}

void PinIndicator::setup() {
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, LOW);
}

void PinIndicator::respond() {
    digitalWrite(this->pin, this->state ? HIGH : LOW);
}
