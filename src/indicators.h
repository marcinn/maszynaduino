#ifndef MASZYNADUINO_INDICATORS_H
#define MASZYNADUINO_INDICATORS_H

#include "Arduino.h"
#include "comm.h"
#include "mux.h"
#include "outputs.h"

class DebugMonitor;


enum class Alert {
    epbrake_enabled = 0,              // indicator0
    ventilator_overload,
    motor_overload_threshold,
    _ind3,
    _ind4,
    _ind5,
    _ind6,
    _ind7,
    coupled_hv_voltage_relays, // indicator1
    boczniki1,
    boczniki2,
    boczniki3,
    boczniki4,
    radio_enabled,
    compressor_enabled,
    fuel_pump_off,
    train_heating,            // indicator2
    motor_resistors,
    wheel_slip,
    _ind19,
    _ind20,
    _ind21,
    alerter,
    shp,
    motor_connectors,        // indicator3; otwarte styczniki liniowe
    _ind25,
    converter_overload,
    ground_relay,
    motor_overload,
    line_breaker,       // wylacznik szybki
    compressor_overload,
    _ind31,
    _ind32,
    _ind33,
    trainstate_cab,
    recorder_braking,
    recorder_power,
    radio_stop,
    springbrake_active,
    alerter_sound,
    UNUSED = -1
};


class Indicator {
    public:
        Indicator(IOutput *output, int pin, Alert indicatorNumber);
        void update(MaszynaState *state);
        void setup();
        void reset();
        void respond();
        Alert getAlert();
        uint8_t getAlertNumber();
        bool getState();

    protected:
        bool readState(MaszynaState *state);
        Alert alert;
        bool state = false;
        bool invert = false;
        int pin;
        IOutput *output;
};

#endif
