/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScaleSpaceAudioProcessor::ScaleSpaceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
     // No buses required
     ),
    apvts(*this, nullptr, "Parameters", addParameters()),
    stringStates("stringStates")
#endif
{
    apvts.state = juce::ValueTree("Parameters");
    
    standardKBMMapping = Tunings::Tuning().keyboardMapping.rawText;
    standardSCLTuning = Tunings::Tuning().scale.rawText;
    
    tuning1 = Tunings::Tuning();
    tuning2 = Tunings::Tuning();
    tuning3 = Tunings::Tuning();
    tuning4 = Tunings::Tuning();
    tuning5 = Tunings::Tuning();
    tuning6 = Tunings::Tuning();
    tuning7 = Tunings::Tuning();
    tuning8 = Tunings::Tuning();
    testTuning = Tunings::Tuning();
    
    // Register MTS master
    if (MTS_CanRegisterMaster())
    {
        MTS_RegisterMaster();
        MTS_SetScaleName("Tuning set by Scale-Space");
    }
    
    // Fill the frequency arrays with default frequencies from testTuning
    for (int i = 0; i < 128; i++)
    {
        frequenciesInHz[i] = testTuning.frequencyForMidiNote(i);
        targetFrequenciesInHz[i] = testTuning.frequencyForMidiNote(i);
    }
        
    setStringState(kStateFileSCL1, standardSCLTuning);
    setStringState(kStateFileSCL2, standardSCLTuning);
    setStringState(kStateFileSCL3, standardSCLTuning);
    setStringState(kStateFileSCL4, standardSCLTuning);
    setStringState(kStateFileSCL5, standardSCLTuning);
    setStringState(kStateFileSCL6, standardSCLTuning);
    setStringState(kStateFileSCL7, standardSCLTuning);
    setStringState(kStateFileSCL8, standardSCLTuning);
    setStringState(kStateFileKBM1, standardKBMMapping);
    setStringState(kStateFileKBM2, standardKBMMapping);
    setStringState(kStateFileKBM3, standardKBMMapping);
    setStringState(kStateFileKBM4, standardKBMMapping);
    setStringState(kStateFileKBM5, standardKBMMapping);
    setStringState(kStateFileKBM6, standardKBMMapping);
    setStringState(kStateFileKBM7, standardKBMMapping);
    setStringState(kStateFileKBM8, standardKBMMapping);
    setStringState(kStateNameSCL1, "Standard SCL tuning");
    setStringState(kStateNameSCL2, "Standard SCL tuning");
    setStringState(kStateNameSCL3, "Standard SCL tuning");
    setStringState(kStateNameSCL4, "Standard SCL tuning");
    setStringState(kStateNameSCL5, "Standard SCL tuning");
    setStringState(kStateNameSCL6, "Standard SCL tuning");
    setStringState(kStateNameSCL7, "Standard SCL tuning");
    setStringState(kStateNameSCL8, "Standard SCL tuning");
    setStringState(kStateNameKBM1, "Standard KBM mapping");
    setStringState(kStateNameKBM2, "Standard KBM mapping");
    setStringState(kStateNameKBM3, "Standard KBM mapping");
    setStringState(kStateNameKBM4, "Standard KBM mapping");
    setStringState(kStateNameKBM5, "Standard KBM mapping");
    setStringState(kStateNameKBM6, "Standard KBM mapping");
    setStringState(kStateNameKBM7, "Standard KBM mapping");
    setStringState(kStateNameKBM8, "Standard KBM mapping");
    
    setStringState(kStateLastOpenedLoadPath, "");
    setStringState(kStateFileSavePath, "");
    
    paintFlag = false;
    keysOnFlag = false;
    
    previousSmooth = false;
    
    currentEditedTuning = TUNING_COUNT;
}

ScaleSpaceAudioProcessor::~ScaleSpaceAudioProcessor()
{
    MTS_DeregisterMaster();
}

//==============================================================================
const juce::String ScaleSpaceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ScaleSpaceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ScaleSpaceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ScaleSpaceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ScaleSpaceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ScaleSpaceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ScaleSpaceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ScaleSpaceAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ScaleSpaceAudioProcessor::getProgramName (int index)
{
    return {};
}

void ScaleSpaceAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ScaleSpaceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ScaleSpaceAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ScaleSpaceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ScaleSpaceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    //auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();

    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //    buffer.clear (i, 0, buffer.getNumSamples());

    // Calculated weighted average of the four scales, and set target frequencies 
    
    float x = *apvts.getRawParameterValue("x");
    float y = *apvts.getRawParameterValue("y");
    float z = *apvts.getRawParameterValue("z");
    
    bool threeDMode = *apvts.getRawParameterValue("threeDimensional");
    
    bool smoothOn = *apvts.getRawParameterValue("smooth");
    double smoothFactor = *apvts.getRawParameterValue("smoothAmount") * 1000;
    int frames = buffer.getNumSamples();
    
    if (!threeDMode)
    {
        // 2D mode. Tunings 1 to 4 and axes x and y.
        for (uint32_t i = 0; i < 128; i++)
        {
            targetFrequenciesInHz[i] =  (tuning1.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f + (y / 2.0f)))
                                        + (tuning2.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f + (y / 2.0f)))
                                        + (tuning3.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f - (y / 2.0f)))
                                        + (tuning4.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f - (y / 2.0f)));
            
            if ( smoothOn && !previousSmooth )
                frequenciesInHz[i] = targetFrequenciesInHz[i];
        }
    }
    else
    {
        // 3D mode. Tunings 1 to 8 and axes x, y and z.
        for (uint32_t i = 0; i < 128; i++)
        {
            targetFrequenciesInHz[i] =  (tuning1.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f + (y / 2.0f)) * (0.5f - (z / 2.0f)))
                                        + (tuning2.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f + (y / 2.0f)) * (0.5f - (z / 2.0f)))
                                        + (tuning3.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f - (y / 2.0f)) * (0.5f - (z / 2.0f)))
                                        + (tuning4.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f - (y / 2.0f)) * (0.5f - (z/  2.0f)))
                                        + (tuning5.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f + (y / 2.0f)) * (0.5f + (z / 2.0f)))
                                        + (tuning6.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f + (y / 2.0f)) * (0.5f + (z / 2.0f)))
                                        + (tuning7.frequencyForMidiNote(i) * (0.5f - (x / 2.0f)) * (0.5f - (y / 2.0f)) * (0.5f + (z / 2.0f)))
                                        + (tuning8.frequencyForMidiNote(i) * (0.5f + (x / 2.0f)) * (0.5f - (y / 2.0f)) * (0.5f + (z / 2.0f)));
            
            if ( smoothOn && !previousSmooth )
                frequenciesInHz[i] = targetFrequenciesInHz[i];
        }
    }
    
    
    // Smoothing. Currently done via division of the remaining difference to target
    // for every frame, to try for consistent behaviour regardless of buffer size
      
    if (smoothOn)
    {
        for (uint32_t fr = 0; fr < frames; ++fr)
        {
            for (int i = 0; i < 128; i++)
            {
                double difference = targetFrequenciesInHz[i] - frequenciesInHz[i];
                if (std::fabs(difference) < 0.01f)
                {
                    frequenciesInHz[i] = targetFrequenciesInHz[i];
                }
                else
                {
                    frequenciesInHz[i] += (difference / smoothFactor);
                }
            }
        }
        MTS_SetNoteTunings(frequenciesInHz);
    }
    else
    {
        MTS_SetNoteTunings(targetFrequenciesInHz);
    }
    
    
    previousSmooth = smoothOn;
    
    // Register midi on & off notes for display in the tuning editor    
    if (!midiMessages.isEmpty())
    {
        for (const MidiMessageMetadata md : midiMessages)
        {
            MidiMessage msg = md.getMessage();
            if (msg.isNoteOn())
            {
                if (msg.getNoteNumber() >= 0 && msg.getNoteNumber() <= 127)
                    currentKeysOn[msg.getNoteNumber()] = true;
                    keysOnFlag = true;
            }
            else if (msg.isNoteOff())
            {
                if (msg.getNoteNumber() >= 0 && msg.getNoteNumber() <= 127)
                    currentKeysOn[msg.getNoteNumber()] = false;
                    keysOnFlag = true;
            }
        }
    }
}



//==============================================================================
bool ScaleSpaceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ScaleSpaceAudioProcessor::createEditor()
{
    return new ScaleSpaceAudioProcessorEditor (*this);
}

