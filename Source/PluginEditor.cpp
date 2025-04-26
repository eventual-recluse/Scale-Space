/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ScaleSpaceAudioProcessorEditor::ScaleSpaceAudioProcessorEditor (ScaleSpaceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1315, 700);
    
    // Slider attachments
    xAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "x", multiPad.xSlider);
    yAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "y", multiPad.ySlider);
    zAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "z", multiPad.zSlider);
    smoothAmtSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getApvts(), "smoothAmount", smoothAmtSlider);
    smoothToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.getApvts(), "smooth", smoothToggle);
    dimensionModeButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (audioProcessor.getApvts(), "threeDimensional", dimensionModeButton);
    
    addAndMakeVisible(&multiPad);

    juce::Colour mainUITextColor = juce::Colours::black;
    juce::Colour buttonOnColor = juce::Colour::fromRGB(0xF7, 0xC4, 0x6C);
    
    addAndMakeVisible (&pluginTitleLabel);
    pluginTitleLabel.setFont (juce::FontOptions(brunoAceSC).withHeight(42));
    pluginTitleLabel.setText ("ScaleSpace", juce::dontSendNotification);
    pluginTitleLabel.setColour (juce::Label::textColourId, mainUITextColor);
    pluginTitleLabel.setMinimumHorizontalScale(1.0f);
    
    // Scale Box 1
    addAndMakeVisible (&scaleBox1);
    scaleBox1.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox1.scaleLabel.setText ("SCALE 1", juce::dontSendNotification);
    scaleBox1.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, SCL, "Open SCL File for Scale 1", "*.scl"); };
    scaleBox1.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_1, KBM, "Open KBM File for Scale 1", "*.kbm"); };
    scaleBox1.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_1); };

    // Scale Box 2
    addAndMakeVisible (&scaleBox2);
    scaleBox2.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox2.scaleLabel.setText ("SCALE 2", juce::dontSendNotification);
    scaleBox2.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, SCL, "Open SCL File for Scale 2", "*.scl"); };
    scaleBox2.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_2, KBM, "Open KBM File for Scale 2", "*.kbm"); };
    scaleBox2.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_2); };
    
    // Scale Box 3
    addAndMakeVisible (&scaleBox3);
    scaleBox3.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox3.scaleLabel.setText ("SCALE 3", juce::dontSendNotification);
    scaleBox3.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, SCL, "Open SCL File for Scale 3", "*.scl"); };
    scaleBox3.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_3, KBM, "Open KBM File for Scale 3", "*.kbm"); };
    scaleBox3.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_3); };
    
    // Scale Box 4
    addAndMakeVisible (&scaleBox4);
    scaleBox4.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox4.scaleLabel.setText ("SCALE 4", juce::dontSendNotification);
    scaleBox4.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, SCL, "Open SCL File for Scale 4", "*.scl"); };
    scaleBox4.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_4, KBM, "Open KBM File for Scale 4", "*.kbm"); };
    scaleBox4.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_4); };
    
    // Scale Box 5
    addAndMakeVisible (&scaleBox5);
    scaleBox5.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox5.scaleLabel.setText ("SCALE 5", juce::dontSendNotification);
    scaleBox5.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_5, SCL, "Open SCL File for Scale 5", "*.scl"); };
    scaleBox5.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_5, KBM, "Open KBM File for Scale 5", "*.kbm"); };
    scaleBox5.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_5); };
    
    // Scale Box 6
    addAndMakeVisible (&scaleBox6);
    scaleBox6.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox6.scaleLabel.setText ("SCALE 6", juce::dontSendNotification);
    scaleBox6.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_6, SCL, "Open SCL File for Scale 6", "*.scl"); };
    scaleBox6.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_6, KBM, "Open KBM File for Scale 6", "*.kbm"); };
    scaleBox6.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_6); };
    
    // Scale Box 7
    addAndMakeVisible (&scaleBox7);
    scaleBox7.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox7.scaleLabel.setText ("SCALE 7", juce::dontSendNotification);
    scaleBox7.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_7, SCL, "Open SCL File for Scale 7", "*.scl"); };
    scaleBox7.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_7, KBM, "Open KBM File for Scale 7", "*.kbm"); };
    scaleBox7.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_7); };
    
    // Scale Box 8
    addAndMakeVisible (&scaleBox8);
    scaleBox8.scaleLabel.setFont (juce::FontOptions(brunoAce).withHeight(22));
    scaleBox8.scaleLabel.setText ("SCALE 8", juce::dontSendNotification);
    scaleBox8.openSclButton.onClick = [this] { scaleOpenButtonClicked(TUNING_8, SCL, "Open SCL File for Scale 8", "*.scl"); };
    scaleBox8.openKbmButton.onClick = [this] { scaleOpenButtonClicked(TUNING_8, KBM, "Open KBM File for Scale 8", "*.kbm"); };
    scaleBox8.openEditorButton.onClick = [this] { openEditorButtonClicked(TUNING_8); };
    
    // Export buttons
    addAndMakeVisible(&exportSclButton);
    exportSclButton.setColour (juce::TextButton::buttonColourId, buttonOnColor);
    exportSclButton.setColour (juce::TextButton::textColourOffId, mainUITextColor);
    exportSclButton.setButtonText ("Export SCL");
    exportSclButton.onClick = [this] { exportButtonClicked(SCL, "Save SCL File for current tuning", "*.scl"); };
    
    addAndMakeVisible(&exportKbmButton);
    exportKbmButton.setColour (juce::TextButton::buttonColourId, buttonOnColor);
    exportKbmButton.setColour (juce::TextButton::textColourOffId, mainUITextColor);
    exportKbmButton.setButtonText ("Export KBM");
    exportKbmButton.onClick = [this] { exportButtonClicked(KBM, "Save KBM File for current tuning", "*.kbm"); };
    
    addAndMakeVisible(&smoothToggle);
    smoothToggle.setColour (juce::ToggleButton::textColourId, juce::Colours::black);
    smoothToggle.setColour (juce::ToggleButton::tickColourId, juce::Colours::black);
    smoothToggle.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
    smoothToggle.setButtonText ("Smooth");
    
    addAndMakeVisible (&smoothAmtSlider);
    smoothAmtSlider.setColour(juce::Slider::thumbColourId , juce::Colour::fromRGB(0x00, 0x54, 0xC2));
    smoothAmtSlider.setColour(juce::Slider::trackColourId, buttonOnColor);
    smoothAmtSlider.setColour(juce::Slider::backgroundColourId, buttonOnColor);
    smoothAmtSlider.setColour(juce::Slider::textBoxTextColourId, mainUITextColor);
    smoothAmtSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    smoothAmtSlider.setColour(juce::Slider::textBoxOutlineColourId, mainUITextColor);
    smoothAmtSlider.setRange (1.0f, 10.0f);
    
    addAndMakeVisible (&dimensionModeButton);
    dimensionModeButton.setColour (juce::TextButton::buttonColourId, juce::Colour::fromRGB(0xFA, 0xFA, 0xFA));
    dimensionModeButton.setColour (juce::TextButton::buttonOnColourId, buttonOnColor);
    dimensionModeButton.setColour (juce::TextButton::textColourOffId, mainUITextColor);
    dimensionModeButton.setColour (juce::TextButton::textColourOnId, mainUITextColor);
    dimensionModeButton.setToggleable(true);
    dimensionModeButton.setClickingTogglesState(true);
    dimensionModeButton.setLookAndFeel(&dimensionModeButtonLookAndFeel);
    dimensionModeButton.setButtonText ("3D");
    dimensionModeButton.onClick = [this] { dimensionModeButtonClicked(); };
    
    // Setting these colours doesn't work yet
    //tooltipWindow->setColour(juce::TooltipWindow::backgroundColourId, juce::Colours::white);
    //tooltipWindow->setColour(juce::TooltipWindow::textColourId, juce::Colours::black);
    //tooltipWindow->setColour(juce::TooltipWindow::outlineColourId, juce::Colours::black);
    
    // Set SCL & KBM Name label texts
    updateAllNameLabels();
    
    multiPad.threeDimensionModeActive(*audioProcessor.getApvts().getRawParameterValue("threeDimensional"));
    
    tuningEditor = std::make_unique<TuningEditor>(audioProcessor, TUNING_1);
    
    initialUIUpdate = true;
    startTimerHz(10);
}

