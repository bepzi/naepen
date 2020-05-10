#pragma once

#include "NaepenLookAndFeel.h"
#include "dsp/BandlimitedOscillator.h"

#include <JuceHeader.h>

class WaveformSelectorListBoxModel : public ListBoxModel {
public:
    WaveformSelectorListBoxModel(
        NaepenLookAndFeel &look_and_feel, AudioProcessorValueTreeState &avpts,
        Identifier waveform_id);

    int getNumRows() override;
    void paintListBoxItem(
        int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;

    void selectedRowsChanged(int row) override;

private:
    NaepenLookAndFeel &look_and_feel;

    AudioProcessorValueTreeState &state;

    Identifier waveform_id;

    // list of (human-readable name, database-name)
    std::vector<std::pair<String, String>> waveforms;
};