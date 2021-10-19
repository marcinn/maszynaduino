#pragma once
#include "TM1637Display.h"
#include "display.h"
#include "comm.h"

class TM1637Output {
    public:
        virtual void setNumber(uint16_t number, bool colon = false);
        virtual void setup();
};

class TM1637ArduinoOutput : public TM1637Output {
    public:
        TM1637ArduinoOutput(int clockPin, int dioPin);
        void setNumber(uint16_t number, bool colon = false) override;
        void setup() override;
    protected:
        TM1637Display *display;
};

class TM1637Clock : public GenericDisplay {
    public:
        TM1637Clock(TM1637Output *output);
        void update(MaszynaState *state);
        void setup();
        void respond();
    protected:
        TM1637Output *output;
        bool displayColon = false;
        s_datetime *dt;
};
