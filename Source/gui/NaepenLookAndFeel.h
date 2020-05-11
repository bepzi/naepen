#pragma once

#include <JuceHeader.h>

class NaepenLookAndFeel : public LookAndFeel_V4 {
public:
    NaepenLookAndFeel()
    {
        ColourScheme scheme(
            window_background_argb, widget_background_argb, menu_background_argb, outline_argb,
            default_text_argb, default_fill_argb, highlighted_text_argb, highlighted_fill_argb,
            menu_text_argb);
        setColourScheme(scheme);

        auto font_typeface = Typeface::createSystemTypefaceFor(
            BinaryData::TruenoRg_otf, BinaryData::TruenoRg_otfSize);
        jassert(font_typeface->getName().isNotEmpty());
        setDefaultSansSerifTypeface(font_typeface);

        setDefaultLookAndFeel(this);
    }

    void drawLabel(Graphics &g, Label &label) override
    {
        g.fillAll(label.findColour(Label::backgroundColourId));

        if (!label.isBeingEdited()) {
            auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font(getLabelFont(label));

            g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
            g.setFont(font);

            auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

            g.drawFittedText(
                label.getText(), textArea, label.getJustificationType(),
                jmax(1, (int)(textArea.getHeight() / font.getHeight())),
                label.getMinimumHorizontalScale());

            g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
        } else if (label.isEnabled()) {
            g.setColour(label.findColour(Label::outlineColourId));
        }

        g.drawRoundedRectangle(label.getLocalBounds().toFloat(), 8.0f, 1.0f);
    }

    void drawRotarySlider(
        Graphics &g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider &slider) override
    {
        auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(4.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        Path backgroundArc;
        backgroundArc.addCentredArc(
            bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f, rotaryStartAngle,
            rotaryEndAngle, true);

        g.setColour(outline);
        g.strokePath(
            backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

        if (slider.isEnabled()) {
            Path valueArc;
            valueArc.addCentredArc(
                bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
                rotaryStartAngle, toAngle, true);

            g.setColour(fill);
            g.strokePath(
                valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        auto thumbWidth = lineW * 2.0f;
        Point<float> thumbPoint(
            bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
            bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

        g.setColour(slider.findColour(Slider::thumbColourId));
        g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    }

    static constexpr uint32 black = 0xff282828;
    static constexpr uint32 white = 0xffe8e8e8;

    // highlighted_fill should be slightly darker than widget_background
    // highlighted_text should be black or white, heavy contrast with highlighted_fill

    static constexpr uint32 window_background_argb = 0xff0b132b;

    // Slider "unselected portion", background of ComboBox and ListItemMenu
    static constexpr uint32 widget_background_argb = 0xff1c2541;
    static constexpr uint32 default_text_argb = white;

    // Knobs and scrollbars
    static constexpr uint32 default_fill_argb = 0xff5bc0be;
    static constexpr uint32 highlighted_text_argb = white;

    // Slider "selected portion", hover over ComboBox item
    static constexpr uint32 highlighted_fill_argb = 0xff3a506b;

    static constexpr uint32 menu_background_argb = window_background_argb;
    static constexpr uint32 outline_argb = default_text_argb;
    static constexpr uint32 menu_text_argb = default_text_argb;
};
