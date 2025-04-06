#ifndef MULTIDIMENSIONALPAD_H
#define MULTIDIMENSIONALPAD_H

#include <JuceHeader.h>

struct MultiDimensionalPad : public juce::Component
                            , public juce::Slider::Listener
{
    MultiDimensionalPad();
    ~MultiDimensionalPad();
    
    enum Thumb {
        XY_PAD_THUMB = 0,
        Z_TRACK_THUMB = 1,
        NO_THUMB_SELECTED = 2
    };
    
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void paint(Graphics& g) override;
    void resized() override;
    void sliderValueChanged(Slider* slider) override;
    void threeDimensionModeActive(const bool status);
    void setXYPadBoundsForTwoDWidget();
    
    juce::Point<float> thumbPos;
    float thumbRadius;
    uint32_t thumbDragged;
    
    bool threeDimensionMode;
    
    juce::Slider xSlider;
    juce::Slider ySlider;
    juce::Slider zSlider;
    
    juce::Line<float> zTrack;
    juce::Point<float> zThumbPos;
    float zThumbRadius;
    
    juce::Rectangle<float> xyPad;
    float xyPadSizeFor3D;
    
    juce::Rectangle<float> grid3dFace1;
    juce::Rectangle<float> grid3dFace2;
    juce::Line<float> grid3dEdge1;
    juce::Line<float> grid3dEdge2;
    juce::Line<float> grid3dEdge3;
    juce::Line<float> grid3dEdge4;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiDimensionalPad)
};

#endif // MULTIDIMENSIONALPAD_H
