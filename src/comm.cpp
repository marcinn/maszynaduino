#include "Arduino.h"
#include "comm.h"
#include "debugmonitor.h"

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

SerialTransmitter::SerialTransmitter(HardwareSerial *serial, int baud) : Transmitter() {
    this->baud = baud;
    this->serial = serial;
#ifdef MASZYNADUINO_MASZYNA_UART_SYNC_BUG_WORKAROUND
    this->serial->setTimeout(100);
#else
    this->serial->setTimeout(1000);
#endif
}

void SerialTransmitter::transmit() {
    if(!initialized) {
        this->serial->begin(baud, SERIAL_8N1);
        initialized = true;
    }
    if(serial) {
        InputFrame *inputs = Maszyna->getInputs();
        OutputFrame *outputs = Maszyna->getOutputs();

#ifdef MASZYNADUINO_MASZYNA_UART_SYNC_BUG_WORKAROUND
        this->serial->readBytes((uint8_t *) inputs, sizeof(InputFrame));
        this->serial->write((uint8_t *) outputs, sizeof(OutputFrame));
#else
        if(serial->available() >= sizeof(InputFrame)) {
            this->serial->readBytes((uint8_t *) inputs, sizeof(InputFrame));
        }
        if(serial->availableForWrite() >= sizeof(OutputFrame)) {
            this->serial->write((uint8_t *) outputs, sizeof(OutputFrame));
        }
#endif
    }
}

HardwareSerial* SerialTransmitter::getSerial() {
    return serial;
}

int SerialTransmitter::getSerialBaud() {
    return baud;
}

void SerialTransmitter::debugMonitor(HardwareSerial *dbg) {
    dbg->print("Active: ");
    dbg->print((bool) serial);
    dbg->print("  InBuf: ");
    dbg->print(serial->available());
    dbg->print("  WrAvl: ");
    dbg->print(serial->availableForWrite());
    dbg->print("  Baud: ");
    dbg->print(baud);
    dbg->print("  TrnActive: ");
    dbg->print(transmissionActive);
    dbg->println();
}

InputFrame* MaszynaState::getInputs() {
    return &input;
}
OutputFrame* MaszynaState::getOutputs() {
    return &output;
}

bool MaszynaState::getIndicatorState(int indicatorNum) {
    uint8_t const byteNum = (indicatorNum / 8);
    uint8_t const bitNum = indicatorNum % 8;
    return ((uint8_t *) &input)[byteNum+4] & (1 << bitNum);
}

void MaszynaState::setOutputSwitch(uint8_t num, bool state) {
    if(num > 47) {
        return;
    }

    uint8_t const byteNum = (num / 8);
    uint8_t const bitNum = num % 8;

    setOutputBit(byteNum, bitNum, state);
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
