#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "BgfxComponent.h"

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        addAndMakeVisible(bgfxView);
        setSize(1024, 768);
    }

    void resized() override
    {
        bgfxView.setBounds(getLocalBounds());
    }

private:
    BgfxComponent bgfxView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
