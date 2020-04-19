#pragma once

#include <JuceHeader.h>

struct DatabaseIdentifiers {
    static const Identifier DATABASE_TYPE_ID;

    static const Identifier MASTER_GAIN;
};

const Identifier DatabaseIdentifiers::DATABASE_TYPE_ID {"NaepenStateDatabase"};

const Identifier DatabaseIdentifiers::MASTER_GAIN {"MasterGain"};