ScaleSpaceAudioProcessorEditor::~ScaleSpaceAudioProcessorEditor()
{
    editorWindow.deleteAndZero();
}

//==============================================================================
void ScaleSpaceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour(juce::Colour::fromRGB(0xFA, 0xFA, 0xFA));
    g.fillAll();
}

void ScaleSpaceAudioProcessorEditor::resized()
{
    pluginTitleLabel.setBounds (20, 20, getWidth() - 20, 30);
    
    if (!*audioProcessor.getApvts().getRawParameterValue("threeDimensional"))
    {
        // 2D mode
        uint32_t scaleBoxOriginX = 65;
        uint32_t scaleBoxOriginY = 80;
        uint32_t scaleBoxXOffset = 880;
        uint32_t scaleBoxYOffset = 310;
        scaleBox1.setBounds (scaleBoxOriginX, scaleBoxOriginY, 300, 170);
        scaleBox2.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY, 300, 170);
        scaleBox3.setBounds (scaleBoxOriginX, scaleBoxOriginY + scaleBoxYOffset, 300, 170);
        scaleBox4.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY + scaleBoxYOffset, 300, 170);
        
        scaleBox5.setEnabled(false);
        scaleBox5.setVisible(false);
        scaleBox6.setEnabled(false);
        scaleBox6.setVisible(false);
        scaleBox7.setEnabled(false);
        scaleBox7.setVisible(false);
        scaleBox8.setEnabled(false);
        scaleBox8.setVisible(false);
    }
    else
    {
        // 3D mode
        scaleBox5.setEnabled(true);
        scaleBox5.setVisible(true);
        scaleBox6.setEnabled(true);
        scaleBox6.setVisible(true);
        scaleBox7.setEnabled(true);
        scaleBox7.setVisible(true);
        scaleBox8.setEnabled(true);
        scaleBox8.setVisible(true);
        
        uint32_t scaleBoxOriginX = 10;
        uint32_t scaleBoxOriginY = 210;
        uint32_t scaleBoxXOffset = 210;
        uint32_t scaleBoxYOffset = 180;
        scaleBox1.setBounds (scaleBoxOriginX, scaleBoxOriginY, 200, 170);
        scaleBox2.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY, 200, 170);
        scaleBox3.setBounds (scaleBoxOriginX, scaleBoxOriginY + scaleBoxYOffset, 200, 170);
        scaleBox4.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY + scaleBoxYOffset, 200, 170);
        
        scaleBoxOriginX = 890;
        scaleBoxOriginY = 60;
        scaleBoxXOffset = 210;
        scaleBoxYOffset = 180;
        scaleBox5.setBounds (scaleBoxOriginX, scaleBoxOriginY, 200, 170);
        scaleBox6.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY, 200, 170);
        scaleBox7.setBounds (scaleBoxOriginX, scaleBoxOriginY + scaleBoxYOffset, 200, 170);
        scaleBox8.setBounds (scaleBoxOriginX + scaleBoxXOffset, scaleBoxOriginY + scaleBoxYOffset, 200, 170);
        
    }
    
    smoothToggle.setBounds (getWidth() * 0.390f, 590, 80, 30);
    smoothAmtSlider.setBounds (getWidth() * 0.442, 590, 215, 30);
    
    exportSclButton.setBounds (getWidth() * 0.415f, 650, 100, 30);
    exportKbmButton.setBounds (getWidth() * 0.505f, 650, 100, 30);
    
    multiPadSize = getHeight() / 1.55f;
    
    dimensionModeButton.setBounds (295, 15, 60, 45);
    
    float xOrigin = (getWidth() - multiPadSize) / 2.0f;
    float yOrigin = 100.0f;
    multiPad.setBounds(xOrigin, yOrigin, multiPadSize, multiPadSize);
  
}

