#ifndef TUNINGEDITOR_H
#define TUNINGEDITOR_H

#include "Tunings.h"
#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <sstream>
#include <set>
#include <bitset>
//#include <stdio.h>
//#include <algorithm>
//#include <locale>
//#include <cstring>
#include "fmt/format.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// For trying out different UI scales
constexpr double scaleTunEd = 1.5;

enum RadioButtonGroupIds
{
    EditModeButtonGroup = 1001,
    PolarModeButtonGroup = 1002
};

// These colors replace the ones which the Surge XT Tuning Overlay gets from Surge's skin code.
const juce::Colour btnOffTunEdCol = juce::Colours::lightgrey;
const juce::Colour btnOnTunEdCol = juce::Colours::orange;
const juce::Colour btnTextOnTunEdCol = juce::Colours::black;
const juce::Colour btnTextOffTunEdCol = juce::Colours::black;

const juce::Colour whiteKeyTunEdColor = juce::Colour::fromRGB(0x97, 0x97, 0x97);
const juce::Colour blackKeyTunEdColor = juce::Colour::fromRGB(0x24, 0x24, 0x24);
const juce::Colour separatorTunEdColor = juce::Colour::fromRGBA(0x00, 0x00, 0x00, 0xFF);
const juce::Colour textTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);
const juce::Colour pressedKeyTunEdColor = juce::Colour::fromRGB(0xFF, 0x93, 0x00);
const juce::Colour pressedKeyTextTunEdColor = juce::Colour::fromRGBA(0x00, 0x00, 0x00, 0xFF);

const juce::Colour radialBackgroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour radialKnobFillTunEdColor = juce::Colour::fromRGB(0x35, 0x15, 0x00);
const juce::Colour radialKnobFillHoverTunEdColor = juce::Colour::fromRGB(0x90, 0x40, 0x00);
const juce::Colour radialKnobBorderTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour radialKnobThumbTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);
const juce::Colour radialKnobPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0x90, 0x90);
const juce::Colour radialTonelabelTunEdColor = juce::Colour::fromRGB(0x97, 0x97, 0x97);
const juce::Colour radialTonelabelPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0x90, 0x00);
const juce::Colour radialTonelabelBorderTunEdColor = juce::Colour::fromRGB(0x97, 0x97, 0x97);
const juce::Colour radialTonelabelBackgroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour radialTonelabelTextTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour radialTonelabelBackgroundPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0x90, 0x00);
const juce::Colour radialTonelabelBorderPlayedTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour radialTonelabelTextPlayedTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);

const juce::Colour intervalBackgroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour intervalNoteLabelBackgroundTunEdColor = juce::Colour::fromRGB(0x97, 0x97, 0x97);
const juce::Colour intervalNoteLabelBackgroundPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0x90, 0x00);
const juce::Colour intervalNoteLabelForegroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour intervalNoteLabelForegroundHoverTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour intervalNoteLabelForegroundPlayedTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour intervalNoteLabelForegroundHoverPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);
const juce::Colour intervalTextTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour intervalTextHoverTunEdColor = juce::Colour::fromRGB(0x35, 0x15, 0x00);
const juce::Colour intervalSkippedTunEdColor = juce::Colour::fromRGB(0xAA, 0xAA, 0xAA);
const juce::Colour intervalHeatmapZeroTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0xFF);
const juce::Colour intervalHeatmapNegFarTunEdColor = juce::Colour::fromRGB(0x82, 0x82, 0xFF);
const juce::Colour intervalHeatmapNegNearTunEdColor = juce::Colour::fromRGB(0xFF, 0xE6, 0xFF);
const juce::Colour intervalHeatmapPosFarTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0x00);
const juce::Colour intervalHeatmapPosNearTunEdColor = juce::Colour::fromRGB(0xFF, 0xFF, 0xC8);
const juce::Colour intervalHeatmapZeroAlpha9TunEdColor = juce::Colour::fromRGBA(0xFF, 0xFF, 0xFF, 0xE5);

