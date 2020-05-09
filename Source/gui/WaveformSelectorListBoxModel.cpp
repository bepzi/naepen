#include "WaveformSelectorListBoxModel.h"

#include <utility>

WaveformSelectorListBoxModel::WaveformSelectorListBoxModel(
    AudioProcessorValueTreeState &apvts, Identifier waveform_id) :
    state(apvts),
    waveform_id(std::move(waveform_id)),
    waveforms(
        {{"Sine", "Sine"},
         {"Triangle", "Triangle"},
         {"Square", "Square"},
         {"EngineersSawtooth", "Engineer's Sawtooth"},
         {"WhiteNoise", "White Noise"}})
{
}

int WaveformSelectorListBoxModel::getNumRows()
{
    return waveforms.size();
}

void WaveformSelectorListBoxModel::paintListBoxItem(
    int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
    if (!isPositiveAndBelow(rowNumber, getNumRows())) {
        return;
    }

    Rectangle<int> area = {0, 0, width, height};
    area.reduce(2, 0);
    if (rowIsSelected) {
        g.fillAll(Colours::darkgrey);
        g.setColour(Colours::white);
    } else {
        g.setColour(Colours::lightgrey);
    }

    g.drawText(waveforms[rowNumber].second, area, Justification::centredLeft);
}

void WaveformSelectorListBoxModel::selectedRowsChanged(int row)
{
    if (row < 0) {
        return;
    }

    state.state.setProperty(waveform_id, waveforms[row].first, nullptr);
}
