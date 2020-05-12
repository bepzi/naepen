#include "OscillatorComponent.h"

#include "dsp/SvfFilter.h"

#include <JuceHeader.h>

static const int TEXT_HEIGHT = 24;

OscillatorComponent::OscillatorComponent(
    const String &oscillator_name, APVTS &state, const Identifier &waveform_id,
    const String &detune_semitones_id, const String &detune_cents_id, const String &pan_id,
    const String &gain_attack_id, const String &gain_decay_id, const String &gain_sustain_id,
    const String &gain_release_id, const String &filter_type_id, const String &filter_enabled_id,
    const String &filter_cutoff_id, const String &filter_q_id) :
    oscillator_name_label("", oscillator_name),

    state(state),
    filter_type_id(filter_type_id),

    waveform_selector_model(std::make_unique<WaveformSelectorListBoxModel>(state, waveform_id)),
    waveform_selector("", waveform_selector_model.get()),

    detune_semitones_slider_attachment(state, detune_semitones_id, detune_semitones_slider),
    detune_cents_slider_attachment(state, detune_cents_id, detune_cents_slider),

    pan_slider_attachment(state, pan_id, pan_slider),

    gain_attack_slider_attachment(state, gain_attack_id, gain_attack_slider),
    gain_decay_slider_attachment(state, gain_decay_id, gain_decay_slider),
    gain_sustain_slider_attachment(state, gain_sustain_id, gain_sustain_slider),
    gain_release_slider_attachment(state, gain_release_id, gain_release_slider),

    filter_enabled_button_attachment(state, filter_enabled_id, filter_enabled_button),
    filter_cutoff_slider_attachment(state, filter_cutoff_id, filter_cutoff_slider),
    filter_q_slider_attachment(state, filter_q_id, filter_q_slider)
{
    addAndMakeVisible(oscillator_name_label);

    waveform_selector.updateContent();

    // Properly select the correct waveform on startup
    int selected_waveform_idx = 0;
    const String &selected_waveform_id =
        state.state.getProperty(waveform_id, waveform_selector_model->waveforms[0].first)
            .toString();
    for (; selected_waveform_idx < (int)waveform_selector_model->waveforms.size();
         ++selected_waveform_idx) {
        if (waveform_selector_model->waveforms[selected_waveform_idx].first ==
            selected_waveform_id) {
            break;
        }

        jassert(selected_waveform_idx != (int)waveform_selector_model->waveforms.size() - 1);
    }
    waveform_selector.selectRow(selected_waveform_idx);

    addAndMakeVisible(waveform_selector);

    addAndMakeVisible(detune_semitones_slider);
    addAndMakeVisible(detune_cents_slider);
    detune_semitones_label.setJustificationType(Justification::centred);
    detune_cents_label.setJustificationType(Justification::centred);
    addAndMakeVisible(detune_semitones_label);
    addAndMakeVisible(detune_cents_label);

    addAndMakeVisible(pan_slider);
    pan_label.setJustificationType(Justification::centred);
    addAndMakeVisible(pan_label);

    addAndMakeVisible(gain_attack_slider);
    addAndMakeVisible(gain_decay_slider);
    addAndMakeVisible(gain_sustain_slider);
    addAndMakeVisible(gain_release_slider);
    gain_attack_label.setJustificationType(Justification::centred);
    gain_decay_label.setJustificationType(Justification::centred);
    gain_sustain_label.setJustificationType(Justification::centred);
    gain_release_label.setJustificationType(Justification::centred);
    addAndMakeVisible(gain_attack_label);
    addAndMakeVisible(gain_decay_label);
    addAndMakeVisible(gain_sustain_label);
    addAndMakeVisible(gain_release_label);

    // TODO: Writing out the filter variants here is a bit hackish, redo this
    filter_type_combo_box.addItemList(
        {"Lowpass", "Highpass", "Bandpass"}, (int)SvfFilter::Type::LOWPASS);
    int selected_filter_id = state.state.getProperty(filter_type_id, (int)SvfFilter::Type::LOWPASS);
    filter_type_combo_box.setSelectedId(selected_filter_id, dontSendNotification);
    filter_type_combo_box.addListener(this);

    addAndMakeVisible(filter_type_combo_box);
    addAndMakeVisible(filter_enabled_button);
    addAndMakeVisible(filter_cutoff_slider);
    addAndMakeVisible(filter_q_slider);
    filter_cutoff_label.setJustificationType(Justification::centred);
    filter_q_label.setJustificationType(Justification::centred);
    addAndMakeVisible(filter_cutoff_label);
    addAndMakeVisible(filter_q_label);
}

void OscillatorComponent::paint(Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    g.setColour(Colour(NaepenLookAndFeel::widget_background_argb));
    g.fillRect(oscillator_name_label.getBounds());

    g.setColour(Colour(NaepenLookAndFeel::outline_argb));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 8, 3);
}

void OscillatorComponent::resized()
{
    auto area = getLocalBounds();
    oscillator_name_label.setBounds(area.removeFromTop(TEXT_HEIGHT));

    auto row_height = area.getHeight() / 3;

    auto top_row = area.removeFromTop(row_height).reduced(4, 4);
    {
        waveform_selector.setBounds(top_row.removeFromLeft(top_row.getWidth() / 3));

        auto width = top_row.getWidth() / 3;

        auto region = top_row.removeFromLeft(width);
        pan_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        pan_slider.setBounds(region);

        region = top_row.removeFromLeft(width);
        detune_semitones_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        detune_semitones_slider.setBounds(region);

        region = top_row;
        detune_cents_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        detune_cents_slider.setBounds(region);
    }

    auto middle_row = area.removeFromTop(row_height).reduced(4, 4);
    {
        auto width = middle_row.getWidth() / 4;

        auto region = middle_row.removeFromLeft(width);
        filter_type_combo_box.setBounds(region.removeFromTop(region.getHeight() / 2));
        filter_enabled_button.setBounds(region);

        region = middle_row.removeFromLeft(width);
        filter_cutoff_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        filter_cutoff_slider.setBounds(region);

        region = middle_row.removeFromLeft(width);
        filter_q_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        filter_q_slider.setBounds(region);
    }

    auto bottom_row = area.reduced(4, 4);
    {
        auto width = bottom_row.getWidth() / 4;

        auto region = bottom_row.removeFromLeft(width);
        gain_attack_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        gain_attack_slider.setBounds(region);

        region = bottom_row.removeFromLeft(width);
        gain_decay_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        gain_decay_slider.setBounds(region);

        region = bottom_row.removeFromLeft(width);
        gain_sustain_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        gain_sustain_slider.setBounds(region);

        region = bottom_row.removeFromLeft(width);
        gain_release_label.setBounds(region.removeFromBottom(TEXT_HEIGHT));
        gain_release_slider.setBounds(region);
    }
}

void OscillatorComponent::comboBoxChanged(ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged != &filter_type_combo_box) {
        return;
    }

    int selected_filter_type = filter_type_combo_box.getSelectedId();
    jassert(selected_filter_type > 0);

    state.state.setProperty(filter_type_id, selected_filter_type, nullptr);
}
