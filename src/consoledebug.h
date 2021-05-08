#ifndef MASZYNADUINO_CONSOLEDEBUG_H
#define MASZYNADUINO_CONSOLEDEBUG_H


#include "Arduino.h"
#include "console.h"
#include "comm.h"

class ConsoleDebug : public BaseConsole {
    public:
        ConsoleDebug(Console *console, HardwareSerial *debugSerial);
        void setup();
        void update();
        void transmit();
        void clearScreen();
        bool isDataChanged();
        //void addConsole(Console *);
        void log(const String &);
    protected:
        void printBits(uint32_t n, int numBits);
    private:
        HardwareSerial *serial;
        Console *console;
        InputFrame previousInput;
        OutputFrame previousOutput;
        unsigned long timeCounter = 0;
};

#endif
