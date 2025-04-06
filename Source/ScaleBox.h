#ifndef SCALEBOX_H
#define SCALEBOX_H

#include <JuceHeader.h>

struct ScaleBox : public juce::Component
{
    ScaleBox();
    ~ScaleBox();
    
    void paint(Graphics& g) override;
    void resized() override;
    
    juce::TextButton openSclButton;
    juce::TextButton openKbmButton;
    juce::TextButton openEditorButton;
    juce::Label scaleLabel;
    juce::Label sclNameLabel;
    juce::Label kbmNameLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScaleBox)
};

#endif // SCALEBOX_H