const juce::Colour sclkbmBackgroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour sclkbmEditorBorderTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour sclkbmEditorBackgroundTunEdColor = juce::Colour::fromRGB(0x15, 0x15, 0x15);
const juce::Colour sclkbmEditorTextTunEdColor = juce::Colour::fromRGB(0xE3, 0xE3, 0xE3);
const juce::Colour sclkbmEditorCommentTunEdColor = juce::Colour::fromRGB(0x97, 0x97, 0x97);
const juce::Colour sclkbmEditorCentsTunEdColor = juce::Colour::fromRGB(0xA3, 0xA3, 0xFF);
const juce::Colour sclkbmEditorRatioTunEdColor = juce::Colour::fromRGB(0xA3, 0xFF, 0xA3);
const juce::Colour sclkbmEditorPlayedTunEdColor = juce::Colour::fromRGB(0xFF, 0x90, 0x00);

const juce::Colour msegEditorTextTunEdColor = juce::Colour::fromRGBA(0x00, 0x00, 0x00, 0xFF);
const juce::Colour msegEditorPanelTunEdColor = juce::Colour::fromRGB(0xCD, 0xCE, 0xD4);

// Currently, ScaleSpace uses default fonts here
const juce::Font boldTunEdFont = juce::Font(juce::FontOptions(8.0f, juce::Font::bold));
const juce::Font plainTunEdFont = juce::Font(juce::FontOptions(8.0f, juce::Font::plain));
const juce::Font plainMonoTunEdFont = juce::Font(juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));

// From surge XT source code: src/common/UnitConversions.h
inline std::string get_notename(int i_value, int i_offset)
{
    int ival = i_value < 0 ? i_value - 11 : i_value;
    int octave = (ival / 12) - i_offset;
    char notenames[13][3] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"};
    // make sure to read the notenames array in reverse for negative i_values
    int note = (i_value < 0) ? (12 - abs(i_value % 12)) : (i_value % 12);

    return fmt::format("{:s}{:d}", notenames[note], octave);
}

class TuningTableListBoxModel;
class SCLKBMDisplay;
class RadialScaleGraph;
struct IntervalMatrix;
struct TuningControlArea;

class ScaleSpaceAudioProcessor;

struct TuningEditor : public juce::Component
                    , public juce::FileDragAndDropTarget
                    , public juce::Timer
{
    class TuningTextEditedListener
    {
      public:
        virtual ~TuningTextEditedListener() = default;
        virtual void scaleTextEdited(juce::String newScale) = 0;
    };
  
  
    TuningEditor(ScaleSpaceAudioProcessor& p, const uint32_t tuningIndex);
    ~TuningEditor();
    
    void onToneChanged(int tone, double newCentsValue);
    void onToneStringChanged(int tone, const std::string &newCentsValue);
    void onScaleRescaled(double scaledBy);
    void onScaleRescaledAbsolute(double setRITo);
    void recalculateScaleText();
    void setTuning(const Tunings::Tuning &t);
    void setupForTuning(const uint32_t tuningIndex);
    void onNewSCLKBM(const std::string &scl, const std::string &kbm);
    void resized() override;
    void setMidiOnKeys(const std::bitset<128> &keys);
    void setApplyEnabled(bool b);
    void setPolarMode(const int m);
    void showEditor(int which);
    
    bool isInterestedInFileDrag (const juce::StringArray &files) override;
    void filesDropped (const juce::StringArray &files, int x, int y) override;
    void timerCallback() override;
    
    std::unique_ptr<TuningTableListBoxModel> tuningKeyboardTableModel;
    std::unique_ptr<juce::TableListBox> tuningKeyboardTable;

    std::unique_ptr<TuningControlArea> controlArea;

    std::unique_ptr<SCLKBMDisplay> sclKbmDisplay;
    std::unique_ptr<RadialScaleGraph> radialScaleGraph;
    std::unique_ptr<IntervalMatrix> intervalMatrix;
    
    Tunings::Tuning tuning;
    Tunings::KeyboardMapping currentMapping;
    Tunings::Scale currentScale;
    
    ScaleSpaceAudioProcessor& audioProcessor;
    
    uint32_t tuningNumber;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TuningEditor)
    
};


struct ExternalTuningEditorWindow : public juce::DocumentWindow
{
    ExternalTuningEditorWindow (const juce::String& name, juce::Colour backgroundColour, int requiredButtons);
    
    ~ExternalTuningEditorWindow();

    void closeButtonPressed() override;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExternalTuningEditorWindow)
};


#endif // TUNINGEDITOR_H
