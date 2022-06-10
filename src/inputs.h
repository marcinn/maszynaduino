#pragma once
#include <MCP23017.h>

enum InputMode {
    PULLUP = 2,
    FLOATING = 0
};

class IInput {
    public:
        virtual int read(int pin) = 0;
        virtual void setup(int pin, InputMode mode = InputMode::PULLUP) = 0;
};

class MCP23017Input : public IInput {
    public:
        int read(int pin);
        void setup(int pin, InputMode mode = InputMode::PULLUP);
        MCP23017Input(int address);
    protected:
        MCP23017 *expander;
        int state = 0;
        bool initialized = false;
};


class ArduinoDigitalInput : public IInput {
    public:
        int read(int pin);
        void setup(int pin, InputMode mode = InputMode::PULLUP);
};

extern ArduinoDigitalInput *arduinoDigitalInputs;
