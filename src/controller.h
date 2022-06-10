#pragma once

#include "outputs.h"
#include "comm.h"

class AnalogInput {
    public:
        virtual void setup();
        virtual int read();
};

class ArduinoAnalogInput : public AnalogInput {
    public:
        ArduinoAnalogInput(int pin, int max = 1024, bool invert = false);
        void setup();
        int read();
    protected:
        int pin;
        bool invert;
        int scale;
};


enum class ControllerType {
    master_controller = 0,
    secondary_controller,
    train_brake,
    independent_brake,
    NONE = -1
};


class Controller {
    public:
    Controller(AnalogInput *input, ControllerType type);
    void setup();
    void update();
    void respond(MaszynaState *state);

    protected:
        AnalogInput *input;
        ControllerType type;
        int value = 0;
};
