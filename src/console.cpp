#include "Arduino.h"
#include "console.h"
#include "comm.h"


Console::Console(HardwareSerial *serial) {
    this->serial = serial;
    //MaszynaTransmitter.registerConsole(this);
}

Console::Console(HardwareSerial *serial, Switch **switches, int switchesCount) : Console(serial) {
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
    this->serial->begin(19200, SERIAL_8N1);

    for (int i = 0; i < this->switchesCount; i++) {
        this->switches[i]->setup();
    }
    for (int i = 0; i < this->indicatorsCount; i++) {
        this->indicators[i]->setup();
    }

    this->initialized = true;
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
    bool readframe = false;
    //this->serial->readBytes((byte *) &this->tmpBuf, 4);
        if(this->serial) {
            /*
            if(!this->transmissionActive) {
                this->serial->write((byte *) &this->output, sizeof(this->output));
                this->transmissionActive = true;
            } else {
                this->serial->readBytes((byte *) &this->input, sizeof(this->input));
                this->serial->write((byte *) &this->output, sizeof(this->output));
            }*/
            this->serial->readBytes((byte *) &this->input, sizeof(this->input));
            this->serial->write((byte *) &this->output, sizeof(this->output));
            //this->serial->write((byte *) &this->preamble, 4);
        }
    /*
       if(this->serial->available() > 3) {
       if(!this->awaitingFrame) {
       this->serial->readBytes((uint8_t *) &this->tmpBuf, 4);
       if(this->tmpBuf[0] == 0xEF && this->tmpBuf[1] == 0xEF && this->tmpBuf[2] == 0xEF && this->tmpBuf[3] == 0xEF) {
       this->synced = true;
       readframe = true;
       } else {
       this->synced = false;
       }
       }
       if(readframe && this->synced) {
       if(this->serial->available() >= sizeof(this->input)) {
       this->serial->readBytes((byte *) &this->input, sizeof(this->input));
       }
       }
       }
       */
    /*
       if(this->serial) {
       if(!this->synced) {
       if(this->serial->available()) {
       uint8_t c = this->serial->read();
       if (c==0xEF) {
       this->syncstep++;
       if(this->syncstep==4) {
       this->synced = true;
       this->syncstep = 0;
       if(this->serial->available() >= sizeof(this->input)) {
       this->serial->readBytes((byte *) &this->input, sizeof(this->input));
       }
       }
       } else {
       this->syncstep = 0;
       }
       }
       } else {
       if(this->serial->available() > 3) {
       this->serial->readBytes((uint8_t *) &this->tmpBuf, 4);
       if(!(this->tmpBuf[0] == 0xEF && this->tmpBuf[1] == 0xEF && this->tmpBuf[2] == 0xEF && this->tmpBuf[3] == 0xEF)) {
       this->synced = false;
       }
       }
       if(this->synced && this->serial->available() >= sizeof(this->input)) {
       this->serial->readBytes((byte *) &this->input, sizeof(this->input));
       }
       }
       this->serial->write(this->preamble, 4);
       this->serial->write((byte *) &this->output, sizeof(this->output));
       }
       */
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

HardwareSerial* Console::getSerial() {
    return this->serial;
}

int Console::getSerialBaud() {
    return this->baud;
}

bool Console::isTransmissionActive() {
    return this->transmissionActive;
}

bool Console::isTransmissionStarted() {
    return this->transmissionStarted;
}
