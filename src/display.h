#pragma once

class GenericDisplay {
    public:
        virtual void update(MaszynaState *state);
        virtual void setup();
        virtual void respond();
};