//==============================================================================
void ScaleSpaceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    // Create a new XmlElement to store the parameter and string states.
    // We'll store a combination of apvts xml and stringStates xml.
    std::unique_ptr<juce::XmlElement> settings (new juce::XmlElement ("SETTINGS"));
    
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    // Create a deep copy of xml (because xml created by createXml is a unique_ptr, it can't be added as a child element to settings)
    juce::XmlElement* parameters = new juce::XmlElement (*xml);
    // add the parameter xml to settings as a child element
    settings->addChildElement (parameters);
    
    // Add the state strings. They will all be tagged "STRINGS"
    for (uint32_t i = 0; i < kStateCount; i++)
    {
        juce::XmlElement* saveStates = new juce::XmlElement ("STRINGS");
        saveStates->setAttribute ("key", getStateKey(i));
        saveStates->setAttribute ("value", getStringState(i));
        settings->addChildElement (saveStates);
    }
    
    copyXmlToBinary (*settings, destData);
}
void ScaleSpaceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName ("SETTINGS"))
        {
            for (auto* e : xmlState->getChildIterator())
            {
                if (e->hasTagName ("STRINGS"))
                {
                    setStringState( e->getStringAttribute ("key").toRawUTF8(), e->getStringAttribute ("value") );
                }
                else if (e->hasTagName (apvts.state.getType()))
                {
                    // The whole apvts state xml was added as one child element. It has the same tag as our apvts state.
                    apvts.replaceState (juce::ValueTree::fromXml (*e));
                }
            }
        }
    }
    // Apply tunings. false set as we don't need to update the file string states, as we're restoring the tunings from those states
    applyNewTuning(TUNING_1, getStringState(kStateFileSCL1).toStdString(), SCL, false);
    applyNewTuning(TUNING_2, getStringState(kStateFileSCL2).toStdString(), SCL, false);
    applyNewTuning(TUNING_3, getStringState(kStateFileSCL3).toStdString(), SCL, false);
    applyNewTuning(TUNING_4, getStringState(kStateFileSCL4).toStdString(), SCL, false);
    applyNewTuning(TUNING_5, getStringState(kStateFileSCL5).toStdString(), SCL, false);
    applyNewTuning(TUNING_6, getStringState(kStateFileSCL6).toStdString(), SCL, false);
    applyNewTuning(TUNING_7, getStringState(kStateFileSCL7).toStdString(), SCL, false);
    applyNewTuning(TUNING_8, getStringState(kStateFileSCL8).toStdString(), SCL, false);
    applyNewTuning(TUNING_1, getStringState(kStateFileKBM1).toStdString(), KBM, false);
    applyNewTuning(TUNING_2, getStringState(kStateFileKBM2).toStdString(), KBM, false);
    applyNewTuning(TUNING_3, getStringState(kStateFileKBM3).toStdString(), KBM, false);
    applyNewTuning(TUNING_4, getStringState(kStateFileKBM4).toStdString(), KBM, false);
    applyNewTuning(TUNING_5, getStringState(kStateFileKBM5).toStdString(), KBM, false);
    applyNewTuning(TUNING_6, getStringState(kStateFileKBM6).toStdString(), KBM, false);
    applyNewTuning(TUNING_7, getStringState(kStateFileKBM7).toStdString(), KBM, false);
    applyNewTuning(TUNING_8, getStringState(kStateFileKBM8).toStdString(), KBM, false);
}

void ScaleSpaceAudioProcessor::setStringState(juce::Identifier id, const juce::String & newVal)
{
    stringStates.setAttribute (id, newVal);
}

void ScaleSpaceAudioProcessor::setStringState(const uint32_t index, const juce::String & newVal)
{
    stringStates.setAttribute (getStateKey(index), newVal);
}

void ScaleSpaceAudioProcessor::setIntState(const uint32_t index, const int newVal)
{
    stringStates.setAttribute (getStateKey(index), newVal);
}

const juce::String ScaleSpaceAudioProcessor::getStringState(const uint32_t index)
{
    return stringStates.getStringAttribute (getStateKey(index));
}

const int ScaleSpaceAudioProcessor::getIntState(const uint32_t index)
{
    return stringStates.getIntAttribute (getStateKey(index));
}

