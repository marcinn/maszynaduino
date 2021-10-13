#include "Arduino.h"
#include "mux.h"
#include "comm.h"
#include "console.h"



//uint16_t _MUXDIR[MAX_MUXERS] = {MUXDIR_INPUT};

volatile uint8_t _MUX_COUNT = 0;
volatile uint32_t _MUX_STATE = 0;
volatile uint8_t _MUX_CURRENT_CHANNEL = 0;

Mux *_MUXERS[MAX_MUXERS] = {};
volatile uint8_t _MUX_CURRENT = 0;
volatile uint8_t _MUX_PREV = 0;

unsigned long _MUX_TIME = 0;
bool _MUX_SETUP = false;


void update_muxers() {
    unsigned long muxStart = micros();
    uint32_t muxState = 0;

    if(_MUX_COUNT > 0) {
        for(uint8_t m=0;m<_MUX_COUNT;m++) {
            muxState = 0;
            Mux *mux = _MUXERS[m];
            if(!_MUX_SETUP) {
                mux->setup();
            }
            mux->enable();
            for(uint8_t ch=0; ch<mux->channelsCount(); ch++) {
                mux->channel(ch);
                bool state;

                if(mux->getChannelMode(ch) == MuxChannelMode::pullup) {
                    state = !mux->readDigital();
                    muxState |= (state<<ch);
                } else {
                    state = mux->getRequestedChannelState(ch);
                    mux->writeDigital(state);
                    if(!state) {
                        muxState |= (1<<ch);
                    } else {
                        muxState &= ~(1<<ch);
                    }
                }
            }
            mux->disable();
            mux->setState(~muxState);
        }
        _MUX_SETUP = false;
    }
    _MUX_TIME = micros() - muxStart;
}


Mux::Mux(int pinEnable, int pinData, int pinS0, int pinS1, int pinS2, int pinS3) {
    this->pinEnable = pinEnable;
    this->pinData = pinData;
    this->pinS0 = pinS0;
    this->pinS1 = pinS1;
    this->pinS2 = pinS2;
    this->pinS3 = pinS3;

    _MUXERS[_MUX_COUNT] = this;
    _MUX_COUNT++;

    this->setup();
}

void Mux::setup() {
    pinMode(this->pinS0, OUTPUT);
    pinMode(this->pinS1, OUTPUT);
    pinMode(this->pinS2, OUTPUT);
    pinMode(this->pinS3, OUTPUT);
    pinMode(this->pinEnable, OUTPUT);

    this->disable();
}

void Mux::setState(uint32_t state) {
    this->state = state;
}

uint32_t Mux::getState() {
    return this->state;
}

void Mux::enable() {
    digitalWrite(this->pinEnable, LOW);
}

void Mux::disable() {
    digitalWrite(this->pinEnable, HIGH);
}

bool Mux::readDigital() {
    return digitalRead(this->pinData);
}

int Mux::readAnalog() {
    return analogRead(this->pinData);
}

void Mux::writeDigital(bool value) {
    return digitalWrite(this->pinData, value ? HIGH : LOW);
}

void Mux::channel(int ch) {
    digitalWrite(this->pinS0, ch & 1);
    digitalWrite(this->pinS1, ch & 2);
    digitalWrite(this->pinS2, ch & 4);
    digitalWrite(this->pinS3, ch & 8);
    pinMode(
            this->pinData,
            channelDirections & (1 << ch) ? OUTPUT : INPUT_PULLUP
           );
}

void Mux::setChannelMode(uint8_t channel, MuxChannelMode mode) {
    if(mode == MuxChannelMode::pullup) {
        channelDirections &= ~(1 << channel);
    } else {
        channelDirections |= (1 << channel);
    }
}

MuxChannelMode Mux::getChannelMode(int channel) {
    return channelDirections & (1 << channel) ? MuxChannelMode::output : MuxChannelMode::pullup;
}

uint32_t Mux::getDirections() {
    return channelDirections;
}

bool Mux::getChannelState(int channel) {
    return state & (1 << channel);
}

bool Mux::getRequestedChannelState(int channel) {
    return requestedState & (1 << channel);
}

void Mux::requestChannelState(int channel, bool state) {
    if(state) {
        requestedState |= 1 << channel;
    } else {
        requestedState &= ~(1 << channel);
    }
}

void Mux::debug(DebugFrame *d) {
}
