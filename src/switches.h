#ifndef MASZYNADUINO_SWITCHES_H
#define MASZYNADUINO_SWITCHES_H

#include "Arduino.h"
#include "mux.h"
#include "comm.h"


class Console;
class DebugMonitor;

enum class SwitchMode { NORMAL=0, INVERT };


class Switch {
  public:
    virtual void setup();
    void update();
    void respond(MaszynaState *state);
    virtual void onChange();
    bool getState();
    bool isOn();
    bool isOff();
    int getOutputNumber();
  protected:
    int mode;
    int outputNum;
    bool state = false;
    bool invert = false;

    virtual bool probe();
};


class PinSwitch : public Switch {
  public:
    PinSwitch(int pin, int outputNum, int mode, SwitchMode invert);
    PinSwitch(int pin, int outputNum, SwitchMode mode = SwitchMode::NORMAL);
    void setup();
  protected:
    bool probe();
  private:
    int pin;
};


class MuxSwitch : public Switch {
  public:
    MuxSwitch(Mux *mux, int channel, int outputNumber, SwitchMode invert = SwitchMode::NORMAL);
    void setup();
  protected:
    bool probe();
  private:
    Mux *mux;
    int channel;
};

#endif