void ScaleSpaceAudioProcessorEditor::scaleOpenButtonClicked(const uint32_t tuningNumber, const uint32_t tunType, const char* chooserText, const char* fileExt)
{
	if (tuningNumber >= TUNING_COUNT)
		return;
	
	if (tunType >= FILETYPE_COUNT)
		return;
	
	chooser = std::make_unique<juce::FileChooser> (chooserText,
		juce::File(audioProcessor.getStringState(kStateLastOpenedLoadPath)),
		fileExt);
	auto chooserFlags = juce::FileBrowserComponent::openMode
						| juce::FileBrowserComponent::canSelectFiles;
	chooser->launchAsync (chooserFlags, [this, tuningNumber, tunType, fileExt] (const juce::FileChooser& fc)
		{
			auto file = fc.getResult();
			// This is true if a file was chosen, false if the dialog was cancelled
			if (file != juce::File {})
			{
				if (file.getFileExtension() == juce::String(fileExt).trimCharactersAtStart("*"))
				{
					if (!file.existsAsFile())
						return;
					auto fileText = file.loadFileAsString();
					
					// Should be true if the new scale was applied successfully
					if (applyNewTuningFromEditor(tuningNumber, fileText, tunType))
					{
						// Update Scale Name stringState
						audioProcessor.setNameStateForTuning(tuningNumber, file.getFileName(), tunType);
						audioProcessor.setStringState( kStateLastOpenedLoadPath, file.getParentDirectory().getFullPathName() );
					}
				}
				else
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Couldn't apply file", "Wrong file type!");
				}
				// Update UI.
				updateAllNameLabels();
				repaint();
			}
		});
}

