#include "Arduino.h"
#include "mux.h"
#include "comm.h"


//uint16_t _MUXDIR[MAX_MUXERS] = {MUXDIR_INPUT};

volatile uint8_t _MUX_COUNT = 0;
volatile uint32_t _MUX_STATE = 0;
volatile uint8_t _MUX_CURRENT_CHANNEL = 0;

Mux *_MUXERS[MAX_MUXERS] = {};
volatile uint8_t _MUX_CURRENT = 0;


ISR(TIMER2_COMPA_vect) {
    if(_MUX_COUNT > 0) {
        Mux *mux = _MUXERS[_MUX_CURRENT];
        mux->channel(_MUX_CURRENT_CHANNEL);
        if(_MUX_CURRENT_CHANNEL == 0) {
            mux->setDataPinMode(INPUT_PULLUP);
            mux->enable();
        }
        bool state = mux->readDigital();
        if(state) {
            _MUX_STATE |= (1<<_MUX_CURRENT_CHANNEL);
        } else {
            _MUX_STATE &= ~(1<<_MUX_CURRENT_CHANNEL);
        }
        _MUX_CURRENT_CHANNEL++;
        if(_MUX_CURRENT_CHANNEL>=mux->channelsCount()) {
            mux->disable();
            mux->setState(_MUX_STATE);
            _MUX_CURRENT_CHANNEL = 0;
            _MUX_STATE = 0;
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

void Mux::setDataPinMode(int mode) {
    pinMode(this->pinData, mode);
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

void Mux::writeAnalog(int value) {
    return analogWrite(this->pinData, value);
}

void Mux::channel(int ch) {
    digitalWrite(this->pinS0, ch & 1);
    digitalWrite(this->pinS1, ch & 2);
    digitalWrite(this->pinS2, ch & 4);
    digitalWrite(this->pinS3, ch & 8);
}

void Mux::initializeTimers() {
    if(timersInitialized) {
        return;
    }

    cli();
    /*
    TCCR0A = 0;// set entire TCCR1A register to 0
    TCCR0B = 0;// same for TCCR1B
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR0A = 154;
    //OCR1A = 1300;
    // turn on CTC mode
    TCCR0B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR0B |= (1 << CS12) | (1 << CS10);
    */
    //TIMSK0 |= (1 << OCIE0A);

    TCCR2A = 0;// set entire TCCR1A register to 0
    TCCR2B = 0;// same for TCCR1B
    TCNT2  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR2A = 154;
    //OCR1A = 1300;
    // turn on CTC mode
    TCCR2B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR2B |= (1 << CS12) | (1 << CS10);
    TIMSK2 |= (1 << OCIE2A);
    sei();
}

bool Mux::timersInitialized = false;
