#ifndef MASZYNADUINO_SWITCHES_H
#define MASZYNADUINO_SWITCHES_H

#include "Arduino.h"
#include "mux.h"
#include "comm.h"
#include "inputs.h"


class Console;
class DebugMonitor;

enum class SwitchMode { NORMAL=0, INVERT };


class Switch {
  public:
    Switch(IInput *input, int pin, int outputNumber, InputMode mode=InputMode::PULLUP, SwitchMode invert=SwitchMode::NORMAL);
    virtual void setup();
    void update();
    void respond(MaszynaState *state);
    virtual void onChange();
    bool getState();
    bool isOn();
    bool isOff();
    int getOutputNumber();
  protected:
    int pin;
    InputMode mode;
    int outputNum;
    bool state = false;
    bool invert = false;
    IInput *input;

    bool probe();
};


/* DEPRECADED */
class PinSwitch : public Switch {
  public:
    PinSwitch(int pin, int outputNum, int mode, SwitchMode invert);
    PinSwitch(int pin, int outputNum, SwitchMode mode = SwitchMode::NORMAL);
};


#endif
