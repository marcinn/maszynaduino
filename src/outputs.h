#ifndef MASZYNADUINO_OUTPUTS_H
#define MASZYNADUINO_OUTPUTS_H
#include "PCF8574.h"

enum ActiveStateMode {
    ACTIVE_LOW = 0,
    ACTIVE_HIGH
};

class IOutput {
    public:
        virtual void write(int pin, int value) = 0;
        virtual void setup(int pin) = 0;
};

class PCF8574Output : public IOutput {
    public:
        void write(int pin, int value);
        void setup(int pin);
        PCF8574Output(int address);
    protected:
        PCF8574 *expander;
        int state = 0;
        bool initialized = false;
};


class ArduinoOutput : public IOutput {
    public:
        ArduinoOutput(ActiveStateMode activeStateMode=ActiveStateMode::ACTIVE_HIGH);
        virtual void write(int pin, int value);
        void setup(int pin);
    protected:
        uint8_t _low = LOW;
        uint8_t _high = HIGH;
};

class ArduinoAnalogOutput : public ArduinoOutput {
    void write(int pin, int value);
};

#endif
