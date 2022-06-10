#include "inputs.h"


int ArduinoDigitalInput::read(int pin) {
    return digitalRead(pin);
}

void ArduinoDigitalInput::setup(int pin, InputMode mode) {
    pinMode(pin, mode == InputMode::PULLUP ? INPUT_PULLUP : INPUT);
}

MCP23017Input::MCP23017Input(int address) {
    expander = new MCP23017(address);
}

int MCP23017Input::read(int pin) {
    MCP23017Register reg = (
        pin < 8 ? MCP23017Register::GPIO_A : MCP23017Register::GPIO_B
    );
    return expander->readRegister(reg) & (1 << (pin % 8));
}

void MCP23017Input::setup(int pin, InputMode mode) {
    uint8_t port = (pin / 8);
    uint8_t iodir = expander->readRegister(port ? MCP23017Register::IODIR_B : MCP23017Register::IODIR_A);
    uint8_t gppu = expander->readRegister(port ? MCP23017Register::GPPU_B : MCP23017Register::GPPU_A);
    uint8_t ipol = expander->readRegister(port ? MCP23017Register::IPOL_B : MCP23017Register::IPOL_A);
    uint8_t _pinmask = 1 << (pin % 8);

    expander->portMode(
        port ? MCP23017Port::B : MCP23017Port::A,
        iodir | _pinmask,
        mode == InputMode::PULLUP ? gppu | _pinmask : gppu & ~_pinmask
    );
}