juce::AudioProcessorValueTreeState::ParameterLayout ScaleSpaceAudioProcessor::addParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout paramLayout;
    
    paramLayout.add(std::make_unique<juce::AudioParameterFloat>("x", "X", -1.0f, 1.0f, -1.0f));
    paramLayout.add(std::make_unique<juce::AudioParameterFloat>("y", "Y", -1.0f, 1.0f, 1.0f));
    paramLayout.add(std::make_unique<juce::AudioParameterFloat>("z", "Z", -1.0f, 1.0f, -1.0f));
    paramLayout.add(std::make_unique<juce::AudioParameterFloat>("smoothAmount", "Smooth Amt", 1.0f, 10.0f, 1.0f));
    paramLayout.add(std::make_unique<juce::AudioParameterBool>("smooth", "Smooth", false));
    paramLayout.add(std::make_unique<juce::AudioParameterBool>("threeDimensional", "3D", false));
    
    return paramLayout;
}

AudioProcessorValueTreeState& ScaleSpaceAudioProcessor::getApvts()
{
    return apvts;
}

Tunings::Tuning& ScaleSpaceAudioProcessor::getTuning(const uint32_t tuningNumber) noexcept
{
    switch (tuningNumber)
    {
        case TUNING_1: return tuning1;
        case TUNING_2: return tuning2;
        case TUNING_3: return tuning3;
        case TUNING_4: return tuning4;
        case TUNING_5: return tuning5;
        case TUNING_6: return tuning6;
        case TUNING_7: return tuning7;
        case TUNING_8: return tuning8;
    }
    // return testTuning by default (shouldn't ever happen)
    return testTuning;
}

Tunings::Tuning ScaleSpaceAudioProcessor::getCopyOfTuning(const uint32_t tuningNumber) noexcept
{
    switch (tuningNumber)
    {
        case TUNING_1: return tuning1;
        case TUNING_2: return tuning2;
        case TUNING_3: return tuning3;
        case TUNING_4: return tuning4;
        case TUNING_5: return tuning5;
        case TUNING_6: return tuning6;
        case TUNING_7: return tuning7;
        case TUNING_8: return tuning8;
    }
    // return testTuning by default (shouldn't ever happen)
    return testTuning;
}

Tunings::Tuning& ScaleSpaceAudioProcessor::getTestTuning()
{
    return testTuning;
}

const char* ScaleSpaceAudioProcessor::getStateKey(const uint32_t index) noexcept
{
    switch (index)
    {
        case kStateFileSCL1: return "scl_file_1";
        case kStateFileSCL2: return "scl_file_2";
        case kStateFileSCL3: return "scl_file_3";
        case kStateFileSCL4: return "scl_file_4";
        case kStateFileSCL5: return "scl_file_5";
        case kStateFileSCL6: return "scl_file_6";
        case kStateFileSCL7: return "scl_file_7";
        case kStateFileSCL8: return "scl_file_8";
        case kStateFileKBM1: return "kbm_file_1";
        case kStateFileKBM2: return "kbm_file_2";
        case kStateFileKBM3: return "kbm_file_3";
        case kStateFileKBM4: return "kbm_file_4";
        case kStateFileKBM5: return "kbm_file_5";
        case kStateFileKBM6: return "kbm_file_6";
        case kStateFileKBM7: return "kbm_file_7";
        case kStateFileKBM8: return "kbm_file_8";
        case kStateNameSCL1: return "scl_name_1";
        case kStateNameSCL2: return "scl_name_2";
        case kStateNameSCL3: return "scl_name_3";
        case kStateNameSCL4: return "scl_name_4";
        case kStateNameSCL5: return "scl_name_5";
        case kStateNameSCL6: return "scl_name_6";
        case kStateNameSCL7: return "scl_name_7";
        case kStateNameSCL8: return "scl_name_8";
        case kStateNameKBM1: return "kbm_name_1";
        case kStateNameKBM2: return "kbm_name_2";
        case kStateNameKBM3: return "kbm_name_3";
        case kStateNameKBM4: return "kbm_name_4";
        case kStateNameKBM5: return "kbm_name_5";
        case kStateNameKBM6: return "kbm_name_6";
        case kStateNameKBM7: return "kbm_name_7";
        case kStateNameKBM8: return "kbm_name_8";
        case kStateFileSavePath: return "file_save_path";
        case kStateLastOpenedLoadPath: return "last_opened_load_path";
        case kStateEditorMode: return "editor_mode";
        case kStateEditorPolarMode : return "editor_polar_mode";
    }

    return "unknown";
}

