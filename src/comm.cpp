#include "Arduino.h"
#include "comm.h"

extern MaszynaState *Maszyna;


Transmitter::Transmitter() {
    this->state = (MaszynaState *) Maszyna;
}

MaszynaState *Transmitter::getState() {
    return this->state;
}

bool Transmitter::isTransmissionStarted() {
    return transmissionStarted;
}

bool Transmitter::isTransmissionActive() {
    return transmissionActive;
}

SerialTransmitter::SerialTransmitter(HardwareSerial *serial, unsigned long baud) : Transmitter() {
    this->baud = baud;
    this->serial = serial;
    this->serial->setTimeout(250);
}

void SerialTransmitter::transmit() {
    if(!initialized) {
        this->serial->begin(baud, SERIAL_8N1);
        initialized = true;
    }
    if ((millis() - lastSend) > 100) {

        OutputFrame *outputs =  state->getOutputs();
        InputFrame *inputs =  state->getInputs();

        int bytesToRead = (INPUTS_SIZE - (skipPreamble ? 4 : 0));
        int readOffset = skipPreamble ? 4 : 0;

        if (this->synced) {
            if (serial->available() >= bytesToRead) {

                int br = serial->readBytes((uint8_t *) &tmpBuf + readOffset, bytesToRead);
                if (br != bytesToRead) {
                    this->synced = false;
                    syncstep = 0;
                } else {
                    serial->write((uint8_t *) outputs, OUTPUTS_SIZE);
                    skipPreamble = false;
                    this->synced = (tmpBuf.preamble[0] == 0xEF && tmpBuf.preamble[1] == 0xEF && tmpBuf.preamble[2] == 0xEF && tmpBuf.preamble[3] == 0xEF);
                    if (this->synced) {
                        memcpy((uint8_t*) inputs, (uint8_t*) &tmpBuf, INPUTS_SIZE);
                        lastSend = millis();
                        lastRead = lastSend;
                    } else {
                        syncstep = 0;
                    }

                }
            } else {
                serial->readBytes((uint8_t *) &tmpBuf, serial->available());
                serial->write((uint8_t *) outputs, sizeof(OutputFrame));
                lastSend = millis();
            }
        } else if (!serial->available()) {
            if ((millis() - lastSend) > 1000) {
                int bw = serial->write((uint8_t *) outputs, sizeof(OutputFrame));
                lastSend = millis();
            }
        } else {
            int c;

            while (serial->available() && (c = serial->read())) {
                if (c == -1) {
                    break;
                }
                if (c == 0xEF) {
                    syncstep++;
                    if (syncstep == 4) {
                        syncstep = 0;
                        skipPreamble = true;

                        this->synced = true;
                        if (serial->available() >= INPUTS_SIZE - 4) {

                            int br = serial->readBytes((uint8_t *) &tmpBuf + 4, INPUTS_SIZE - 4);

                            if (br == INPUTS_SIZE - 4) {
                                skipPreamble = false;

                            } else {
                                this->synced = false;
                            }
                        }
                        break;
                    }
                } else {
                    syncstep = 0;
                }
            }
        }
    }
    if(transmissionActive && (millis() - lastRead) > 500) {
        transmissionActive = false;
    }
    if(!transmissionActive && (millis() - lastRead) < 500) {
        transmissionActive = true;
    }
}

HardwareSerial* SerialTransmitter::getSerial() {
    return serial;
}

unsigned long SerialTransmitter::getSerialBaud() {
    return baud;
}

InputFrame* MaszynaState::getInputs() {
    return &input;
}
OutputFrame* MaszynaState::getOutputs() {
    return &output;
}

bool MaszynaState::getIndicatorState(Alert alert) {
    uint8_t const indicatorNum = static_cast<int>(alert);
    uint8_t const byteNum = (indicatorNum / 8);
    uint8_t const bitNum = indicatorNum % 8;
    return ((uint8_t *) &input.indicator0)[byteNum] & (1 << bitNum);
}

void MaszynaState::setIndicatorState(Alert alert, bool state) {
    uint8_t const indicatorNum = static_cast<int>(alert);
    uint8_t const byteNum = (indicatorNum / 8);
    uint8_t const bitNum = indicatorNum % 8;
    if (state) {
        ((uint8_t *) &input.indicator0)[byteNum] |= (1 << bitNum);
    } else {
        ((uint8_t *) &input.indicator0)[byteNum] &= ~(1 << bitNum);
    }
}

void MaszynaState::setOutputSwitch(uint8_t num, bool state) {
    if(num > 47) {
        return;
    }

    uint8_t const byteNum = (num / 8);
    uint8_t const bitNum = num % 8;

    setOutputBit(byteNum, bitNum, state);
}

bool MaszynaState::getOutputSwitch(uint8_t num) {
    uint8_t const byteNum = (num / 8);
    uint8_t const bitNum = num % 8;
    return ((uint8_t *) &output)[byteNum+4] & 1 << bitNum;
}

void MaszynaState::setOutputBit(uint8_t byteNum, uint8_t bitNum, bool state) {
    if(state) {
        ((uint8_t *) &output)[byteNum+4] |= 1 << bitNum;
    } else {
        ((uint8_t *) &output)[byteNum+4] &= ~(1 << bitNum);
    }
}

void MaszynaState::setInputs(InputFrame *frame) {
    memcpy(&this->input, (uint8_t *) frame, sizeof(InputFrame));
}

void MaszynaState::setMuxCalcTime(unsigned long t) {
    muxCalcTime = t;
}

unsigned long MaszynaState::getMuxCalcTime() {
    return muxCalcTime ;
}
