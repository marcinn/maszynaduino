#ifndef MASZYNADUINO_CONSOLEDEBUG_H
#define MASZYNADUINO_CONSOLEDEBUG_H


#include "Arduino.h"
#include "console.h"
#include "comm.h"

/*
#define DF_TR_ACTIVE 1

struct __attribute__((packed))  DebugFrame {
    char preamble[4] = "MAS!";
    uint8_t tr_flags;
    uint8_t tr_inbuf;
    uint8_t tr_writebuf;
    InputFrame tr_rx;
    OutputFrame tr_tx;
    uint8_t mux_count;
    uint8_t mux_max = MAX_MUXERS;
    uint32_t mux_state[MAX_MUXERS];
    uint32_t mux_dir[MAX_MUXERS];
    uint8_t con_count;
    uint8_t con_max = MAX_CONSOLES;

}
*/

class DebugMonitor {
    public:
        DebugMonitor(HardwareSerial *debugSerial, Transmitter *transmitter, Console *console);
        void setup();
        void update();
        void transmit();
        void clearScreen();
        bool isDataChanged();
        //void addConsole(Console *);
        void log(const String &);
        void print(const String &);
        void print(int, int mode=DEC);
        void println(int, int mode=DEC);
        void print(bool, int mode=DEC);
        void println(bool, int mode=DEC);
        void println(const String &);
        void print(const char s[]);
        void println(const char s[]);
        void print(char s);
        void println(char s);
        void println();
        void printBits(uint32_t n, int numBits);
    private:
        HardwareSerial *serial;
        Console *console;
        Transmitter *transmitter;
        InputFrame previousInput;
        OutputFrame previousOutput;
        unsigned long timeCounter = 0;
};

#endif