void ScaleSpaceAudioProcessorEditor::exportButtonClicked(const uint32_t tunType, const char* chooserText, const char* fileExt)
{
    if (!editorWindow)
    {
        chooser = std::make_unique<juce::FileChooser> (chooserText,
            juce::File(audioProcessor.getStringState(kStateFileSavePath)),
            fileExt);
        auto chooserFlags = juce::FileBrowserComponent::saveMode
                            | juce::FileBrowserComponent::canSelectFiles
                            | juce::FileBrowserComponent::warnAboutOverwriting;
        chooser->launchAsync (chooserFlags, [this, tunType] (const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                // This is true if a file was chosen, false if the dialog was cancelled
                if (file != juce::File {})
                {
                    if (tunType == SCL)
                        audioProcessor.exportCurrentSclTuning(file.getFullPathName());
                    else if (tunType == KBM)
                        audioProcessor.exportCurrentKbmTuning(file.getFullPathName());
                }
            });
    }
    else
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is open", "Please close the Tuning Editor window before exporting!");
	}
}

void ScaleSpaceAudioProcessorEditor::updateAllNameLabels()
{
    updateNameLabel (scaleBox1.sclNameLabel, kStateNameSCL1);
    updateNameLabel (scaleBox1.kbmNameLabel, kStateNameKBM1);
    updateNameLabel (scaleBox2.sclNameLabel, kStateNameSCL2);
    updateNameLabel (scaleBox2.kbmNameLabel, kStateNameKBM2);
    updateNameLabel (scaleBox3.sclNameLabel, kStateNameSCL3);
    updateNameLabel (scaleBox3.kbmNameLabel, kStateNameKBM3);
    updateNameLabel (scaleBox4.sclNameLabel, kStateNameSCL4);
    updateNameLabel (scaleBox4.kbmNameLabel, kStateNameKBM4);
    updateNameLabel (scaleBox5.sclNameLabel, kStateNameSCL5);
    updateNameLabel (scaleBox5.kbmNameLabel, kStateNameKBM5);
    updateNameLabel (scaleBox6.sclNameLabel, kStateNameSCL6);
    updateNameLabel (scaleBox6.kbmNameLabel, kStateNameKBM6);
    updateNameLabel (scaleBox7.sclNameLabel, kStateNameSCL7);
    updateNameLabel (scaleBox7.kbmNameLabel, kStateNameKBM7);
    updateNameLabel (scaleBox8.sclNameLabel, kStateNameSCL8);
    updateNameLabel (scaleBox8.kbmNameLabel, kStateNameKBM8);
}

