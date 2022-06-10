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

bool Transmitter::isSynced() {
    return synced;
}

SerialTransmitter::SerialTransmitter(HardwareSerial *serial, unsigned long baud, unsigned int updateTime) : Transmitter() {
    this->baud = baud;
    this->serial = serial;
    this->updateTime = updateTime;
}

void SerialTransmitter::transmit() {
    if(!initialized) {
        this->serial->begin(baud, SERIAL_8N1);
        initialized = true;
    }

    if(millis() - lastUpdated < updateTime) {
        return;
    }

    lastUpdated = millis();

    int bw = serial->write((uint8_t *) state->getOutputs(), OUTPUTS_SIZE);
    if(bw < OUTPUTS_SIZE) {
        return;
    }

    int br = serial->readBytes((uint8_t *) &inBuf, INPUTS_SIZE);


    if(br == INPUTS_SIZE) {
        uint8_t *p = (uint8_t *) &inBuf.preamble;
        synced = ((p[0] == 0xEF) && (p[1] == 0xEF) && (p[2] == 0xEF) && (p[3] == 0xEF));

        if(synced) {
            lastRead = millis();
            state->setInputs(&inBuf);
        } else {
            int syncstep = 0;
            while(syncstep<4) {
                int c = serial->read();
                if(c == -1) {
                    break;
                }
                if(c == 0xEF) {
                    syncstep++;
                }
                if(syncstep == 4) {
                    synced = true;
                    serial->readBytes((uint8_t *) &inBuf+4, INPUTS_SIZE-4);
                    break;
                }
            }
        }
    }

    if(transmissionActive && (millis() - lastRead) > 500) {
        transmissionActive = false;
        synced = false;
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
    int const indNum = static_cast<int>(alert);
    setIndicatorState(indNum, state);
}

void MaszynaState::setIndicatorState(int indNum, bool state) {
    int const byteNum = indNum >> 3;
    int const bitNum = indNum % 8;
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
