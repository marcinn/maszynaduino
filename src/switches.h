#ifndef MASZYNADUINO_SWITCHES_H
#define MASZYNADUINO_SWITCHES_H

#include "Arduino.h"
#include "mux.h"


class Console;


enum SwitchMode { NORMAL=0, INVERT };


class Switch {
  public:
    virtual void setup();
    void update();
    void respond(Console *console);
    bool getState();
    bool isOn();
    bool isOff();
  protected:
    int mode;
    int frame;
    int bitNum;
    bool state = false;
    bool invert = false;

    virtual bool probe();
};


class PinSwitch : public Switch {
  public:
    PinSwitch(int pin, int frame, int bitNum, int mode, SwitchMode invert);
    PinSwitch(int pin, int frame, int bitNum);
    PinSwitch(int pin, int outputNumber, SwitchMode invert=SwitchMode::NORMAL);
    void setup();
  protected:
    bool probe();
  private:
    int pin;
};


class MuxSwitch : public Switch {
  public:
    MuxSwitch(Mux *mux, int channel, int frame, int bitNum, SwitchMode invert);
    MuxSwitch(Mux *mux, int channel, int frame, int bitNum);
    MuxSwitch(Mux *mux, int channel, int outputNumber, SwitchMode invert=SwitchMode::NORMAL);
    void setup();
  protected:
    bool probe();
  private:
    Mux *mux;
    int channel;
};

#endif