// To make it easier to update the name string states from the main UI or tuning editor
void ScaleSpaceAudioProcessor::setNameStateForTuning(const uint32_t tuningNumber, const juce::String & nameString, const uint32_t tunType)
{
	setStringState(getNameIndexFromTuningAndFileType(tuningNumber, tunType), nameString);
}

// This function is to check the tuning is valid (before attempting to apply it). This check takes the tuning number, scl or kbm tuning data, and tuning type.
bool ScaleSpaceAudioProcessor::editedTuningValid(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType)
{
    bool success = false;
    
    if (tunType == SCL)
    {
        try
        {
            auto s = Tunings::parseSCLData(tunData.toStdString());
            auto k = getTuning(tuningNumber).keyboardMapping;
            testTuning = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
            success = true;
        }
        catch (const std::exception& e)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Couldn't apply SCL", e.what());
            success = false;
        }
    }
    else if (tunType == KBM)
    {
        try
        {
            auto s = getTuning(tuningNumber).scale;
            auto k = Tunings::parseKBMData(tunData.toStdString());
            testTuning = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
            success = true;
        }
        catch (const std::exception& e)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Couldn't apply KBM", e.what());
            success = false;
        }
    }
    return success;
}

// This function is to check the tuning is valid (before attempting to apply it). This check takes a Scale and KeyboardMapping
bool ScaleSpaceAudioProcessor::validSclKbmPair(const Tunings::Scale &s, const Tunings::KeyboardMapping &k)
{
    bool success = false;
    
    try
    {
        testTuning = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
        success = true;
    }
    catch (const std::exception& e)
    {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Couldn't apply tuning", e.what());
        success = false;
    }
        
    return success;
}

