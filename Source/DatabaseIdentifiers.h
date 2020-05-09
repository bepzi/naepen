#pragma once

#include <JuceHeader.h>

struct DatabaseIdentifiers {
    static const Identifier DATABASE_TYPE_ID;

    static const Identifier MASTER_GAIN;

    // Parameters for Oscillator 1
    // ====================================================
    static const Identifier OSC_ONE_GROUP;

    static const Identifier OSC_ONE_WAVEFORM;

    static const Identifier OSC_ONE_PAN;

    static const Identifier OSC_ONE_GAIN;
    static const Identifier OSC_ONE_GAIN_ATTACK;
    static const Identifier OSC_ONE_GAIN_DECAY;
    static const Identifier OSC_ONE_GAIN_SUSTAIN;
    static const Identifier OSC_ONE_GAIN_RELEASE;

    static const Identifier OSC_ONE_FILTER_TYPE;
    static const Identifier OSC_ONE_FILTER_ENABLED;
    static const Identifier OSC_ONE_FILTER_CUTOFF;
    static const Identifier OSC_ONE_FILTER_Q;

    // Parameters for Oscillator 2
    // ====================================================
    static const Identifier OSC_TWO_GROUP;

    static const Identifier OSC_TWO_WAVEFORM;

    static const Identifier OSC_TWO_PAN;

    static const Identifier OSC_TWO_GAIN;
    static const Identifier OSC_TWO_GAIN_ATTACK;
    static const Identifier OSC_TWO_GAIN_DECAY;
    static const Identifier OSC_TWO_GAIN_SUSTAIN;
    static const Identifier OSC_TWO_GAIN_RELEASE;

    static const Identifier OSC_TWO_FILTER_TYPE;
    static const Identifier OSC_TWO_FILTER_ENABLED;
    static const Identifier OSC_TWO_FILTER_CUTOFF;
    static const Identifier OSC_TWO_FILTER_Q;
};
