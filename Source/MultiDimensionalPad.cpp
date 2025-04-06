#include "MultiDimensionalPad.h"

MultiDimensionalPad::MultiDimensionalPad()
{
    addAndMakeVisible(xSlider);
    xSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    xSlider.setRange(-1.0f, 1.0f, 0.0001f);
    xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    xSlider.addListener(this);
    xSlider.setEnabled(false);
    xSlider.setVisible(false);
    
    addAndMakeVisible(ySlider);
    ySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    ySlider.setRange(-1.0f, 1.0f, 0.0001f);
    ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    ySlider.addListener(this);
    ySlider.setEnabled(false);
    ySlider.setVisible(false);
    
    addAndMakeVisible(zSlider);
    zSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    zSlider.setRange(-1.0f, 1.0f, 0.0001f);
    zSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    zSlider.addListener(this);
    zSlider.setEnabled(false);
    zSlider.setVisible(false);
    
    thumbRadius = 10.0f;
    zThumbRadius = 8.0f;
    thumbDragged = NO_THUMB_SELECTED;
    
    threeDimensionMode = false;
}

MultiDimensionalPad::~MultiDimensionalPad()
{
}

void MultiDimensionalPad::mouseDown(const MouseEvent& event)
{
    // required for getDistanceFromPoint()
    juce::Point<float> pointOnline;
    
    if ( (event.position.getX() > xyPad.getX()) && (event.position.getX() < xyPad.getRight()) && (event.position.getY() > xyPad.getY()) && (event.position.getY() < xyPad.getBottom()) )
    {
        auto newPos = event.position;
            
        newPos.x = juce::jlimit<float>(xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius, newPos.x);
        newPos.y = juce::jlimit<float>(xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, newPos.y);
        
        xSlider.setValue(juce::jmap<float>(newPos.x, xyPad.getX() + thumbRadius, xyPad.getRight()  - thumbRadius, xSlider.getMinimum(), xSlider.getMaximum()));
        ySlider.setValue(juce::jmap<float>(newPos.y, xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, ySlider.getMaximum(), ySlider.getMinimum()));

        repaint();
        
        thumbDragged = XY_PAD_THUMB;
    }
    
    else if ( (threeDimensionMode) && (zThumbPos.getDistanceFrom(juce::Point<float>(event.position.getX(), event.position.getY())) < zThumbRadius) )
    {
        thumbDragged = Z_TRACK_THUMB;
    }
    
    else if ( (threeDimensionMode) && (zTrack.getDistanceFromPoint(event.position, pointOnline) < zThumbRadius) )
    {
        auto newPos = event.position;
        
        zSlider.setValue(juce::jmap<float>(zTrack.findNearestProportionalPositionTo(event.position), zSlider.getMinimum(), zSlider.getMaximum()));
        
        repaint();
        
        thumbDragged = Z_TRACK_THUMB;
    }
    
    else
    {
        thumbDragged = NO_THUMB_SELECTED;
    }
}

void MultiDimensionalPad::mouseDrag(const MouseEvent& event)
{
    if (thumbDragged == XY_PAD_THUMB)
    {
        auto newPos = event.position;

        newPos.x = juce::jlimit<float>(xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius, newPos.x);
        newPos.y = juce::jlimit<float>(xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, newPos.y);

        xSlider.setValue(juce::jmap<float>(newPos.x, xyPad.getX() + thumbRadius, xyPad.getRight()  - thumbRadius, xSlider.getMinimum(), xSlider.getMaximum()));
        ySlider.setValue(juce::jmap<float>(newPos.y, xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius, ySlider.getMaximum(), ySlider.getMinimum()));

        repaint();
    }
    else if (thumbDragged == Z_TRACK_THUMB)
    {
        auto newPos = event.position;
        
        zSlider.setValue(juce::jmap<float>(zTrack.findNearestProportionalPositionTo(event.position), zSlider.getMinimum(), zSlider.getMaximum()));
        
        repaint();
    }
}

