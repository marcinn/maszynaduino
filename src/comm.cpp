#include "comm.h"


Transmitter MaszynaTransmitter = Transmitter();

BaseConsole::BaseConsole() {
}

bool BaseConsole::isInitialized() {
    return this->initialized;
}

void Transmitter::registerConsole(BaseConsole *console) {
    consoles[this->consolesNum] = console;
    this->consolesNum++;
}

bool Transmitter::isTransmissionActive() {
    return this->transmissionActive;
}

uint8_t Transmitter::getRegisteredConsolesNumber() {
    return this->consolesNum;
}

void Transmitter::transmitNextConsole() {
    if(this->consolesNum > 0) {
        this->transmissionActive = true;
        this->consoles[this->currentConsole]->update();
        this->consoles[this->currentConsole]->transmit();
        this->currentConsole++;
        if(this->currentConsole == this->consolesNum) {
            this->currentConsole = 0;
        }
        this->transmissionActive = false;
    }
}
