#include "Arduino.h"
#include "console.h"
#include "comm.h"
#include "mux.h"


Console::Console(Switch **switches, int switchesCount) {
    for (int i = 0; i < switchesCount; i++) {
        this->addSwitch(switches[i]);
    }
}

Console::Console(Switch **switches, int switchesCount, Indicator **indicators, int indicatorsCount) : Console(switches, switchesCount) {
    for (int i=0; i<indicatorsCount; i++) {
        this->addIndicator(indicators[i]);
    }
}

void Console::setup() {
    for (int i = 0; i < this->switchesCount; i++) {
        this->switches[i]->setup();
    }
    for (int i = 0; i < this->indicatorsCount; i++) {
        this->indicators[i]->setup();
    }
    for (int i = 0; i < this->controllersCount; i++) {
        this->controllers[i]->setup();
    }
    for (int i = 0; i < this->displaysCount; i++) {
        this->displays[i]->setup();
    }
    initialized = true;
}

void Console::update() {
    if(!initialized) {
        this->setup();
    }
    if ((millis() - lastUpdate) < 50) {
        return;
    }
    lastUpdate = millis();
    for (int i = 0; i < this->switchesCount; i++) {
        this->switches[i]->update();
        this->switches[i]->respond(Maszyna);
    }
    for (int i = 0; i < this->indicatorsCount; i++) {
        this->indicators[i]->update(Maszyna);
        this->indicators[i]->respond();
    }
    for (int i = 0; i < this->displaysCount; i++) {
        this->displays[i]->update(Maszyna);
        this->displays[i]->respond();
    }
    for (int i = 0; i < this->controllersCount; i++) {
        this->controllers[i]->update();
        this->controllers[i]->respond(Maszyna);
    }
}

void Console::addSwitch(Switch *_switch) {
    this->switches[this->switchesCount] = _switch;
    this->switchesCount++;
}

void Console::addController(Controller *_reg) {
    this->controllers[this->controllersCount] = _reg;
    this->controllersCount++;
}

void Console::addIndicator(Indicator *indicator) {
    this->indicators[this->indicatorsCount] = indicator;
    this->indicatorsCount++;
}

void Console::addDisplay(GenericDisplay *_reg) {
    this->displays[this->displaysCount] = _reg;
    this->displaysCount++;
}

int Console::getSwitchesCount() {
    return this->switchesCount;
}

int Console::getDisplaysCount() {
    return this->displaysCount;
}

int Console::getIndicatorsCount() {
    return this->indicatorsCount;
}

int Console::getControllersCount() {
    return this->controllersCount;
}

Indicator *Console::getIndicator(int num) {
    if(num<indicatorsCount) {
        return this->indicators[num];
    } else {
        return nullptr;
    }
}

Controller *Console::getController(int num) {
    if(num<controllersCount) {
        return this->controllers[num];
    } else {
        return nullptr;
    }
}

GenericDisplay *Console::getDisplay(int num) {
    if(num<displaysCount) {
        return this->displays[num];
    } else {
        return nullptr;
    }
}

Switch *Console::getSwitch(int num) {
    if(num<switchesCount) {
        return this->switches[num];
    } else {
        return nullptr;
    }
}

void Console::turnOffIndicators() {
    for (int i = 0; i < this->indicatorsCount; i++) {
        Maszyna->setIndicatorState(this->indicators[i]->getAlert(), false);
        this->indicators[i]->reset();
        this->indicators[i]->respond();
    }
}