void MultiDimensionalPad::paint(Graphics& g)
{
    // Note that in 3D mode we draw some of the 3D grid here but leave some of it
    // until the end of this paint call, as some parts should overlay the XY pad area
    if (threeDimensionMode)
    {
        // Draw z slider track
        g.setColour(juce::Colours::black);
        g.drawLine(zTrack);
        
        // Draw z slider thumb    
        zThumbPos = zTrack.getPointAlongLine( (zSlider.getValue() - zSlider.getMinimum() ) / ( zSlider.getMaximum() - zSlider.getMinimum() ) * zTrack.getLength() );
        g.setColour(juce::Colour::fromRGB(0x00, 0x54, 0xC2));
        g.fillEllipse(zThumbPos.x - zThumbRadius, zThumbPos.y - zThumbRadius, zThumbRadius * 2, zThumbRadius * 2);
        
        // Draw (some of) 3d grid
        g.setColour(juce::Colours::grey);
        g.drawLine(grid3dEdge3);
        g.drawRoundedRectangle(grid3dFace2, 10, 1);
        
        // xyPad location is anchored to the Z axis thumb
        xyPad.setBounds(zThumbPos.getX() - (xyPadSizeFor3D * 1.2f), zThumbPos.getY() - xyPadSizeFor3D, xyPadSizeFor3D, xyPadSizeFor3D);
    }
    
    // Fill XY Pad area
    g.setColour(juce::Colour::fromRGB(0xF7, 0xC4, 0x6C));
    g.fillRoundedRectangle(xyPad, 10);
    
    // Draw XY pad grid
    for (int i = 0; i < 9; i++)
    {
        if (i == 4)
        {
            g.setColour(juce::Colour::fromRGBA(0x46, 0x66, 0xE6, 0x88));
        }
        else
        {
            g.setColour(juce::Colour::fromRGBA(0x46, 0x66, 0xE6, 0x2A));
        }

        g.drawLine(xyPad.getX() + thumbRadius, xyPad.getY() + thumbRadius + i * ( (xyPad.getHeight() - (thumbRadius * 2) ) / 8), xyPad.getRight() - thumbRadius, xyPad.getY() + thumbRadius + i * ( (xyPad.getHeight() - (thumbRadius * 2) ) / 8), 2);
        g.drawLine(xyPad.getX() + thumbRadius + i * ( (xyPad.getWidth() - (thumbRadius * 2) ) / 8), xyPad.getY() + thumbRadius, xyPad.getX() + thumbRadius + i * ( (xyPad.getWidth() - (thumbRadius * 2) ) / 8), xyPad.getBottom() - thumbRadius, 2);
    }
    // Draw XY Pad
    g.setColour(juce::Colour::fromRGB(0x46, 0x66, 0xE6));
    g.drawRoundedRectangle(xyPad, 10, 2);

    // Draw XY pad thumb
    thumbPos = juce::Point<float>(  juce::jmap<float>(xSlider.getValue(), xSlider.getMinimum(), xSlider.getMaximum(), xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius),
                                    juce::jmap<float>(ySlider.getValue(), ySlider.getMaximum(), ySlider.getMinimum(), xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius) );
    g.setColour(juce::Colour::fromRGB(0x00, 0x54, 0xC2));
    g.fillEllipse(thumbPos.x - thumbRadius, thumbPos.y - thumbRadius, thumbRadius * 2, thumbRadius * 2);
    
    if (threeDimensionMode)
    {
        // Draw rest of 3D grid
        g.setColour(juce::Colours::grey);
        g.drawLine(grid3dEdge1);
        g.drawLine(grid3dEdge2);
        g.drawLine(grid3dEdge4);
        g.drawRoundedRectangle(grid3dFace1, 10, 1);
    }
}

