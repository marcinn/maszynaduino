#include "Arduino.h"
#include "consoledebug.h"
#include "comm.h"

ConsoleDebug::ConsoleDebug(Console *console, HardwareSerial *debugSerial) {
    this->serial = debugSerial;
    this->console = console;
}

void ConsoleDebug::setup() {
    this->serial->begin(57600);
}

bool ConsoleDebug::isDataChanged() {
    bool c1 = memcmp(&previousInput, console->getInputs(), sizeof(previousInput));
    bool c2 = memcmp(&previousOutput, console->getOutputs(), sizeof(previousOutput));
    return (c1 != 0 || c2 !=0);
}

void ConsoleDebug::send() {
    if(!isDataChanged()) {
        return;
    }

    const OutputFrame *outputs = console->getOutputs();
    const InputFrame *inputs = console->getInputs();

    memcpy(&previousInput, inputs, sizeof(previousInput));
    memcpy(&previousOutput, outputs, sizeof(previousOutput));

    this->clearScreen();

    serial->print("[SETUP] Indicators count: ");
    serial->print(console->getIndicatorsCount());
    serial->print(" Switches count: ");
    serial->println(console->getSwitchesCount());
    serial->println();

    serial->println("Inputs");
    serial->println("-------------------------------------------");
    serial->print("I0: ");
    serial->print(inputs->indicator0, BIN);
    serial->print(" I1: ");
    serial->print(inputs->indicator1, BIN);
    serial->print(" I2: ");
    serial->print(inputs->indicator2, BIN);
    serial->print(" I3: ");
    serial->print(inputs->indicator3, BIN);
    serial->print(" I4: ");
    serial->println(inputs->indicator4, BIN);
    serial->print("BP: ");
    serial->print(inputs->break_pressure);
    serial->print(" PP: ");
    serial->print(inputs->pipe_pressure);
    serial->print(" TP: ");
    serial->print(inputs->tank_pressure);
    serial->print(" HVV: ");
    serial->print(inputs->hv_voltage);
    serial->print(" HC1: ");
    serial->print(inputs->hv_current1);
    serial->print(" HC2: ");
    serial->print(inputs->hv_current2);
    serial->print(" HC3: ");
    serial->println(inputs->hv_current3);
    serial->println("");
    serial->println("Outputs");
    serial->println("-------------------------------------------");
    serial->print("SW0: ");
    printBits(outputs->switch0, 8);
    serial->print(" SW1: ");
    printBits(outputs->switch1, 8);
    serial->print(" SW2: ");
    printBits(outputs->switch2, 8);
    serial->print(" SW3: ");
    printBits(outputs->switch3, 8);
    serial->print(" SW4: ");
    printBits(outputs->switch4, 8);
    serial->print(" SW5: ");
    printBits(outputs->switch5, 8);
    serial->println("");
    serial->print("MasterC: ");
    serial->print(outputs->master_controller);
    serial->print(" SecondC: ");
    serial->print(outputs->second_controller);
    serial->print(" TBrake: ");
    serial->print(outputs->train_brake);
    serial->print(" IBrake: ");
    serial->println(outputs->independent_brake);

}

void ConsoleDebug::clearScreen() {
    serial->write(27);       // ESC command
    serial->print("[2J");    // clear screen command
    serial->write(27);
    serial->print("[H");     // cursor to home command
}

/* based on: http://engineeringnotes.blogspot.com/2016/10/printbits-routine.html */
void ConsoleDebug::printBits(long int n, int numBits) {
    char b;
    for (byte i = 0; i < numBits; i++) {
        // shift 1 and mask to identify each bit value
        b = (n & (1 << (numBits - 1 - i))) > 0 ? '1' : '0'; // slightly faster to print chars than ints (saves conversion)
        this->serial->print(b);
    }
}
