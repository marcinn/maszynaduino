#ifndef MASZYNADUINO_CONSOLEDEBUG_H
#define MASZYNADUINO_CONSOLEDEBUG_H


#include "Arduino.h"
#include "console.h"
#include "comm.h"


#define DEBUG_CMDBUFSIZE  5


struct __attribute__((packed))  DebugFrame {
    uint32_t uptime;
	uint8_t trn_flags;
    uint8_t trn_serial_avail = 0;
    InputFrame tr_rx;
    OutputFrame tr_tx;
    char muxterminator[4] = {'M', 'U', 'X', '!'};
    uint8_t mux_count;
	uint32_t mux_calc_time = 0;
    uint32_t mux_state[MAX_MUXERS];
    uint32_t mux_dir[MAX_MUXERS];
    char consoleterminator[4] = {'C', 'O', 'N', '!'};
    uint8_t console_count;
    uint8_t max_consoles = MAX_CONSOLES;
    uint8_t max_switches = CONSOLE_MAX_SWITCHES;
    uint8_t max_indicators = CONSOLE_MAX_INDICATORS;
    uint8_t console_switches_count[MAX_CONSOLES];
    uint8_t console_switches[MAX_CONSOLES][CONSOLE_MAX_SWITCHES];
    uint8_t console_switches_state[MAX_CONSOLES][CONSOLE_MAX_SWITCHES];
    uint8_t console_indicators_count[MAX_CONSOLES];
    uint8_t console_indicators[MAX_CONSOLES][CONSOLE_MAX_INDICATORS];
    uint8_t console_indicators_state[MAX_CONSOLES][CONSOLE_MAX_INDICATORS];
    char terminator[4] = {'E', 'O', 'N', '!'};
};


class DebugMonitor {
    public:
        DebugMonitor(HardwareSerial *debugSerial, SerialTransmitter *transmitter, Console *console, unsigned long baud = 115200);
        void setup();
        void update();
        void transmit();
        //void addConsole(Console *);
    private:
        unsigned long baud;
        HardwareSerial *serial;
        Console *console;
        SerialTransmitter *transmitter;
        InputFrame previousInput;
        OutputFrame previousOutput;
        unsigned long lastSent = 0;
        unsigned long timeCounter = 0;
        bool initialized = false;
};

#endif
