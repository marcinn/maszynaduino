#include "Arduino.h"
#include "debugmonitor.h"
#include "comm.h"
#include "mux.h"
#include "version.h"

extern uint32_t _MUX_STATE;
extern uint8_t _MUX_CURRENT;
extern uint8_t _MUX_COUNT;
extern uint8_t _MUX_CURRENT_CHANNEL;
extern Mux* _MUXERS[];

DebugMonitor::DebugMonitor(HardwareSerial *debugSerial, Transmitter *transmitter, Console *console) {
    this->serial = debugSerial;
    this->console = console;
    this->transmitter = transmitter;
}

void DebugMonitor::setup() {
    this->serial->begin(500000);
}

void DebugMonitor::log(const String &s) {
    if(this->serial && this->serial->availableForWrite()) {
        this->serial->println(s);
    }
}

void DebugMonitor::print(const String &s) { serial->print(s); }
void DebugMonitor::println(const String &s) { serial->println(s); }
void DebugMonitor::print(int s, int m) { serial->print(s, m); }
void DebugMonitor::println(int s, int m) { serial->println(s, m); }
void DebugMonitor::print(bool s, int m) { serial->print(s, m); }
void DebugMonitor::println(bool s, int m) { serial->println(s, m); }
void DebugMonitor::println() { serial->println(); }

bool DebugMonitor::isDataChanged() {
    /*
    bool c1 = memcmp(&previousInput, console->getInputs(), sizeof(previousInput));
    bool c2 = memcmp(&previousOutput, console->getOutputs(), sizeof(previousOutput));
    */
    unsigned long second = millis() / 100;
    if(second != this->timeCounter) {
        this->timeCounter = second;
        return true;
    } else {
        return false;
    }
}

void DebugMonitor::update() {
}

void DebugMonitor::transmit() {
    if(!isDataChanged()) {
        return;
    }

    if(!this->serial || !this->serial->availableForWrite()) {
        return;
    }

    // TODO: Maszyna ref should be changeable
    //MaszynaState *state = transmitter->getState();
    MaszynaState *state = Maszyna;

    InputFrame *inputs = state->getInputs();
    OutputFrame *outputs = state->getOutputs();

    this->clearScreen();

    serial->print("Maszynaduino ");
    serial->print(MASZYNADUINO_VERSION_STR);
    serial->print(" Monitor               Uptime: ");
    serial->print(millis() / 1000);
    serial->println("s");
    serial->println("----------------------------------------------------------------");
    serial->println();

    serial->println("[Transmitter]");
    transmitter->debugMonitor(serial);  // does not work, dunno why

    serial->print("RX: ");
    for(int i=0;i<sizeof(InputFrame);i++) {
        serial->print(((uint8_t *) inputs)[i], HEX);
        if(i>0 && (i % (sizeof(InputFrame)/2)) == 0) {
            serial->println();
            serial->print("    ");
        } else {
            serial->print(" ");
        }
    }
    serial->println();
    serial->print("TX: ");
    for(int i=0;i<sizeof(OutputFrame);i++) {
        serial->print(((uint8_t *) outputs)[i], HEX);
        serial->print(" ");
    }
    serial->println();
    serial->println();

    serial->println("[Muxers]");
    serial->print("Count: ");
    serial->print(_MUX_COUNT);
    serial->print("  NO: "); // Number of muxes
    serial->print(_MUX_CURRENT);
    serial->print("  CH: ");  // Current channel
    serial->print(_MUX_CURRENT_CHANNEL, HEX);
    serial->print(" IS: "); // Internal state
    printBits(_MUX_STATE, 32);
    serial->println();
    for(int i=0;i<MAX_MUXERS;i++) {
        Mux *mux = _MUXERS[i];
        if(mux) {
            serial->print(" #");
            serial->print(i);
            serial->print(": ");
            printBits(_MUXERS[i]->getState(), 32);
            serial->println();
        }
    }
    serial->println();

    serial->println("[Console]");
    serial->print("Indicators: ");
    serial->print(console->getIndicatorsCount());
    serial->print("  Switches: ");
    serial->println(console->getSwitchesCount());
    serial->println();

    serial->println("Inputs");
    serial->println("-------------------------------------------");
    serial->print("I0: ");
    printBits(inputs->indicator0, 8);
    serial->print(" I1: ");
    printBits(inputs->indicator1, 8);
    serial->print(" I2: ");
    printBits(inputs->indicator2, 8);
    serial->print(" I3: ");
    printBits(inputs->indicator3, 8);
    serial->print(" I4: ");
    printBits(inputs->indicator4, 8);
    serial->println();
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
    serial->print(inputs->hv_current3);
    serial->println();
    serial->print("SPD: ");
    serial->print(inputs->tacho);
    serial->print(" ODO: ");
    serial->print(inputs->odometer);
    serial->print(" LVV: ");
    serial->print(inputs->lv_voltage);
    serial->print(" RCH: ");
    serial->print(inputs->radio_channel);
    serial->println("");
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

void DebugMonitor::clearScreen() {
    serial->write(27);       // ESC command
    serial->print("[2J");    // clear screen command
    serial->write(27);
    serial->print("[H");     // cursor to home command
}

/* based on: http://engineeringnotes.blogspot.com/2016/10/printbits-routine.html */
void DebugMonitor::printBits(uint32_t n, int numBits) {
    char b;
    for (byte i = 0; i < numBits; i++) {
        // shift 1 and mask to identify each bit value
        b = (n & (1 << (numBits - 1 - i))) > 0 ? '1' : '0'; // slightly faster to print chars than ints (saves conversion)
        this->serial->print(b);
    }
}
