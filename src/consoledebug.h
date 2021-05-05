#ifndef MASZYNADUINO_CONSOLEDEBUG_H
#define MASZYNADUINO_CONSOLEDEBUG_H


#include "Arduino.h"
#include "console.h"

class ConsoleDebug {
    public:
        ConsoleDebug(Console *console, HardwareSerial *debugSerial);
        void setup();
        void send();
        void clearScreen();
        bool isDataChanged();
    protected:
        void printBits(long int n, int numBits);
    private:
        HardwareSerial *serial;
        Console *console;
        InputFrame previousInput;
        OutputFrame previousOutput;
};

#endif
