#pragma once

#include "dsp/BandlimitedOscillator.h"

#include <JuceHeader.h>

class WaveformSelectorListBoxModel : public ListBoxModel {
public:
    int getNumRows() override;
    void paintListBoxItem(
        int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;

    void add_item(const String &item);

private:
    std::vector<String> waveforms = {};
};