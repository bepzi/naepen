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
