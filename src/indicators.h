#ifndef MASZYNADUINO_INDICATORS_H
#define MASZYNADUINO_INDICATORS_H

#include "Arduino.h"
#include "comm.h"


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

#endif
