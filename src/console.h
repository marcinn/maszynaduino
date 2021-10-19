#ifndef MASZYNADUINO_CONSOLE_H
#define MASZYNADUINO_CONSOLE_H

#include "Arduino.h"
#include "switches.h"
#include "indicators.h"
#include "controller.h"
#include "display.h"
#include "comm.h"

#ifndef MAX_CONSOLES
#define MAX_CONSOLES 4
#endif

#ifndef CONSOLE_MAX_SWITCHES
#define CONSOLE_MAX_SWITCHES 40
#endif

#ifndef CONSOLE_MAX_REGULATORS
#define CONSOLE_MAX_REGULATORS 4
#endif

#ifndef CONSOLE_MAX_INDICATORS
#define CONSOLE_MAX_INDICATORS 40
#endif

#ifndef CONSOLE_MAX_DISPLAYS
#define CONSOLE_MAX_DISPLAYS 4
#endif


class Console {
  public:
    Console();
    Console(Switch **switches, int switchesCount);
    Console(Switch **switches, int switchesCount, Indicator **indicators, int indicatorsCount);
    void setup();
    void update();
    void addSwitch(Switch *);
    void addController(Controller *);
    void addIndicator(Indicator *indicator);
    void addDisplay(GenericDisplay *display);
    int getSwitchesCount();
    int getIndicatorsCount();
    int getControllersCount();
    int getDisplaysCount();
    Indicator *getIndicator(int num);
    Switch *getSwitch(int num);
    Controller *getController(int num);
    GenericDisplay *getDisplay(int num);
    void turnOffIndicators();
  private:
    int switchesCount = 0;
    int indicatorsCount = 0;
    int controllersCount = 0;
    int displaysCount = 0;
    Switch *switches[CONSOLE_MAX_SWITCHES]; // bufor
    Indicator *indicators[CONSOLE_MAX_INDICATORS]; // bufor
    Controller *controllers[CONSOLE_MAX_REGULATORS]; // bufor
    GenericDisplay *displays[CONSOLE_MAX_DISPLAYS]; // bufor
    unsigned long lastUpdate = 0;
    bool initialized = false;
};

#endif
