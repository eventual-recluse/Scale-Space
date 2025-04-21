/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Tunings.h"
#include "libMTSMaster.h"
#include "TuningEditor.h"

enum States {
    kStateFileSCL1 = 0,
    kStateFileSCL2 = 1,
    kStateFileSCL3 = 2,
    kStateFileSCL4 = 3,
    kStateFileSCL5 = 4,
    kStateFileSCL6 = 5,
    kStateFileSCL7 = 6,
    kStateFileSCL8 = 7,
    kStateFileKBM1 = 8,
    kStateFileKBM2 = 9,
    kStateFileKBM3 = 10,
    kStateFileKBM4 = 11,
    kStateFileKBM5 = 12,
    kStateFileKBM6 = 13,
    kStateFileKBM7 = 14,
    kStateFileKBM8 = 15,
    kStateNameSCL1 = 16,
    kStateNameSCL2 = 17,
    kStateNameSCL3 = 18,
    kStateNameSCL4 = 19,
    kStateNameSCL5 = 20,
    kStateNameSCL6 = 21,
    kStateNameSCL7 = 22,
    kStateNameSCL8 = 23,
    kStateNameKBM1 = 24,
    kStateNameKBM2 = 25,
    kStateNameKBM3 = 26,
    kStateNameKBM4 = 27,
    kStateNameKBM5 = 28,
    kStateNameKBM6 = 29,
    kStateNameKBM7 = 30,
    kStateNameKBM8 = 31,
    kStateLastOpenedLoadPath = 32,
    kStateFileSavePath = 33,
    kStateEditorMode = 34,
    kStateEditorPolarMode = 35,
    kStateCount    = 36
};

enum ScaleTunings {
    TUNING_1 = 0,
    TUNING_2 = 1,
    TUNING_3 = 2,
    TUNING_4 = 3,
    TUNING_5 = 4,
    TUNING_6 = 5,
    TUNING_7 = 6,
    TUNING_8 = 7,
    TUNING_COUNT = 8
};

enum FileType {
    SCL = 0,
    KBM = 1,
    FILETYPE_COUNT = 2
};

//==============================================================================
/**
*/
class ScaleSpaceAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ScaleSpaceAudioProcessor();
    ~ScaleSpaceAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void setStringState(juce::Identifier id, const juce::String & newVal);
    void setStringState(const uint32_t index, const juce::String & newVal);
    void setIntState(const uint32_t index, const int newVal);
    const juce::String getStringState(const uint32_t index);
    const int getIntState(const uint32_t index);

    AudioProcessorValueTreeState& getApvts();
    Tunings::Tuning& getTuning(const uint32_t tuningNumber) noexcept;
    Tunings::Tuning getCopyOfTuning(const uint32_t tuningNumber) noexcept;
    Tunings::Tuning& getTestTuning();
    const char* getStateKey(const uint32_t index) noexcept;
    void setNameStateForTuning(const uint32_t tuningNumber, const juce::String & nameString, const uint32_t tunType);
    bool applyNewTuning(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType, const bool setFileState = true);
    bool applySclKbmPair(const uint32_t tuningNumber, const Tunings::Scale &s, const Tunings::KeyboardMapping &k);
    bool retuneToScale(const uint32_t tuningNumber, const Tunings::Scale &s);
    void applyDroppedFile(const uint32_t tuningNumber, const std::string & filePath);
    void exportCurrentSclTuning(const juce::String & filePath);
    void exportCurrentKbmTuning(const juce::String & filePath);
    void openTuningEditor(const uint32_t tuningNumber, const float desktopScaleFactor);
    void setNoteOn(int noteNumber);
    void setNoteOff(int noteNumber);
    std::bitset<128>& getCurrentKeysOn();
    
    std::atomic<bool> paintFlag;
    std::atomic<bool> keysOnFlag;
    uint32_t currentEditedTuning;
        
    std::string standardSCLTuning;
    std::string standardKBMMapping;
    
    juce::Component::SafePointer<ExternalTuningEditorWindow> editorWindow;
    
protected:
	bool editedTuningValid(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType);
	bool validSclKbmPair(const Tunings::Scale &s, const Tunings::KeyboardMapping &k);
	void resetTuningStringStatesToStandard(const uint32_t tuningNumber);
	const uint32_t getComplementaryKbmOrSclState(const uint32_t index)  noexcept;
    const uint32_t getFileIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept;
    const uint32_t getNameIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept;
	
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout addParameters();
    // stringStates XmlElement holds our SCL & KBM file contents and names.
    // This format is also convenient for saving and restoring the states.
    juce::XmlElement stringStates;
    
    // Tunings for Scales 1 to 8
    Tunings::Tuning tuning1;
    Tunings::Tuning tuning2;
    Tunings::Tuning tuning3;
    Tunings::Tuning tuning4;
    Tunings::Tuning tuning5;
    Tunings::Tuning tuning6;
    Tunings::Tuning tuning7;
    Tunings::Tuning tuning8;
    // A separate tuning used for testing if SCL &KBM combinations are valid
    Tunings::Tuning testTuning;
    
    // Frequency arrays
    double frequenciesInHz[128];
    double targetFrequenciesInHz[128];
    
    bool previousSmooth;
    
    juce::Component::SafePointer<TuningEditor> tuningEditor;
    
    std::bitset<128> currentKeysOn;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScaleSpaceAudioProcessor)
};