void ScaleSpaceAudioProcessorEditor::updateNameLabel(juce::Label &label, const uint32_t nameIndex)
{
    if (label.getFont().getHeight() <= 0.0f)
        return;
    
    label.setText (audioProcessor.getStringState(nameIndex), juce::dontSendNotification);
    // Rough approximation of whether to set a tooltip, i.e. try to estimate if the name fits in the scale box
    if ((audioProcessor.getStringState(nameIndex).length() / 2.2f) > (label.getWidth() / label.getFont().getHeight()))
        label.setTooltip (audioProcessor.getStringState(nameIndex));
    else
        label.setTooltip ("");
}

bool ScaleSpaceAudioProcessorEditor::isInterestedInFileDrag (const juce::StringArray &files)
{
    if (files.size() !=1)
        return false;
    
    for (auto i = files.begin(); i != files.end(); ++i)
    {
        if (i->endsWith(".scl") || i->endsWith(".kbm"))
        {
            return true;
        }
    }
    return false;
}

void ScaleSpaceAudioProcessorEditor::filesDropped (const juce::StringArray &files, int x, int y)
{
    if (files.size() !=1)
        return;
        
    for (auto i = files.begin(); i != files.end(); ++i)
    {
        if (i->endsWith(".scl") || i->endsWith(".kbm"))
        {
            uint32_t scaleToApply = TUNING_COUNT;
            
            if ( (x > scaleBox1.getX()) && (x < scaleBox1.getRight()) && (y > scaleBox1.getY()) && (y < scaleBox1.getBottom()) )
                scaleToApply = TUNING_1;
            else if ( (x > scaleBox2.getX()) && (x < scaleBox2.getRight()) && (y > scaleBox2.getY()) && (y < scaleBox2.getBottom()) )
                scaleToApply = TUNING_2;
            else if ( (x > scaleBox3.getX()) && (x < scaleBox3.getRight()) && (y > scaleBox3.getY()) && (y < scaleBox3.getBottom()) )
                scaleToApply = TUNING_3;
            else if ( (x > scaleBox4.getX()) && (x < scaleBox4.getRight()) && (y > scaleBox4.getY()) && (y < scaleBox4.getBottom()) )
                scaleToApply = TUNING_4;
            else if ( *audioProcessor.getApvts().getRawParameterValue("threeDimensional") && (x > scaleBox5.getX()) && (x < scaleBox5.getRight()) && (y > scaleBox5.getY()) && (y < scaleBox5.getBottom()) )
                scaleToApply = TUNING_5;
            else if ( *audioProcessor.getApvts().getRawParameterValue("threeDimensional") && (x > scaleBox6.getX()) && (x < scaleBox6.getRight()) && (y > scaleBox6.getY()) && (y < scaleBox6.getBottom()) )
                scaleToApply = TUNING_6;
            else if ( *audioProcessor.getApvts().getRawParameterValue("threeDimensional") && (x > scaleBox7.getX()) && (x < scaleBox7.getRight()) && (y > scaleBox7.getY()) && (y < scaleBox7.getBottom()) )
                scaleToApply = TUNING_7;
            else if ( *audioProcessor.getApvts().getRawParameterValue("threeDimensional") && (x > scaleBox8.getX()) && (x < scaleBox8.getRight()) && (y > scaleBox8.getY()) && (y < scaleBox8.getBottom()) )
                scaleToApply = TUNING_8;
                
            if (scaleToApply != TUNING_COUNT)
            {
                if (audioProcessor.applyDroppedFile(scaleToApply, i->toStdString()))
                {
                    if (tuningEditor && audioProcessor.currentEditedTuning == scaleToApply)
                        tuningEditor->setupForTuning(scaleToApply);
                }
            }
        }
    }
    // Update UI.
    updateAllNameLabels();
    repaint();
}

