#pragma once

#include "dsp/BandlimitedOscillator.h"

#include <JuceHeader.h>

class WaveformSelectorListBoxModel : public ListBoxModel {
public:
    WaveformSelectorListBoxModel(AudioProcessorValueTreeState &avpts, Identifier waveform_id);

    int getNumRows() override;
    void paintListBoxItem(
        int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;

    void selectedRowsChanged(int row) override;

private:
    AudioProcessorValueTreeState &state;

    Identifier waveform_id;

    // list of (human-readable name, database-name)
    std::vector<std::pair<String, String>> waveforms;
};