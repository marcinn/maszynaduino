#include "Arduino.h"
#include "debugmonitor.h"
#include "comm.h"
#include "mux.h"
#include "version.h"

extern uint32_t _MUX_STATE;
extern uint8_t _MUX_CURRENT;
extern uint8_t _MUX_COUNT;
extern uint8_t _MUX_CURRENT_CHANNEL;
extern unsigned long _MUX_TIME;
extern Mux* _MUXERS[];

DebugMonitor::DebugMonitor(HardwareSerial *debugSerial, SerialTransmitter *transmitter, Console *console, unsigned long baud) {
    this->serial = debugSerial;
    this->console = console;
    this->transmitter = transmitter;
    this->baud = baud;
}

void DebugMonitor::setup() {
}

void DebugMonitor::update() {
}

void DebugMonitor::transmit() {
    if(!initialized) {
        this->serial->begin(baud);
        initialized = true;
    }
    if(!this->serial || !this->serial->availableForWrite()) {
        return;
    }

    char cmd;

    if((cmd = this->serial->read()) > -1) {
    } else {
        return;
    }

    if (cmd == 'm') {
        // TODO: Maszyna ref should be changeable
        //MaszynaState *state = transmitter->getState();
        MaszynaState *state = Maszyna;

        DebugFrame f;
        InputFrame *inputs = state->getInputs();
        OutputFrame *outputs = state->getOutputs();

        f.uptime = millis();
        f.trn_flags = 1 & transmitter->synced;
        f.trn_serial_avail = transmitter->getSerial()->available();

        memcpy(&f.tr_rx, inputs, sizeof(InputFrame));
        memcpy(&f.tr_tx, outputs, sizeof(OutputFrame));
        f.mux_count = _MUX_COUNT;
        f.mux_calc_time = _MUX_TIME;
        for(int i=0;i<MAX_MUXERS;i++) {
            Mux *mux = _MUXERS[i];
            f.mux_state[i] = mux ? mux->getState() : 0;
            f.mux_dir[i] = mux ? mux->getDirections() : 0;
        }
        f.console_count = 1;
        for(int i=0;i<MAX_CONSOLES;i++) {
            Console *c;
            if(i==0) {
                c = console;
            } else {
                c = nullptr;
            }
            f.console_switches_count[i] = console ? console->getSwitchesCount() : -1;
            f.console_indicators_count[i] = console ? console->getIndicatorsCount() : -1;
            for(int j=0;j<CONSOLE_MAX_SWITCHES;j++) {
                f.console_switches[i][j] = console ? console->getSwitch(j)->getOutputNumber() : -1;
                f.console_switches_state[i][j] = console ? (int) console->getSwitch(j)->getState() : -1;
            }
            for(int j=0;j<CONSOLE_MAX_INDICATORS;j++) {
                Indicator *ind = console ? console->getIndicator(j) : nullptr;
                f.console_indicators[i][j] = ind ? ind->getAlertNumber() : -1;
                f.console_indicators_state[i][j] = ind ? (int) ind->getState() : 0;
            }
        }

        this->serial->write((uint8_t *) &f, sizeof(f));
    }

}
