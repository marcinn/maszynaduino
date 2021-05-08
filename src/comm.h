#ifndef MASZYNADUINO_COMM_H
#define MASZYNADUINO_COMM_H

#include "Arduino.h"


class BaseConsole {
    public:
        BaseConsole();
        virtual void update();
        virtual void transmit();
        bool isInitialized();
    protected:
        bool initialized = false;
};


struct __attribute__((packed))  InputFrame {         // bajt
    uint8_t preamble[4];       // 0-3
    uint16_t tacho;             // 4-5
    uint8_t indicator0;        // 6
    uint8_t indicator1;        // 7
    uint8_t indicator2;        // 8
    uint8_t indicator3;        // 9
    uint8_t indicator4;        // 10
    uint16_t break_pressure;    // 11-12
    uint16_t pipe_pressure;     // 13-14
    uint16_t tank_pressure;     // 15-16
    uint16_t hv_voltage;        // 17-18
    uint16_t hv_current1;       // 19-20
    uint16_t hv_current2;       // 21-22
    uint16_t hv_current3;       // 23-24
    uint16_t year_month;        // 25-26
    uint16_t day_hour_minute;   // 27-28
    uint16_t second_milisecond; // 29-30
    long odometer;          // 31-34
    uint16_t lv_voltage;        // 35-36
    uint8_t radio_channel;     // 37
    uint8_t unused[14];        // 38-51
};

struct __attribute__((packed))  OutputFrame {
    uint8_t preamble[4];       // 0-3
    uint8_t switch0;
    uint8_t switch1;
    uint8_t switch2;
    uint8_t switch3;
    uint8_t switch4;
    uint8_t switch5;
    uint8_t master_controller;
    uint8_t second_controller;
    uint16_t train_brake;
    uint16_t independent_brake;
    uint8_t unused[4];
};


/*
class MaSzynaUART {
    public:
        MaSzynaUART(HardwareSerial *serial, int baud=57600);
        void receive(InputFrame *);
        void send(OutputFrame *);

    private:
        InputFrame tempInput;
}
*/
#define MAX_CONSOLES 10


class Transmitter {
    public:
        void registerConsole(BaseConsole *);
        void transmitNextConsole();
        bool isTransmissionActive();
        uint8_t getRegisteredConsolesNumber();
    private:
        uint8_t consolesNum = 0;
        uint8_t currentConsole = 0;
        bool transmissionActive = false;
        BaseConsole *consoles[MAX_CONSOLES] = {};
};

extern Transmitter MaszynaTransmitter;

#endif
