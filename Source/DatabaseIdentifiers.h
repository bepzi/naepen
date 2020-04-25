#pragma once

#include <JuceHeader.h>

struct DatabaseIdentifiers {
    static const Identifier DATABASE_TYPE_ID;

    static const Identifier MASTER_GAIN;

    // Parameters for Oscillator 1
    // ====================================================
    static const Identifier OSC_ONE_GROUP;
    static const Identifier OSC_ONE_GAIN_ATTACK;
    static const Identifier OSC_ONE_GAIN_DECAY;
    static const Identifier OSC_ONE_GAIN_SUSTAIN;
    static const Identifier OSC_ONE_GAIN_RELEASE;

    static const Identifier OSC_ONE_FILTER_ENABLED;
    static const Identifier OSC_ONE_FILTER_CUTOFF;
    static const Identifier OSC_ONE_FILTER_Q;

    static const Identifier OSC_ONE_FILTER_ATTACK;
    static const Identifier OSC_ONE_FILTER_DECAY;
    static const Identifier OSC_ONE_FILTER_SUSTAIN;
    static const Identifier OSC_ONE_FILTER_RELEASE;

    // ====================================================
};
