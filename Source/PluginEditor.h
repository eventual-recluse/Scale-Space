/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TuningEditor.h"
#include "ScaleBox.h"
#include "MultiDimensionalPad.h"
#include "ExtraLookAndFeel.h"
//==============================================================================
/**
*/
class ScaleSpaceAudioProcessorEditor  : public juce::AudioProcessorEditor
                                        , public juce::FileDragAndDropTarget
                                        , juce::Timer
{
public:
    ScaleSpaceAudioProcessorEditor (ScaleSpaceAudioProcessor&);
    ~ScaleSpaceAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    bool isInterestedInFileDrag (const juce::StringArray &files) override;
    void filesDropped (const juce::StringArray &files, int x, int y) override;
    void timerCallback() override;
        
protected:
    void scaleOpenButtonClicked(const uint32_t tuningNumber, const uint32_t tunType, const char* chooserText, const char* fileExt);
    void exportButtonClicked(const uint32_t tunType, const char* chooserText, const char* fileExt);
    void updateAllNameLabels();
    void updateNameLabel(juce::Label &label, const uint32_t nameIndex);
    bool applyNewTuningFromEditor(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType);
    void dimensionModeButtonClicked();
    void openEditorButtonClicked(const uint32_t tuningNumber);
    void openTuningEditor(const uint32_t tuningNumber, const float desktopScaleFactor);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ScaleSpaceAudioProcessor& audioProcessor;
    
    std::unique_ptr<TuningEditor> tuningEditor;
    juce::Component::SafePointer<ExternalTuningEditorWindow> editorWindow;
    
    MultiDimensionalPad multiPad;
    float multiPadSize;
    
    DimensionModeButtonLookAndFeel dimensionModeButtonLookAndFeel;
    juce::TextButton dimensionModeButton;
    
    juce::Label pluginTitleLabel;
    
    ScaleBox scaleBox1;
    ScaleBox scaleBox2;
    ScaleBox scaleBox3;
    ScaleBox scaleBox4;
    ScaleBox scaleBox5;
    ScaleBox scaleBox6;
    ScaleBox scaleBox7;
    ScaleBox scaleBox8;

    juce::TextButton exportSclButton;
    juce::TextButton exportKbmButton;
    
    juce::ToggleButton smoothToggle;
    juce::Slider smoothAmtSlider;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> yAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> zAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> smoothAmtSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> smoothToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> dimensionModeButtonAttachment;
    
    juce::Typeface::Ptr brunoAce = juce::Typeface::createSystemTypefaceFor(BinaryData::BrunoAceRegular_ttf, BinaryData::BrunoAceRegular_ttfSize);
    juce::Typeface::Ptr brunoAceSC = juce::Typeface::createSystemTypefaceFor(BinaryData::BrunoAceSCRegular_ttf, BinaryData::BrunoAceSCRegular_ttfSize);
    
    std::unique_ptr<juce::FileChooser> chooser;
    
    juce::SharedResourcePointer<TooltipWindow> tooltipWindow;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleSpaceAudioProcessorEditor)
};
