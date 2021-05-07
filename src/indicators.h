#ifndef MASZYNADUINO_INDICATORS_H
#define MASZYNADUINO_INDICATORS_H

#include "Arduino.h"
#include "comm.h"
#include "mux.h"


class Indicator {
    public:
        void update(InputFrame *inputs);
        virtual void setup();
        virtual void respond();

    protected:
        virtual bool readState(InputFrame *inputs);
        int frame;
        int bitNum;
        bool state = false;
        bool invert = false;
};


class PinIndicator : public Indicator {
    public:
        PinIndicator(int pin, int frame, int bitNum);
        virtual void setup();
        virtual void respond();
    private:
        int pin;
};


class MuxIndicator : public Indicator {
    public:
        MuxIndicator(Mux *mux, int channel, int frameNum, int bitNum);
        virtual void setup();
        virtual void respond();
    private:
        Mux *mux;
        int channel;
};

#endif
