#pragma once

#include "NaepenLookAndFeel.h"
#include "dsp/BandlimitedOscillator.h"

#include <JuceHeader.h>

class WaveformSelectorListBoxModel : public ListBoxModel {
public:
    WaveformSelectorListBoxModel(AudioProcessorValueTreeState &avpts, Identifier waveform_id);

    int getNumRows() override;
    void paintListBoxItem(
        int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;

    void selectedRowsChanged(int row) override;

    // list of (database name, human-readable name)
    const std::vector<std::pair<String, String>> waveforms;

private:
    AudioProcessorValueTreeState &state;

    Identifier waveform_id;
};