// setFileState can be set to false for restoring the tuning from the file string state, because in that case we don't need to update the string state again
bool ScaleSpaceAudioProcessor::applyNewTuning(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType, const bool setFileState)
{
    bool success = false;
    
    if (editedTuningValid(tuningNumber, tunData, tunType))
    {
        // The tuning validity has already been checked but the following try - catches
        // have kept kept just in case...
        if (tunType == SCL)
        {
            try
            {
                auto k = getTuning(tuningNumber).keyboardMapping;
                auto s = Tunings::parseSCLData(tunData.toStdString());
                getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                if (setFileState)
                    setStringState( getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying SCL. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
        else if (tunType == KBM)
        {
            try
            {
                auto s = getTuning(tuningNumber).scale;
                auto k = Tunings::parseKBMData(tunData.toStdString());
                getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                if (setFileState)
                    setStringState( getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying KBM. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
    }
    return success;
}

bool ScaleSpaceAudioProcessor::applySclKbmPair(const uint32_t tuningNumber, const Tunings::Scale &s, const Tunings::KeyboardMapping &k)
{
    bool success = false;
    
    bool sameScl = false;
    bool sameKbm = false;
    
    if (s.rawText == getTuning(tuningNumber).scale.rawText)
        sameScl = true;
    
    if (k.rawText == getTuning(tuningNumber).keyboardMapping.rawText)
        sameKbm = true;
    
    if (validSclKbmPair(s, k))
    {
        // The tuning validity has already been checked but the following try - catches
        // have kept kept just in case...
        try
        {
            getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
            if (!sameScl)
            {
                setStringState( getFileIndexFromTuningAndFileType(tuningNumber, SCL), juce::String(s.rawText));
                setStringState( getNameIndexFromTuningAndFileType(tuningNumber, SCL), "Edited SCL " + getTuning(tuningNumber).scale.description);
            }
            
            if (!sameKbm)
            {
                setStringState( getFileIndexFromTuningAndFileType(tuningNumber, KBM), juce::String(k.rawText));
                setStringState( getNameIndexFromTuningAndFileType(tuningNumber, KBM), "Edited KBM" );
            }
            
            success = true;
        }
        catch (const std::exception& e)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying tuning. Scale reset.", e.what());
            // Reset stringState information for this tuning's SCL and KBM
            resetTuningStringStatesToStandard(tuningNumber);
            success = false;
        }
    }
    paintFlag = true;
    
    return success;
}

bool ScaleSpaceAudioProcessor::retuneToScale(const uint32_t tuningNumber, const Tunings::Scale &s)
{
    bool success = false;
    
    auto k = getTuning(tuningNumber).keyboardMapping;
    
    if (validSclKbmPair(s, k))
    {
        // The tuning validity has already been checked but the following try - catches
        // have kept kept just in case...
        try
        {
            getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
            
            setStringState( getFileIndexFromTuningAndFileType(tuningNumber, SCL), juce::String(s.rawText));
            setStringState( getNameIndexFromTuningAndFileType(tuningNumber, SCL), "Edited SCL " + getTuning(tuningNumber).scale.description);
            success = true;
        }
        catch (const std::exception& e)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying tuning. Scale reset.", e.what());
            // Reset stringState information for this tuning's SCL and KBM
            resetTuningStringStatesToStandard(tuningNumber);
            success = false;
        }
    }
    paintFlag = true;
    
    return success;
}

bool ScaleSpaceAudioProcessor::applyDroppedFile(const uint32_t tuningNumber, const std::string & filePath)
{
    juce::File file(filePath);

    if (!file.existsAsFile())
        return false;
        
    bool success = false;

    auto fileText = file.loadFileAsString();

    if ( file.getFileExtension() == ".scl" )
    {
        // Should be true if the new scale was applied successfully
        if (applyNewTuning(tuningNumber, fileText, SCL))
        {
            // Update Scale Name stringState
            setNameStateForTuning(tuningNumber, file.getFileName(), SCL);
            success = true;
        }
    }
    else if ( file.getFileExtension() == ".kbm" )
    {
        // Should be true if the new scale was applied successfully
        if (applyNewTuning(tuningNumber, fileText, KBM))
        {
            setNameStateForTuning(tuningNumber, file.getFileName(), KBM);
            success = true;
        }
    }

    paintFlag = true;
    
    return success;
}

void ScaleSpaceAudioProcessor::resetTuningStringStatesToStandard(const uint32_t tuningNumber)
{
    setStringState( getFileIndexFromTuningAndFileType(tuningNumber, SCL), standardSCLTuning);
    setStringState( getFileIndexFromTuningAndFileType(tuningNumber, KBM), standardKBMMapping);
    setStringState( getNameIndexFromTuningAndFileType(tuningNumber, SCL), "Standard SCL tuning");
    setStringState( getNameIndexFromTuningAndFileType(tuningNumber, KBM), "Standard KBM mapping");
}

const uint32_t ScaleSpaceAudioProcessor::getComplementaryKbmOrSclState(const uint32_t index) noexcept
{
    switch (index)
    {
        case kStateFileSCL1: return kStateFileKBM1;
        case kStateFileSCL2: return kStateFileKBM2;
        case kStateFileSCL3: return kStateFileKBM3;
        case kStateFileSCL4: return kStateFileKBM4;
        case kStateFileSCL5: return kStateFileKBM5;
        case kStateFileSCL6: return kStateFileKBM6;
        case kStateFileSCL7: return kStateFileKBM7;
        case kStateFileSCL8: return kStateFileKBM8;
        case kStateFileKBM1: return kStateFileSCL1;
        case kStateFileKBM2: return kStateFileSCL2;
        case kStateFileKBM3: return kStateFileSCL3;
        case kStateFileKBM4: return kStateFileSCL4;
        case kStateFileKBM5: return kStateFileSCL5;
        case kStateFileKBM6: return kStateFileSCL6;
        case kStateFileKBM7: return kStateFileSCL7;
        case kStateFileKBM8: return kStateFileSCL8;
        case kStateNameSCL1: return kStateNameKBM1;
        case kStateNameSCL2: return kStateNameKBM2;
        case kStateNameSCL3: return kStateNameKBM3;
        case kStateNameSCL4: return kStateNameKBM4;
        case kStateNameSCL5: return kStateNameKBM5;
        case kStateNameSCL6: return kStateNameKBM6;
        case kStateNameSCL7: return kStateNameKBM7;
        case kStateNameSCL8: return kStateNameKBM8;
        case kStateNameKBM1: return kStateNameSCL1;
        case kStateNameKBM2: return kStateNameSCL2;
        case kStateNameKBM3: return kStateNameSCL3;
        case kStateNameKBM4: return kStateNameSCL4;
        case kStateNameKBM5: return kStateNameSCL5;
        case kStateNameKBM6: return kStateNameSCL6;
        case kStateNameKBM7: return kStateNameSCL7;
        case kStateNameKBM8: return kStateNameSCL8;
    }
    
    return kStateCount;
}

const uint32_t ScaleSpaceAudioProcessor::getFileIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept
{   
    if (tunType == SCL)
    {
        /**/ if (tuningNumber == TUNING_1)
            return kStateFileSCL1;
        else if (tuningNumber == TUNING_2)
            return kStateFileSCL2;
        else if (tuningNumber == TUNING_3)
            return kStateFileSCL3;
        else if (tuningNumber == TUNING_4)
            return kStateFileSCL4;
        else if (tuningNumber == TUNING_5)
            return kStateFileSCL5;
        else if (tuningNumber == TUNING_6)
            return kStateFileSCL6;
        else if (tuningNumber == TUNING_7)
            return kStateFileSCL7;
        else if (tuningNumber == TUNING_8)
            return kStateFileSCL8;
    }
    else if (tunType == KBM)
    {
        /**/ if (tuningNumber == TUNING_1)
            return kStateFileKBM1;
        else if (tuningNumber == TUNING_2)
            return kStateFileKBM2;
        else if (tuningNumber == TUNING_3)
            return kStateFileKBM3;
        else if (tuningNumber == TUNING_4)
            return kStateFileKBM4;
        else if (tuningNumber == TUNING_5)
            return kStateFileKBM5;
        else if (tuningNumber == TUNING_6)
            return kStateFileKBM6;
        else if (tuningNumber == TUNING_7)
            return kStateFileKBM7;
        else if (tuningNumber == TUNING_8)
            return kStateFileKBM8;
    }
    // default
    return kStateCount;
}

const uint32_t ScaleSpaceAudioProcessor::getNameIndexFromTuningAndFileType(const uint32_t tuningNumber, const uint32_t tunType) noexcept
{   
    if (tunType == SCL)
    {
        /**/ if (tuningNumber == TUNING_1)
            return kStateNameSCL1;
        else if (tuningNumber == TUNING_2)
            return kStateNameSCL2;
        else if (tuningNumber == TUNING_3)
            return kStateNameSCL3;
        else if (tuningNumber == TUNING_4)
            return kStateNameSCL4;
        else if (tuningNumber == TUNING_5)
            return kStateNameSCL5;
        else if (tuningNumber == TUNING_6)
            return kStateNameSCL6;
        else if (tuningNumber == TUNING_7)
            return kStateNameSCL7;
        else if (tuningNumber == TUNING_8)
            return kStateNameSCL8;
    }
    else if (tunType == KBM)
    {
        /**/ if (tuningNumber == TUNING_1)
            return kStateNameKBM1;
        else if (tuningNumber == TUNING_2)
            return kStateNameKBM2;
        else if (tuningNumber == TUNING_3)
            return kStateNameKBM3;
        else if (tuningNumber == TUNING_4)
            return kStateNameKBM4;
        else if (tuningNumber == TUNING_5)
            return kStateNameKBM5;
        else if (tuningNumber == TUNING_6)
            return kStateNameKBM6;
        else if (tuningNumber == TUNING_7)
            return kStateNameKBM7;
        else if (tuningNumber == TUNING_8)
            return kStateNameKBM8;
    }
    // default
    return kStateCount;
}

void ScaleSpaceAudioProcessor::exportCurrentSclTuning(const juce::String & filePath)
{
    juce::String fileNamePath(filePath);
    
    if (fileNamePath.isEmpty())
        return;
    
    if (!fileNamePath.endsWith(".scl"))
    {
        fileNamePath = fileNamePath + ".scl";
    }
    
    juce::File sclSaveFile = juce::File(fileNamePath);

    if (sclSaveFile.hasWriteAccess())
    {
        if (sclSaveFile.existsAsFile())
            sclSaveFile.deleteFile();
        
        const juce::Result sclRes (sclSaveFile.create());
        if (sclRes.wasOk())
        {
            sclSaveFile.appendText("! ", false, false, nullptr);
                    sclSaveFile.appendText(sclSaveFile.getFileName(), false, false, nullptr);
                    sclSaveFile.appendText("\n", false, false, nullptr);
            sclSaveFile.appendText("!\n", false, false, nullptr);
            sclSaveFile.appendText(sclSaveFile.getFileName(), false, false, nullptr);
                    sclSaveFile.appendText(" exported from ScaleSpace\n", false, false, nullptr);
            sclSaveFile.appendText(" 128\n", false, false, nullptr);
            sclSaveFile.appendText("!\n", false, false, nullptr);
            for (int i = 61; i < 128; i++)
            {
                // Formula for difference in cents between two frequencies is 1200 * log2(frequency1 / frequency2)
                // If frequency1 is larger than frequency2, the difference will be positive
                float cents = 1200 * std::log2(targetFrequenciesInHz[i] / targetFrequenciesInHz[60]); 
                sclSaveFile.appendText(" ", false, false, nullptr);
                    sclSaveFile.appendText(std::to_string(cents), false, false, nullptr);
                    sclSaveFile.appendText("\n", false, false, nullptr);
            }
            
            for (int i = 0; i < 61; i++)
            {
                float cents = 1200 * std::log2(targetFrequenciesInHz[i] / targetFrequenciesInHz[60]); 
                sclSaveFile.appendText(" ", false, false, nullptr);
                    sclSaveFile.appendText(std::to_string(cents), false, false, nullptr);
                    sclSaveFile.appendText("\n", false, false, nullptr);
            }
        }
    }
}

void ScaleSpaceAudioProcessor::exportCurrentKbmTuning(const juce::String & filePath)
{
    juce::String fileNamePath(filePath);
    
    if (fileNamePath.isEmpty())
        return;
    
    if (!fileNamePath.endsWith(".kbm"))
    {
        fileNamePath = fileNamePath + ".kbm";
    }
    
    juce::File kbmSaveFile = juce::File(fileNamePath);
    
    if (kbmSaveFile.hasWriteAccess())
    {
        if (kbmSaveFile.existsAsFile())
            kbmSaveFile.deleteFile();
            
        const juce::Result kbmRes (kbmSaveFile.create());
        if (kbmRes.wasOk())
        {
            kbmSaveFile.appendText("! ", false, false, nullptr);
                kbmSaveFile.appendText(kbmSaveFile.getFileName(), false, false, nullptr);
                kbmSaveFile.appendText("\n", false, false, nullptr);
            kbmSaveFile.appendText("! Key-for-key mapping with reference frequency at MIDI note 60\n", false, false, nullptr);
            kbmSaveFile.appendText("! Exported from ScaleSpace\n", false, false, nullptr);
            kbmSaveFile.appendText("! Size:\n", false, false, nullptr);
            kbmSaveFile.appendText("0\n", false, false, nullptr);
            kbmSaveFile.appendText("! First MIDI note number to retune:\n", false, false, nullptr);
            kbmSaveFile.appendText("0\n", false, false, nullptr);
            kbmSaveFile.appendText("! Last MIDI note number to retune:\n", false, false, nullptr);
            kbmSaveFile.appendText("127\n", false, false, nullptr);
            kbmSaveFile.appendText("! Middle note where the first entry of the mapping is mapped to:\n", false, false, nullptr);
            kbmSaveFile.appendText("60\n", false, false, nullptr);
            kbmSaveFile.appendText("! Reference note for which frequency is given:\n", false, false, nullptr);
            kbmSaveFile.appendText("60\n", false, false, nullptr);
            kbmSaveFile.appendText("! Frequency to tune the above note to (floating point e.g. 440.0):\n", false, false, nullptr);
            kbmSaveFile.appendText(std::to_string(static_cast<float>(targetFrequenciesInHz[60])));
                kbmSaveFile.appendText("\n", false, false, nullptr);
            kbmSaveFile.appendText("! Scale degree to consider as formal octave:\n", false, false, nullptr);
            kbmSaveFile.appendText("127\n", false, false, nullptr);
        }
    }
}

void ScaleSpaceAudioProcessor::setNoteOn(const int noteNumber)
{
    if (noteNumber >= 0 && noteNumber <= 127)
    {
        currentKeysOn[noteNumber] = true;
        keysOnFlag = true;
    }
}

void ScaleSpaceAudioProcessor::setNoteOff(const int noteNumber)
{
    if (noteNumber >= 0 && noteNumber <= 127)
    {
        currentKeysOn[noteNumber] = false;
        keysOnFlag = true;
    }
}

std::bitset<128>& ScaleSpaceAudioProcessor::getCurrentKeysOn()
{
    return currentKeysOn;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScaleSpaceAudioProcessor();
}
