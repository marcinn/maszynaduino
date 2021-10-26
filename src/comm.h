#ifndef MASZYNADUINO_COMM_H
#define MASZYNADUINO_COMM_H

#include "Arduino.h"

enum class Alert;
enum class ControllerType;

//#define MASZYNADUINO_MASZYNA_UART_SYNC_BUG_WORKAROUND

struct __attribute__((packed))  InputFrame {         // bajt
    uint8_t preamble[4];       // 0-3
    uint16_t tacho;             // 4-5
    uint8_t indicator0;        // 6
    uint8_t indicator1;        // 7
    uint8_t indicator2;        // 8
    uint8_t indicator3;        // 9
    uint8_t indicator4;        // 10
    uint16_t break_pressure;    // 11-12
    uint16_t pipe_pressure;     // 13-14
    uint16_t tank_pressure;     // 15-16
    uint16_t hv_voltage;        // 17-18
    uint16_t hv_current1;       // 19-20
    uint16_t hv_current2;       // 21-22
    uint16_t hv_current3;       // 23-24
    uint16_t year_month;        // 25-26
    uint16_t day_hour_minute;   // 27-28
    uint16_t second_milisecond; // 29-30
    uint32_t odometer;          // 31-34
    uint16_t lv_voltage;        // 35-36
    uint8_t radio_channel;     // 37
    uint8_t unused[14];        // 38-51
};

struct __attribute__((packed))  OutputFrame {
    uint8_t preamble[4];       // 0-3
    uint8_t switch0;
    uint8_t switch1;
    uint8_t switch2;
    uint8_t switch3;
    uint8_t switch4;
    uint8_t switch5;
    uint8_t master_controller;
    uint8_t second_controller;
    uint16_t train_brake;
    uint16_t independent_brake;
    uint8_t unused[4];
};


const int INPUTS_SIZE = sizeof(InputFrame);
const int OUTPUTS_SIZE = sizeof(OutputFrame);


struct s_datetime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

class MaszynaState {
    public:
        InputFrame* getInputs();
        OutputFrame* getOutputs();
        void setInputs(InputFrame *);
        bool getIndicatorState(Alert indicatorNumber);
        bool getIndicatorState(uint8_t indicatorNumber);
        void setOutputBit(uint8_t num, uint8_t bitNum, bool state);
        void setOutputSwitch(uint8_t num, bool state);
        void setIndicatorState(Alert indicatorNum, bool state);
        void setControllerValue(ControllerType, int value);
        bool getOutputSwitch(uint8_t num);
		unsigned long getMuxCalcTime();
		void setMuxCalcTime(unsigned long t);
        s_datetime *getSimDateTime();

    private:
		unsigned long muxCalcTime  = 0;
        InputFrame input = {{0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}};
        OutputFrame output = {{0xEF, 0xEF, 0xEF, 0xEF}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}};
        s_datetime datetime = {0, 0, 0, 0, 0, 0};
};


class Transmitter {
    public:
        Transmitter();
        virtual void transmit();
        bool isTransmissionActive();
        bool isTransmissionStarted();
		bool synced = false;
        MaszynaState *getState();
    protected:
        MaszynaState *state;
        bool transmissionActive = false;
        bool transmissionStarted = false;
};

class SerialTransmitter : public Transmitter {
    public:
        SerialTransmitter(HardwareSerial *serial, unsigned long baud=57600, unsigned int minPause=100);
        void transmit();
        HardwareSerial *getSerial();
        unsigned long getSerialBaud();

    private:
        HardwareSerial *serial;
        unsigned long baud;
        InputFrame tmpBuf = {0};
        bool initialized = false;
        unsigned int minPause = 100;
        unsigned int readPos = 0;
        unsigned long lastSend = 0;
        unsigned long lastRead = 0;
        unsigned long lastUpdate = 0;
        unsigned long lastTransmission = 0;
        byte syncstep = 0;
        bool skipPreamble = false;
};

extern MaszynaState *Maszyna;

#endif
