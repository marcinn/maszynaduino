#ifndef MASZYNADUINO_CONSOLE_H
#define MASZYNADUINO_CONSOLE_H

#include "Arduino.h"
#include "switches.h"
#include "indicators.h"
#include "comm.h"

#ifndef CONSOLE_MAX_SWITCHES
#define CONSOLE_MAX_SWITCHES 100
#endif

#ifndef CONSOLE_MAX_INDICATORS
#define CONSOLE_MAX_INDICATORS 100
#endif


class Console {
  public:
    Console();
    Console(Switch **switches, int switchesCount);
    Console(Switch **switches, int switchesCount, Indicator **indicators, int indicatorsCount);
    void setup();
    void update();
    void addSwitch(Switch *);
    void addIndicator(Indicator *indicator);
    int getSwitchesCount();
    int getIndicatorsCount();
  private:
    int switchesCount = 0;
    int indicatorsCount = 0;
    Switch *switches[CONSOLE_MAX_SWITCHES]; // bufor
    Indicator *indicators[CONSOLE_MAX_INDICATORS]; // bufor
};

#endif