void MultiDimensionalPad::sliderValueChanged(Slider* slider)
{
    if (slider == &xSlider)
    {
        thumbPos.x = juce::jmap<float>(xSlider.getValue(), xSlider.getMinimum(), xSlider.getMaximum(), xyPad.getX() + thumbRadius, xyPad.getRight() - thumbRadius);
    }
    else if (slider == &ySlider)
    {
        thumbPos.y = juce::jmap<float>(ySlider.getValue(), ySlider.getMaximum(), ySlider.getMinimum(), xyPad.getY() + thumbRadius, xyPad.getBottom() - thumbRadius);
    }
    else if (slider == &zSlider)
    {
        zThumbPos = zTrack.getPointAlongLine( (zSlider.getValue() - zSlider.getMinimum() ) / ( zSlider.getMaximum() - zSlider.getMinimum() ) * zTrack.getLength() );
    }

    repaint();
    
}

void MultiDimensionalPad::resized()
{
    if (!threeDimensionMode)
    {
        setXYPadBoundsForTwoDWidget();
    }
    
    // The size of 3D widget components can be set here whatever the mode
    // They are not displayed in 2D mode
    xyPadSizeFor3D = getWidth() / 1.8f;
    
    // The 3D mode widget is drawn relative to the z axis track start,
    // that is, the lower left end of the z axis track
    juce::Point<float> zStart(getWidth() * 0.7f, getHeight() * 0.9f);
    juce::Point<float> zEnd(zStart.getX() + xyPadSizeFor3D / 2.0f, zStart.getY() - xyPadSizeFor3D / 2.0f);
    
    zTrack.setStart(zStart);
    zTrack.setEnd(zEnd);
    
    float xOrigin = zTrack.getStartX() - (xyPadSizeFor3D * 1.2f);
    float yOrigin = zTrack.getStartY() - xyPadSizeFor3D;
    
    grid3dFace1.setBounds(xOrigin, yOrigin, xyPadSizeFor3D, xyPadSizeFor3D);
    grid3dFace2.setBounds(xOrigin + xyPadSizeFor3D / 2.0f, yOrigin - xyPadSizeFor3D / 2.0f, xyPadSizeFor3D, xyPadSizeFor3D);
    
    juce::Point<float> gridOrigin(xOrigin + 2.0f, yOrigin + 3.0f);
    grid3dEdge1.setStart(gridOrigin);
    grid3dEdge1.setEnd(gridOrigin.getX() + xyPadSizeFor3D / 2.0f, gridOrigin.getY() - xyPadSizeFor3D / 2.0f);
    
    grid3dEdge2.setStart(gridOrigin.getX() + xyPadSizeFor3D - 5.0f, gridOrigin.getY() + 1.0f);
    grid3dEdge2.setEnd(gridOrigin.getX() + xyPadSizeFor3D - 5.0f + xyPadSizeFor3D / 2.0f, gridOrigin.getY() + 1.0f - xyPadSizeFor3D / 2.0f);
    
    grid3dEdge3.setStart(gridOrigin.getX() + 1.0f, gridOrigin.getY() - 5.0f + xyPadSizeFor3D);
    grid3dEdge3.setEnd(gridOrigin.getX() + 1.0f + xyPadSizeFor3D / 2.0f, gridOrigin.getY() - 5.0f + xyPadSizeFor3D / 2.0f);
    
    grid3dEdge4.setStart(gridOrigin.getX() - 5.0f + xyPadSizeFor3D, gridOrigin.getY() - 5.0f + xyPadSizeFor3D);
    grid3dEdge4.setEnd(gridOrigin.getX() - 5.0f + xyPadSizeFor3D + xyPadSizeFor3D / 2.0f, gridOrigin.getY() - 5.0f + xyPadSizeFor3D / 2.0f);
}

void MultiDimensionalPad::threeDimensionModeActive(const bool status)
{
    threeDimensionMode = status;
    
    if (!threeDimensionMode)
    {
        setXYPadBoundsForTwoDWidget();
    }
    
    repaint();
}

void MultiDimensionalPad::setXYPadBoundsForTwoDWidget()
{
    xyPad.setBounds(2, 2,  getWidth() - 4, getHeight() - 4);
}
