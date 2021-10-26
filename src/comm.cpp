#include "Arduino.h"
#include "comm.h"
#include "controller.h"
#define MIN(a,b) (((a)<(b))?(a):(b))

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

SerialTransmitter::SerialTransmitter(HardwareSerial *serial, unsigned long baud, unsigned int minPause) : Transmitter() {
    this->baud = baud;
    this->serial = serial;
    this->serial->setTimeout(250);
    this->minPause = minPause;
}

void SerialTransmitter::transmit() {
    if(!initialized) {
        this->serial->begin(baud, SERIAL_8N1);
        initialized = true;
    }
    if ((millis() - lastSend) > minPause) {

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
                serial->readBytes((uint8_t *) &tmpBuf, MIN(serial->available(), sizeof(InputFrame)));
                //serial->readBytes((uint8_t *) &tmpBuf, serial->available());
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
    return getIndicatorState(indicatorNum);
}

bool MaszynaState::getIndicatorState(uint8_t indicatorNum) {
    return ((uint8_t *) &input.indicator0)[indicatorNum >> 3] & (1 << indicatorNum % 8);
}

void MaszynaState::setIndicatorState(Alert alert, bool state) {
    uint8_t const indNum = static_cast<uint8_t>(alert);
    uint8_t const byteNum = indNum >> 3;
    uint8_t const bitNum = indNum % 8;
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

void MaszynaState::setControllerValue(ControllerType type, int value) {
    OutputFrame *o = &output;
    switch(type) {
        case ControllerType::master_controller:
            o->master_controller = (uint8_t) value;
            break;
        case ControllerType::secondary_controller:
            o->second_controller = (uint8_t) value;
            break;
        case ControllerType::train_brake:
            o->train_brake = (uint16_t) value;
            break;
        case ControllerType::independent_brake:
            o->independent_brake = (uint16_t) value;
            break;
    }
};


void MaszynaState::setMuxCalcTime(unsigned long t) {
    muxCalcTime = t;
}

unsigned long MaszynaState::getMuxCalcTime() {
    return muxCalcTime ;
}

s_datetime* MaszynaState::getSimDateTime() {
    uint16_t dhm = input.day_hour_minute;
    uint16_t ym = input.year_month;
    uint16_t sec = input.second_milisecond;
    int day = dhm / 1440;
    int minutes = dhm % 1440;
    int hour = minutes / 60;
    int minute = minutes % 60;
    int year = ym / 12;
    int month = ym % 12;
    int second = sec / 1000;

    this->datetime.year = year+1;
    this->datetime.month = month+1;
    this->datetime.day = day+1;
    this->datetime.hour = hour;
    this->datetime.minute = minute;
    this->datetime.second = second;

    return &datetime;
}
