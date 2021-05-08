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


class Console : public BaseConsole {
  public:
    Console(HardwareSerial *serial);
    Console(HardwareSerial *serial, Switch **switches, int switchesCount);
    Console(HardwareSerial *serial, Switch **switches, int switchesCount, Indicator **indicators, int indicatorsCount);
    void setup();
    void update();
    void transmit();
    void addSwitch(Switch *);
    void addIndicator(Indicator *indicator);
    void setOutputBit(uint8_t num, uint8_t bitNum, bool state);
    void setOutputSwitch(uint8_t num, bool state);
    bool isTransmissionActive();
    bool isTransmissionStarted();
    int getSwitchesCount();
    int getIndicatorsCount();
    int getSerialBaud();
    HardwareSerial *getSerial();
    InputFrame* getInputs();
    OutputFrame* getOutputs();
  private:
    int switchesCount = 0;
    int indicatorsCount = 0;
    int baud = 19200;
    bool transmissionActive = false;
    bool transmissionStarted = false;
    HardwareSerial *serial;
    uint8_t preamble[4] = {0xEF, 0xEF, 0xEF, 0xEF};
    uint8_t tmpBuf[100] = {0};
    struct InputFrame input = {{0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}};
    struct OutputFrame output = {{0xEF, 0xEF, 0xEF, 0xEF}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}};
    Switch *switches[CONSOLE_MAX_SWITCHES]; // bufor
    Indicator *indicators[CONSOLE_MAX_INDICATORS]; // bufor
};

#endif
