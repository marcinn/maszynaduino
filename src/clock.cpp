#include "outputs.h"
#include "comm.h"
#include "clock.h"


TM1637Clock :: TM1637Clock(TM1637Output *output) {
    this->output = output;
}

bool TM1637Clock :: update(MaszynaState *state) {
    dt = state->getSimDateTime();
    return true;
}


void TM1637Clock :: setup() {
    output->setup();
}

void TM1637Clock :: respond() {
    output->setNumber(
        (dt->hour / 10) * 1000
        + (dt->hour % 10) * 100
        + dt->minute,
        dt->second % 2
    );
}

TM1637ArduinoOutput::TM1637ArduinoOutput(int clockPin, int dioPin) {
    display = new TM1637Display(clockPin, dioPin);
}

void TM1637ArduinoOutput::setNumber(uint16_t number, bool colon) {
    display->showNumberDecEx(number, colon ? 0b01000000 : 0, true);
}

void TM1637ArduinoOutput::setup() {
    display->setBrightness(0xf);
}
