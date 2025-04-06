#ifndef EXTRALOOKANDFEEL_H
#define EXTRALOOKANDFEEL_H

class DimensionModeButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DimensionModeButtonLookAndFeel()
    {
    }
    
    juce::Typeface::Ptr brunoAceSC = juce::Typeface::createSystemTypefaceFor(BinaryData::BrunoAceSCRegular_ttf, BinaryData::BrunoAceSCRegular_ttfSize);
    
    void drawButtonText (Graphics& g, TextButton& button,
                                 bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
     {
        g.setFont (juce::FontOptions(brunoAceSC).withHeight(30));
        g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                                : TextButton::textColourOffId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = 3;
        const int leftIndent  = 3;
        const int rightIndent = 3;
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              Justification::centred, 2);
     }
    
};


#endif // EXTRALOOKANDFEEL_H
