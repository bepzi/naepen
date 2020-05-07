#include "WaveformSelectorListBoxModel.h"

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

    g.drawText(waveforms[rowNumber], area, Justification::centredLeft);
}

/**
 * Remember to call updateContent() on the parent ListBox once you're done adding items.
 */
void WaveformSelectorListBoxModel::add_item(const String &item)
{
    waveforms.push_back(item);
}
