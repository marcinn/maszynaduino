#ifndef MASZYNADUINO_CONSOLEDEBUG_H
#define MASZYNADUINO_CONSOLEDEBUG_H


#include "Arduino.h"
#include "console.h"
#include "comm.h"

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
        void println();
    protected:
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
