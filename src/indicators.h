#ifndef MASZYNADUINO_INDICATORS_H
#define MASZYNADUINO_INDICATORS_H

#include "Arduino.h"
#include "comm.h"
#include "mux.h"

class DebugMonitor;


enum Alert {
    _ind0 = 0,              // indicator0
    ventilator_overload,
    motor_overload_threshold,
    _ind3,
    _ind4,
    _ind5,
    _ind6,
    _ind7,
    coupled_hv_voltage_relays, // indicator1
    _ind9,
    _ind10,
    _ind11,
    _ind12,
    _ind13,
    _ind14,
    _ind15,
    train_heating,            // indicator2
    motor_resistors,
    wheel_slip,
    _ind19,
    _ind20,
    _ind21,
    alerter,
    shp,
    motor_connectors,        // indicator3
    _ind25,
    converter_overload,
    ground_relay,
    motor_overload,
    line_breaker,
    compressor_overload,
    _ind31,
    _ind32,
    _ind33,
    trainstate_cab,
    recorder_braking,
    recorder_power,
    radio_stop,
    springbrake_active,
    alerter_sound
};

const String _UNKNOWN_ALERT = "???";

const String AlertNames[] = {
    _UNKNOWN_ALERT,
    "VOV",
    "MOT",
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    "CVR",
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    "TH ",
    "MR ",
    "WS ",
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    "CA ",
    "SHP",
    "MC ",
    _UNKNOWN_ALERT,
    "COO",
    "GR ",
    "MO ",
    "LB ",
    "CO ",
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    _UNKNOWN_ALERT,
    "TC ",
    "REB",
    "REP",
    "RS",
    "SBA",
    "CAS",
};


class Indicator {
    public:
        void update(MaszynaState *state);
        virtual void setup();
        virtual void respond();
        void debugMonitor(DebugMonitor *);

    protected:
        virtual bool readState(MaszynaState *state);
        int indicatorNumber;
        bool state = false;
        bool invert = false;
};


class PinIndicator : public Indicator {
    public:
        PinIndicator(int pin, int indicatorNumber);
        virtual void setup();
        virtual void respond();
    private:
        int pin;
};


class MuxIndicator : public Indicator {
    public:
        MuxIndicator(Mux *mux, int channel, int indicatorNumber);
        virtual void setup();
        virtual void respond();
    private:
        Mux *mux;
        int channel;
};

#endif
