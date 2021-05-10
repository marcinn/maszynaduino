#include "Arduino.h"

#ifndef MASZYNADUINO_MUX_H
#define MASZYNADUINO_MUX_H


#ifndef MAX_MUXERS
#define MAX_MUXERS 10
#endif

#define MUX_CH0 0
#define MUX_CH1 1
#define MUX_CH2 2
#define MUX_CH3 3
#define MUX_CH4 4
#define MUX_CH5 5
#define MUX_CH6 6
#define MUX_CH7 7
#define MUX_CH8 8
#define MUX_CH9 9
#define MUX_CH10 10
#define MUX_CH11 11
#define MUX_CH12 12
#define MUX_CH13 13
#define MUX_CH14 14
#define MUX_CH15 15
#define MUX_CH16 16

enum MuxChannels {
    CH0=0, CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8,
    CH9, CH10, CH11, CH12, CH13, CH14, CH15, CH16
};

#define MUXDIR_INPUT 0
#define MUXDIR_OUTPUT 1

class DebugMonitor;


enum MuxChannelMode { pullup=0, output };

class Mux {
    public:
        Mux(int pinEnable, int pinData, int pinS0, int pinS1, int pinS2, int pinS3);
        void setup();
        void setChannelMode(uint8_t channel, MuxChannelMode mode);
        void enable();
        void disable();
        bool readDigital();
        int readAnalog();
        void writeDigital(bool state);
        void channel(int channel);
        void setState(uint32_t state);
        MuxChannelMode getChannelMode(int ch);
        uint32_t getState();
        bool getChannelState(int ch);
        bool getRequestedChannelState(int ch);
        void requestChannelState(int ch, bool state);
        void debugMonitor(DebugMonitor *dbg);
        uint8_t channelsCount() { return this->channels; }

        static void initializeTimers();

    private:
        int pinS0, pinS1, pinS2, pinS3;
        int pinEnable, pinData;
        int channels = 16;
        uint32_t state = 0;
        uint32_t requestedState = 0;
        uint32_t channelDirections = 0; // bit 0: input_pullup, bit 1: output
        static bool timersInitialized;
};


#endif