// applyNewTuningFromEditor is for files opened from, or dropped onto, the main editor window.
bool ScaleSpaceAudioProcessorEditor::applyNewTuningFromEditor(const uint32_t tuningNumber, const juce::String & tunData, const uint32_t tunType)
{
    bool success = false;
    
    if (audioProcessor.editedTuningValid(tuningNumber, tunData, tunType))
    {
        // The tuning validity has already been checked but the following try - catches
        // have kept kept just in case...
        if (tunType == SCL)
        {
            try
            {
                auto k = audioProcessor.getTuning(tuningNumber).keyboardMapping;
                auto s = Tunings::parseSCLData(tunData.toStdString());
                audioProcessor.getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                audioProcessor.setStringState( audioProcessor.getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                if ( (tuningEditor) && (editorWindow) && (audioProcessor.currentEditedTuning == tuningNumber) )
                    tuningEditor->setupForTuning(tuningNumber);
                
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying SCL. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                audioProcessor.resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
        else if (tunType == KBM)
        {
            try
            {
                auto s = audioProcessor.getTuning(tuningNumber).scale;
                auto k = Tunings::parseKBMData(tunData.toStdString());
                audioProcessor.getTuning(tuningNumber) = Tunings::Tuning(s, k).withSkippedNotesInterpolated();
                
                audioProcessor.setStringState( audioProcessor.getFileIndexFromTuningAndFileType(tuningNumber, tunType), tunData);
                
                if ( (tuningEditor) && (editorWindow) && (audioProcessor.currentEditedTuning == tuningNumber) )
                    tuningEditor->setupForTuning(tuningNumber);
                    
                success = true;
            }
            catch (const std::exception& e)
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Problem applying KBM. Scale reset.", e.what());
                // Reset stringState information for this tuning's SCL and KBM
                audioProcessor.resetTuningStringStatesToStandard(tuningNumber);
                success = false;
            }
        }
    }
    return success;
}

void ScaleSpaceAudioProcessorEditor::dimensionModeButtonClicked()
{
    if (*audioProcessor.getApvts().getRawParameterValue("threeDimensional"))
    {
        multiPad.threeDimensionModeActive(true);
        resized();
    }
    else
    {
        multiPad.threeDimensionModeActive(false);
        resized();
    }
    updateAllNameLabels();
}

void ScaleSpaceAudioProcessorEditor::openEditorButtonClicked(const uint32_t tuningNumber)
{
    if (!editorWindow)
    {
        openTuningEditor(tuningNumber, getApproximateScaleFactorForComponent(this));
        
        // TODO Don't use this timer to update the UI while the tuning editor is open
        startTimerHz(10);
    }
    else if ( audioProcessor.currentEditedTuning == tuningNumber )
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is already open", "The Tuning Editor window is already open for this scale.");
	}
	else
    {
		AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Editor window is already open", "The Tuning Editor window is already open for another scale. Please close the current tuning editor window and try again!");
	}
}

void ScaleSpaceAudioProcessorEditor::openTuningEditor(const uint32_t tuningNumber, const float desktopScaleFactor)
{
    if (!editorWindow && tuningEditor)
    {
        audioProcessor.currentEditedTuning = tuningNumber;
        tuningEditor->setupForTuning(tuningNumber);
        tuningEditor->setDesktopScaleFactor(desktopScaleFactor);
        tuningEditor->startTimerHz(50);
        editorWindow = new ExternalTuningEditorWindow("Edit Tuning for Scale " + juce::String(tuningNumber + 1), Colours::grey, DocumentWindow::closeButton);
        editorWindow->setUsingNativeTitleBar(false);
        editorWindow->setResizable(false, true);
        editorWindow->setContentNonOwned(tuningEditor.get(), true);
        editorWindow->centreWithSize(1000 * desktopScaleFactor, 600 * desktopScaleFactor);
        editorWindow->setVisible(true);
    }
}

// TODO Don't use this timer to update the UI while the tuning editor is open
void ScaleSpaceAudioProcessorEditor::timerCallback()
{
    if (initialUIUpdate)
    {
        updateAllNameLabels();
        repaint();
        initialUIUpdate = false;
        stopTimer();
    }
    else if (editorWindow)
    {
        if (audioProcessor.paintFlag)
        {
            updateAllNameLabels();
            repaint();
            audioProcessor.paintFlag = false;
        }
    }
    else
    {
        tuningEditor->stopTimer();
        stopTimer();
    }
}
