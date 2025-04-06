#include "ScaleBox.h"

ScaleBox::ScaleBox()
{
    addAndMakeVisible(openSclButton);
    addAndMakeVisible(openKbmButton);
    addAndMakeVisible(openEditorButton);
    addAndMakeVisible(scaleLabel);
    addAndMakeVisible(sclNameLabel);
    addAndMakeVisible(kbmNameLabel);
    scaleLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    scaleLabel.setMinimumHorizontalScale(1.0f);
    openSclButton.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB(0xF7, 0xC4, 0x6C));
    openSclButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    openSclButton.setButtonText ("Open SCL File");
    openKbmButton.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB(0xF7, 0xC4, 0x6C));
    openKbmButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    openKbmButton.setButtonText ("Open KBM File");
    openEditorButton.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB(0xF7, 0xC4, 0x6C));
    openEditorButton.setColour (juce::TextButton::textColourOffId, juce::Colours::black);
    openEditorButton.setButtonText ("Edit");
    sclNameLabel.setFont (juce::FontOptions().withHeight(16));
    sclNameLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    sclNameLabel.setMinimumHorizontalScale(1.0f);
    kbmNameLabel.setFont (juce::FontOptions().withHeight(16));
    kbmNameLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    kbmNameLabel.setMinimumHorizontalScale(1.0f);
}

ScaleBox::~ScaleBox()
{
}

void ScaleBox::paint(Graphics& g)
{
    g.drawRect(getLocalBounds());
}

void ScaleBox::resized()
{
    scaleLabel.setBounds (10, 10, 150, 20);
    openSclButton.setBounds (14, 40, 110, 30);
    openKbmButton.setBounds (14, 75, 110, 30);
    openEditorButton.setBounds (128, 40, 60, 65);
    sclNameLabel.setBounds (10, 115, getWidth() * 0.95f, 20);
    kbmNameLabel.setBounds (10, 140, getWidth() * 0.95f, 20);
}
