#pragma once

class GenericDisplay {
    public:
        virtual bool update(MaszynaState *state); // returns true if changed
        virtual void setup();
        virtual void respond();
};
