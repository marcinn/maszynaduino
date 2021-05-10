#include "Arduino.h"
#include "mux.h"
#include "comm.h"
#include "debugmonitor.h"


//uint16_t _MUXDIR[MAX_MUXERS] = {MUXDIR_INPUT};

volatile uint8_t _MUX_COUNT = 0;
volatile uint32_t _MUX_STATE = 0;
volatile uint8_t _MUX_CURRENT_CHANNEL = 0;

Mux *_MUXERS[MAX_MUXERS] = {};
volatile uint8_t _MUX_CURRENT = 0;
volatile uint8_t _MUX_PREV = 0;


ISR(TIMER2_COMPA_vect) {
    if(_MUX_COUNT > 0) {
        _MUXERS[_MUX_PREV]->disable();
        Mux *mux = _MUXERS[_MUX_CURRENT];
        mux->channel(_MUX_CURRENT_CHANNEL);
        bool state;

        if(mux->getChannelMode(_MUX_CURRENT_CHANNEL) == MuxChannelMode::pullup) {
            mux->enable();
            state = mux->readDigital();
            mux->disable();
        } else {
            state = mux->getRequestedChannelState(_MUX_CURRENT_CHANNEL);
            mux->writeDigital(state);
            mux->enable();
        }
        if(state) {
            _MUX_STATE |= (1<<_MUX_CURRENT_CHANNEL);
        } else {
            _MUX_STATE &= ~(1<<_MUX_CURRENT_CHANNEL);
        }
        _MUX_CURRENT_CHANNEL++;
        if(_MUX_CURRENT_CHANNEL>=mux->channelsCount()) {
            mux->setState(_MUX_STATE);
            _MUX_CURRENT_CHANNEL = 0;
            _MUX_STATE = 0;
            _MUX_PREV = _MUX_CURRENT;
            _MUX_CURRENT++;
            if(_MUX_CURRENT == _MUX_COUNT) {
                _MUX_CURRENT = 0;
            }
        }
    }
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
    return channelDirections & (1 << channel) ? MuxChannelMode::output : pullup;
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

void Mux::debugMonitor(DebugMonitor *d) {
    d->printBits(state, channels);
    d->print(" D: ");
    for(int i=channels-1; i>0; i--) {
        d->print(channelDirections & (1 << i) ? 'O': '.');
    }
    d->print(" RQ: ");
    for(int i=channels-1; i>0; i--) {
        d->print(requestedState & (1 << i) ? '+': '-');
    }
    d->println();
}

void Mux::initializeTimers() {
    if(timersInitialized) {
        return;
    }

    cli();
    //160000
    TCCR2A = 0; // set entire TCCR2A register to 0
    TCCR2B = 0; // same for TCCR2B
    TCNT2  = 0; // initialize counter value to 0
    // set compare match register for 160000 Hz increments
    OCR2A = 99; // = 16000000 / (1 * 160000) - 1 (must be <256)
    // turn on CTC mode
    TCCR2B |= (1 << WGM21);
    // Set CS22, CS21 and CS20 bits for 1 prescaler
    TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    sei();
}

bool Mux::timersInitialized = false;
