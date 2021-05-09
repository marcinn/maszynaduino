#ifndef MASZYNADUINO_INDICATORS_H
#define MASZYNADUINO_INDICATORS_H

#include "Arduino.h"
#include "comm.h"
#include "mux.h"


class Indicator {
    public:
        void update(MaszynaState *state);
        virtual void setup();
        virtual void respond();

    protected:
        virtual bool readState(MaszynaState *state);
        int indicatorNumber;
        bool state = false;
        bool invert = false;
};


class PinIndicator : public Indicator {
    public:
        PinIndicator(int pin, int indicatorNumber);
        virtual void setup();
        virtual void respond();
    private:
        int pin;
};


class MuxIndicator : public Indicator {
    public:
        MuxIndicator(Mux *mux, int channel, int indicatorNumber);
        virtual void setup();
        virtual void respond();
    private:
        Mux *mux;
        int channel;
};

#endif
