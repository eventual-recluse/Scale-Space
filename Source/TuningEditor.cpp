#include "TuningEditor.h"
#include "PluginEditor.h"

class TuningTableListBoxModel : public juce::TableListBoxModel
{
  public:
    TuningEditor *parent;
    TuningTableListBoxModel(TuningEditor *p) : parent(p) {}
    ~TuningTableListBoxModel() { table = nullptr; }

    void setTableListBox(juce::TableListBox *t) { table = t; }

    void setupDefaultHeaders(juce::TableListBox *table)
    {
        table->setHeaderHeight(14 * scaleTunEd);
        table->setRowHeight(14 * scaleTunEd);
        table->getHeader().addColumn("Note", 1, 32 * scaleTunEd, 32 * scaleTunEd, 32 * scaleTunEd,
                                     juce::TableHeaderComponent::ColumnPropertyFlags::visible);
        table->getHeader().addColumn("Frequency (Hz)", 2, 52 * scaleTunEd, 52 * scaleTunEd, 52 * scaleTunEd,
                                     juce::TableHeaderComponent::ColumnPropertyFlags::visible);
        table->getHeader().setPopupMenuActive(false);
    }

    virtual int getNumRows() override { return 128; }
    virtual void paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height,
                                    bool rowIsSelected) override
    {
        if (!table)
            return;

        g.fillAll(separatorTunEdColor);
    }

    int mcoff{1};

    void setMiddleCOff(int m)
    {
        mcoff = m;

        if (table)
        {
            table->repaint();
        }
    }

    struct TuningRowComp : juce::Component
    {
        TuningTableListBoxModel &parent;
        TuningRowComp(TuningTableListBoxModel &m) : parent(m) {}

        int rowNumber{0}, columnID{0};
        void paint(juce::Graphics &g) override
        {
            auto width = getWidth();
            auto height = getHeight();

            int noteInScale = rowNumber % 12;
            bool whitekey = true;
            bool noblack = false;

            if ((noteInScale == 1 || noteInScale == 3 || noteInScale == 6 || noteInScale == 8 ||
                 noteInScale == 10))
            {
                whitekey = false;
            }

            if (noteInScale == 4 || noteInScale == 11)
            {
                noblack = true;
            }

            // black key
            auto kbdColour = blackKeyTunEdColor;

            if (whitekey)
            {
                kbdColour = whiteKeyTunEdColor;
            }

            bool no = false;
            auto pressedColour = pressedKeyTunEdColor;

            if (parent.notesOn[rowNumber])
            {
                no = true;
                kbdColour = pressedColour;
            }

            g.fillAll(kbdColour);

            if (!whitekey && columnID != 1)
            {
                g.setColour(separatorTunEdColor);
                // draw an inset top and bottom
                g.fillRect(0, 0, width - 1, 1);
                g.fillRect(0, height - 1, width - 1, 1);
            }

            int txtOff = 0;

            if (columnID == 1)
            {
                // black key
                if (!whitekey)
                {
                    txtOff = 10;
                    // "black key"
                    auto kbdColour = blackKeyTunEdColor;
                    auto kbc = whiteKeyTunEdColor;

                    g.setColour(kbc);
                    g.fillRect(-1, 0, txtOff, height + 2);

                    // OK so now check neighbors
                    if (rowNumber > 0 && parent.notesOn[rowNumber - 1])
                    {
                        g.setColour(pressedColour);
                        g.fillRect(0, 0, txtOff, height / 2);
                    }

                    if (rowNumber < 127 && parent.notesOn[rowNumber + 1])
                    {
                        g.setColour(pressedColour);
                        g.fillRect(0, height / 2, txtOff, height / 2 + 1);
                    }

                    g.setColour(blackKeyTunEdColor);
                    g.fillRect(0, height / 2, txtOff, 1);

                    if (no)
                    {
                        g.fillRect(txtOff, 0, width - 1 - txtOff, 1);
                        g.fillRect(txtOff, height - 1, width - 1 - txtOff, 1);
                        g.fillRect(txtOff, 0, 1, height - 1);
                    }
                }
            }

            auto mn = rowNumber;
            double fr = 0;

            const auto &tuning = parent.tuning;
            
            fr = tuning.frequencyForMidiNote(mn);

            std::string notenum, notename, display;

            g.setColour(separatorTunEdColor);
            g.fillRect(width - 1, 0, 1, height);

            if (noblack)
            {
                g.fillRect(0, height - 1, width, 1);
            }

            g.setColour(textTunEdColor);

            if (no)
            {
                g.setColour(pressedKeyTextTunEdColor);
            }

            int margin = 9;
            auto just_l = juce::Justification::centredLeft;
            auto just_r = juce::Justification::centredRight;

            switch (columnID)
            {
            case 1:
            {
                notenum = std::to_string(mn);
                notename = noteInScale % 12 == 0
                               ? fmt::format("C{:d}", rowNumber / 12 - parent.mcoff)
                               : "";

                g.setFont(boldTunEdFont.withHeight(9.0f * scaleTunEd));
                g.drawText(notename, 2 + txtOff, 0, width - margin, height, just_l, false);
                g.setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));
                g.drawText(notenum, 2 + txtOff, 0, width - txtOff - margin, height,
                           juce::Justification::centredRight, false);

                break;
            }
            case 2:
            {
                display = fmt::format("{:.2f}", fr);
                g.setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));
                g.drawText(display, 2 + txtOff, 0, width - margin, height, just_r, false);
                break;
            }
            }
        }
        
        void mouseDown(const juce::MouseEvent &event) override
        {
            parent.parent->audioProcessor->setNoteOn(rowNumber);
        }

        void mouseUp(const juce::MouseEvent &event) override
        {
            parent.parent->audioProcessor->setNoteOff(rowNumber);
        }
    };

    juce::Component *refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                             juce::Component *existingComponentToUpdate) override
    {
        if (existingComponentToUpdate == nullptr)
            existingComponentToUpdate = new TuningRowComp(*this);

        auto trc = static_cast<TuningRowComp *>(existingComponentToUpdate);

        trc->rowNumber = rowNumber;
        trc->columnID = columnId;

        return existingComponentToUpdate;
    }

    void paintCell(juce::Graphics &, int rowNumber, int columnId, int width, int height,
                   bool rowIsSelected) override
    {
    }

    virtual void tuningUpdated(const Tunings::Tuning &newTuning)
    {
        tuning = newTuning;
        if (table)
            table->repaint();
    }

    Tunings::Tuning tuning;
    std::bitset<128> notesOn;
    std::unique_ptr<juce::PopupMenu> rmbMenu;
    juce::TableListBox *table{nullptr};
};

class RadialScaleGraph;

class InfiniteKnob : public juce::Component
{
  public:
    InfiniteKnob() : juce::Component(), angle(0) {}

    virtual void mouseDown(const juce::MouseEvent &event) override
    {
        juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(true);
        
        lastDrag = 0;
        isDragging = true;
        repaint();
    }

    virtual void mouseDrag(const juce::MouseEvent &event) override
    {
        int d = -(event.getDistanceFromDragStartX() + event.getDistanceFromDragStartY());
        int diff = d - lastDrag;
        lastDrag = d;
        if (diff != 0)
        {
            float mul = 1.0;
            if (event.mods.isShiftDown())
            {
                mul = 0.05;
            }
            angle += diff * mul;
            onDragDelta(diff * mul);
            repaint();
        }
    }

    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override
    {
        /*
         * If I choose based on horiz/vert it only works on trackpads, so just add
         */
        float delta = wheel.deltaX - (wheel.isReversed ? 1 : -1) * wheel.deltaY;
        if (delta == 0)
            return;

#if MAC
        float speed = 1.2;
#else
        float speed = 0.42666;
#endif
        if (event.mods.isShiftDown())
        {
            speed = speed / 10;
        }

        // This is calibrated to give us reasonable speed on a 0-1 basis from the slider
        // but in this widget '1' is the small motion so speed it up some
        speed *= 30;

        angle += delta * speed;
        onDragDelta(delta * speed);
        repaint();
    }

    virtual void mouseUp(const juce::MouseEvent &e) override
    {
        
        // CHECK
        juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(false);
        auto p = localPointToGlobal(e.mouseDownPosition);
        juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(p);
        // ----

        isDragging = false;
        repaint();
    }

    virtual void mouseDoubleClick(const juce::MouseEvent &e) override
    {
        onDoubleClick();
        angle = 0;
        repaint();
    }

    virtual void paint(juce::Graphics &g) override
    {
        int w = getWidth();
        int h = getHeight();
        int b = std::min(w, h);
        float r = b / 2.0;
        float dx = (w - b) / 2.0;
        float dy = (h - b) / 2.0;

        g.saveState();
        g.addTransform(juce::AffineTransform::translation(dx, dy));
        g.addTransform(juce::AffineTransform::translation(r, r));
        g.addTransform(
            juce::AffineTransform::rotation(angle / 50.0 * 2.0 * juce::MathConstants<double>::pi));

        if (isHovered)
        {
            g.setColour(radialKnobFillHoverTunEdColor);
        }
        else
        {
            g.setColour(radialKnobFillTunEdColor);
        }

        g.fillEllipse(-(r - 3), -(r - 3), (r - 3) * 2, (r - 3) * 2);
        g.setColour(radialKnobBorderTunEdColor);
        g.drawEllipse(-(r - 3), -(r - 3), (r - 3) * 2, (r - 3) * 2, r / 5.0);

        if (enabled)
        {
            if (isPlaying)
            {
                g.setColour(radialKnobPlayedTunEdColor);
            }
            else
            {
                g.setColour(radialKnobThumbTunEdColor);
            }

            g.drawLine(0, -(r - 1), 0, r - 1, r / 3.0);
        }

        g.restoreState();
    }

    bool isHovered = false;

    void mouseEnter(const juce::MouseEvent &e) override
    {
        isHovered = true;
        repaint();
    }

    void mouseExit(const juce::MouseEvent &e) override
    {
        isHovered = false;
        repaint();
    }

    int lastDrag = 0;
    bool isDragging = false;
    bool isPlaying{false};
    float angle;
    std::function<void(float)> onDragDelta = [](float f) {};
    std::function<void()> onDoubleClick = []() {};
    bool enabled = true;
};


class RadialScaleGraph : public juce::Component
                         , public juce::TextEditor::Listener
                         , public juce::Button::Listener
{
  public:
    RadialScaleGraph(TuningEditor *p) : parent(p)
    {
        toneList = std::make_unique<juce::Viewport>();
        toneInterior = std::make_unique<juce::Component>();
        toneList->setViewedComponent(toneInterior.get(), false);
        addAndMakeVisible(*toneList);

        radialModeButton = std::make_unique<juce::TextButton>();
        radialModeButton -> setButtonText("Radial");
        angularModeButton = std::make_unique<juce::TextButton>();
        angularModeButton -> setButtonText("Angular");
        radialModeButton->addListener(this);
        angularModeButton->addListener(this);
        radialModeButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
        radialModeButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
        radialModeButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
        radialModeButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
        radialModeButton->setRadioGroupId(PolarModeButtonGroup);
        radialModeButton->setClickingTogglesState (true);
        radialModeButton->setToggleState(true, juce::dontSendNotification);
        angularModeButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
        angularModeButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
        angularModeButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
        angularModeButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
        angularModeButton->setRadioGroupId(PolarModeButtonGroup);
        angularModeButton->setClickingTogglesState (true);
        addAndMakeVisible(*radialModeButton);
        addAndMakeVisible(*angularModeButton);

        setTuning(Tunings::Tuning(Tunings::evenTemperament12NoteScale(), Tunings::tuneA69To(440)));
    }

    void setTuning(const Tunings::Tuning &t)
    {
        int priorLen = tuning.scale.count;
        tuning = t;
        scale = t.scale;

        if (toneEditors.empty() || priorLen != scale.count || toneEditors.size() != scale.count)
        {
            toneInterior->removeAllChildren();
            auto w = usedForSidebar - 10;
            auto m = 4;
            auto h = 23;
            auto labw = 18 * scaleTunEd;

            toneInterior->setSize(w, (scale.count + 1) * (h + m + (4 * scaleTunEd)));
            toneEditors.clear();
            toneKnobs.clear();
            toneLabels.clear();

            for (int i = 0; i < scale.count + 1; ++i)
            {
                auto totalR = juce::Rectangle<int>(m, i * (h + m) + m, w - 2 * m, h);
                int hideHeight = 18 * scaleTunEd;
                int vertOffset = 4 * scaleTunEd;

                auto tl = std::make_unique<juce::Label>("tone index");
                tl->setFont(plainTunEdFont.withHeight(10.0f * scaleTunEd));
                tl->setText(std::to_string(i), juce::NotificationType::dontSendNotification);
                tl->setBounds(totalR.withWidth(labw).withY(totalR.getY() + vertOffset));
                tl->setJustificationType(juce::Justification::centredRight);
                toneInterior->addAndMakeVisible(*tl);
                toneLabels.push_back(std::move(tl));

                if (i == 0)
                {
                    auto tk = std::make_unique<InfiniteKnob>();

                    tk->setBounds(totalR.withX(totalR.getWidth() - h).withWidth(h).withY(totalR.getY() + vertOffset / 2).reduced(2));
                    tk->onDragDelta = [this](float f) {
                        selfEditGuard++;
                        onScaleRescaled(f);
                        selfEditGuard--;
                    };
                    tk->onDoubleClick = [this]() {
                        selfEditGuard++;
                        onScaleRescaledAbsolute(1200.0);
                        selfEditGuard--;
                    };
                    
                    toneInterior->addAndMakeVisible(*tk);
                    toneKnobs.push_back(std::move(tk));

                    hideBtn = std::make_unique<juce::TextButton>();
                    hideBtn->setButtonText({"Hide"});
                    hideBtn->setToggleable(true);
                    hideBtn->addListener(this);

                    hideBtn->setBounds(
                        totalR.withTrimmedLeft(labw + m).withTrimmedRight(h + m).withHeight(hideHeight).reduced(3));

                    toneInterior->addAndMakeVisible(*hideBtn);
                }
                else
                {
                    auto te = std::make_unique<juce::TextEditor>("tone");
                    te->setBounds(totalR.withTrimmedLeft(labw + m).withTrimmedRight(h + m).withY(totalR.getY() + vertOffset));
                    te->setJustification((juce::Justification::verticallyCentred));
                    te->setFont(plainMonoTunEdFont.withHeight(10.0f * scaleTunEd));
                    te->setIndents(4, (te->getHeight() - te->getTextHeight()) / 2);
                    te->setText(std::to_string(i), juce::NotificationType::dontSendNotification);
                    te->setEnabled(i != 0);
                    te->addListener(this);
                    te->setSelectAllWhenFocused(true);

                    te->onEscapeKey = [this]() { giveAwayKeyboardFocus(); };
                    toneInterior->addAndMakeVisible(*te);
                    toneEditors.push_back(std::move(te));

                    auto tk = std::make_unique<InfiniteKnob>();

                    tk->setBounds(totalR.withX(totalR.getWidth() - h).withWidth(h).withY(totalR.getY() + vertOffset).reduced(2));
                    tk->onDragDelta = [this, i](float f) {
                        int idx = i;
                        if (idx == 0)
                            idx = scale.count;
                        auto ct = scale.tones[idx - 1].cents;
                        ct += f;
                        selfEditGuard++;
                        onToneChanged(idx - 1, ct);
                        selfEditGuard--;
                    };

                    tk->onDoubleClick = [this, i]() {
                        auto ri = scale.tones[scale.count - 1].cents;
                        auto nc = ri / scale.count * i;
                        selfEditGuard++;
                        onToneChanged(i - 1, nc);
                        selfEditGuard--;
                    };

                    toneInterior->addAndMakeVisible(*tk);
                    toneKnobs.push_back(std::move(tk));
                }
            }
        }

        for (int i = 0; i < scale.count; ++i)
        {
            auto td = fmt::format("{:.5f}", scale.tones[i].cents);
            if (scale.tones[i].type == Tunings::Tone::kToneRatio)
            {
                td = fmt::format("{:d}/{:d}", scale.tones[i].ratio_n, scale.tones[i].ratio_d);
            }
            toneEditors[i]->setText(td, juce::NotificationType::dontSendNotification);
        }

        notesOn.clear();
        notesOn.resize(scale.count);
        for (int i = 0; i < scale.count; ++i)
            notesOn[i] = false;
        setNotesOn(bitset);

        if (selfEditGuard == 0)
        {
            // Someone dragged something onto me or something. Reset the tuning knobs
            for (const auto &tk : toneKnobs)
            {
                tk->angle = 0;
                tk->repaint();
            }
        }
    }

    bool centsShowing{true};

    enum struct DisplayMode
    {
        RADIAL,
        ANGULAR
    } displayMode{DisplayMode::RADIAL};

    void buttonClicked (juce::Button* buttonThatWasClicked) override
    {
        if (buttonThatWasClicked == hideBtn.get())
        {
            centsShowing = !centsShowing;
            for (const auto &t : toneEditors)
            {
                t->setPasswordCharacter(centsShowing ? 0 : 0x2022);
            }
            hideBtn->setButtonText({centsShowing ? "Hide" : "Show"});
        }
        if (buttonThatWasClicked == angularModeButton.get())
        {
            displayMode = DisplayMode::ANGULAR;
            parent->setPolarMode(1);
        }
        if (buttonThatWasClicked == radialModeButton.get())
        {
            displayMode = DisplayMode::RADIAL;
            parent->setPolarMode(0);
        }
        repaint();
    }
    
    void setDisplayMode(const uint32_t m)
	{
		switch (m)
		{
			case 0:
				displayMode = DisplayMode::RADIAL;
				break;
			case 1:
				displayMode = DisplayMode::ANGULAR;
				break;
			default:
				break;
		}
	}

  private:
    void textEditorReturnKeyPressed(juce::TextEditor &editor) override;
    void textEditorFocusLost(juce::TextEditor &editor) override;
    
    TuningEditor *parent{nullptr};

  public:
    virtual void paint(juce::Graphics &g) override;
    Tunings::Tuning tuning;
    Tunings::Scale scale;
    std::vector<juce::Rectangle<float>> screenHotSpots;
    int hotSpotIndex = -1, wheelHotSpotIndex = -1;
    double dInterval, centsAtMouseDown, angleAtMouseDown, dIntervalAtMouseDown;

    int selfEditGuard{0};

    juce::AffineTransform screenTransform, screenTransformInverted;
    std::function<void(int index, double)> onToneChanged = [](int, double) {};
    std::function<void(int index, const std::string &s)> onToneStringChanged =
        [](int, const std::string &) {};
    std::function<void(double)> onScaleRescaled = [](double) {};
    std::function<void(double)> onScaleRescaledAbsolute = [](double) {};
    static constexpr int usedForSidebar = 185;

    std::unique_ptr<juce::Viewport> toneList;
    std::unique_ptr<juce::Component> toneInterior;
    std::vector<std::unique_ptr<juce::TextEditor>> toneEditors;
    std::vector<std::unique_ptr<juce::Label>> toneLabels;
    std::vector<std::unique_ptr<InfiniteKnob>> toneKnobs;
    std::unique_ptr<juce::TextButton> hideBtn, radialModeButton, angularModeButton;

    void resized() override
    {
        auto rmh = 20;
        toneList->setBounds(0, 0, usedForSidebar, getHeight() - rmh - 4);
        radialModeButton->setBounds(0, getHeight() - rmh - 2, usedForSidebar / 2, rmh);
        angularModeButton->setBounds(usedForSidebar / 2, getHeight() - rmh - 2, usedForSidebar / 2, rmh);
    }

    std::vector<bool> notesOn;
    std::bitset<128> bitset{0};
    void setNotesOn(const std::bitset<128> &bs)
    {
        bitset = bs;

        for (int i = 0; i < scale.count; ++i)
        {
            notesOn[i] = false;
        }

        for (int i = 0; i < 128; ++i)
        {
            if (bitset[i])
            {
                notesOn[tuning.scalePositionForMidiNote(i)] = true;
            }
        }

        for (auto &a : toneKnobs)
        {
            a->isPlaying = false;
        }

        for (int i = 0; i < scale.count; ++i)
        {
            auto ni = (i + 1) % (scale.count);

            if (notesOn[ni])
            {
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                                          radialTonelabelBackgroundPlayedTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::outlineColourId,
                                          radialTonelabelBorderPlayedTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                                          radialTonelabelBorderPlayedTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::textColourId,
                                          radialTonelabelTextPlayedTunEdColor);
                toneEditors[i]->applyColourToAllText(radialTonelabelTextPlayedTunEdColor,
                                                     true);

                toneLabels[i + 1]->setColour(juce::Label::ColourIds::textColourId,
                                             radialTonelabelPlayedTunEdColor);
                toneKnobs[i + 1]->isPlaying = true;

                if (i == scale.count - 1)
                {
                    toneLabels[0]->setColour(juce::Label::ColourIds::textColourId,
                                             radialTonelabelPlayedTunEdColor);
                    toneKnobs[0]->isPlaying = true;
                }
            }
            else
            {
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                                          radialTonelabelBackgroundTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::outlineColourId,
                                          radialTonelabelBorderTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                                          radialTonelabelBorderTunEdColor);
                toneEditors[i]->setColour(juce::TextEditor::ColourIds::textColourId,
                                          radialTonelabelTextTunEdColor);
                toneEditors[i]->applyColourToAllText(radialTonelabelTextTunEdColor, true);

                toneLabels[i + 1]->setColour(juce::Label::ColourIds::textColourId,
                                             radialTonelabelTunEdColor);
                if (i == scale.count - 1)
                    toneLabels[0]->setColour(juce::Label::ColourIds::textColourId,
                                             radialTonelabelTunEdColor);
            }

            toneEditors[i]->repaint();
            toneLabels[i + 1]->repaint();
            toneLabels[0]->repaint();
        }

        toneInterior->repaint();
        toneList->repaint();
        repaint();
    }

    int whichSideOfZero = 0;

    juce::Point<float> lastDragPoint;
    void mouseMove(const juce::MouseEvent &e) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseDoubleClick(const juce::MouseEvent &e) override;
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;
};

void RadialScaleGraph::paint(juce::Graphics &g)
{
    if (notesOn.size() != scale.count)
    {
        notesOn.clear();
        notesOn.resize(scale.count);

        for (int i = 0; i < scale.count; ++i)
        {
            notesOn[i] = 0;
        }
    }

    g.fillAll(radialBackgroundTunEdColor);

    int w = getWidth() - usedForSidebar - 25;
    int h = getHeight();
    float r = std::min(w, h) / 2.1;
    float xo = (w - 2 * r) / 2.0;
    float yo = (h - 2 * r) / 2.0;
    double outerRadiusExtension = 0.4;

    g.saveState();

    screenTransform = juce::AffineTransform()
                          .scaled(1.0 / (1.0 + outerRadiusExtension * 1.1))
                          .scaled(r, -r)
                          .scaled(0.64 * scaleTunEd, 0.64 * scaleTunEd)
                          .translated(r, r)
                          .translated(xo, yo)
                          .translated(usedForSidebar, 0);
    screenTransformInverted = screenTransform.inverted();

    g.addTransform(screenTransform);

    // We are now in a normal x y 0 1 coordinate system with 0, 0 at the center. Cool

    // So first things first - scan for range.
    double ETInterval = scale.tones.back().cents / scale.count;
    double dIntMin = 0, dIntMax = 0;
    double intMin = std::numeric_limits<double>::max(), intMax = std::numeric_limits<double>::min();
    double pCents = 0;
    std::vector<float> intervals; // between tone i and i-1

    for (int i = 0; i < scale.count; ++i)
    {
        auto t = scale.tones[i];
        auto c = t.cents;
        auto in = c - pCents;

        intMin = std::min(intMin, in);
        intMax = std::max(intMax, in);
        pCents = c;
        intervals.push_back(in);

        auto intervalDistance = (c - ETInterval * (i + 1)) / ETInterval;

        dIntMax = std::max(intervalDistance, dIntMax);
        dIntMin = std::min(intervalDistance, dIntMin);
    }

    // intmin < 0 flash warning in angular
    double range = std::max(0.01, std::max(dIntMax, -dIntMin / 2.0)); // twice as many inside rings
    int iRange = std::ceil(range);

    dInterval = outerRadiusExtension / iRange;

    double nup = iRange;
    double ndn = (int)(iRange * 1.6);

    // Now draw the interval circles
    if (displayMode == DisplayMode::RADIAL)
    {
        for (int i = -ndn; i <= nup; ++i)
        {
            if (i == 0)
            {
            }
            else
            {
                float pos = 1.0 * std::abs(i) / ndn;
                float cpos = std::max(0.f, pos);

                g.setColour(juce::Colour(110, 110, 120)
                                .interpolatedWith(getLookAndFeel().findColour(
                                                      juce::ResizableWindow::backgroundColourId),
                                                  cpos * 0.8));

                float rad = 1.0 + dInterval * i;

                g.drawEllipse(-rad, -rad, 2 * rad, 2 * rad, 0.01);
            }
        }

        for (int i = 0; i < scale.count; ++i)
        {
            double frac = 1.0 * i / (scale.count);
            double hfrac = 0.5 / scale.count;
            double sx = std::sin(frac * 2.0 * juce::MathConstants<double>::pi);
            double cx = std::cos(frac * 2.0 * juce::MathConstants<double>::pi);

            if (notesOn[i])
            {
                g.setColour(juce::Colour(255, 255, 255));
            }
            else
            {
                g.setColour(juce::Colour(110, 110, 120));
            }

            g.drawLine(0, 0, (1.0 + outerRadiusExtension) * sx, (1.0 + outerRadiusExtension) * cx,
                       0.01);

            if (centsShowing)
            {
                if (scale.count > 48)
                {
                    // we just don't have room to draw the intervals
                }
                auto rsf = 0.01;
                auto irsf = 1.0 / rsf;

                if (scale.count > 18)
                {
                    // draw them sideways
                    juce::Graphics::ScopedSaveState gs(g);
                    auto t = juce::AffineTransform()
                                 .scaled(-0.7, 0.7)
                                 .scaled(rsf, rsf)
                                 .rotated(juce::MathConstants<double>::pi)
                                 .translated(1.05, 0.0)
                                 .rotated((-frac + 0.25 - hfrac) * 2.0 *
                                          juce::MathConstants<double>::pi);

                    g.addTransform(t);
                    g.setColour(juce::Colours::white);
                    g.setFont(plainTunEdFont.withHeight(10.0f * scaleTunEd));

                    auto msg = fmt::format("{:.2f}", intervals[i]);
                    auto tr =
                        juce::Rectangle<float>(0.f * irsf, -0.1f * irsf, 0.6f * irsf, 0.2f * irsf);

                    auto al = juce::Justification::centredLeft;
                    if (frac + hfrac > 0.5)
                    {
                        // left side rotated flip
                        g.addTransform(juce::AffineTransform()
                                           .rotated(juce::MathConstants<double>::pi)
                                           .translated(tr.getWidth(), 0));
                        al = juce::Justification::centredRight;
                    }
                    g.setColour(juce::Colours::white);
                    g.drawText(msg, tr, al);

                    // g.setColour(juce::Colours::red);
                    // g.drawRect(tr, 0.01 * irsf);
                    // g.setColour(juce::Colours::white);
                }
                else
                {
                    juce::Graphics::ScopedSaveState gs(g);

                    auto t = juce::AffineTransform()
                                 .scaled(-0.7, 0.7)
                                 .scaled(rsf, rsf)
                                 .rotated(juce::MathConstants<double>::pi / 2)
                                 .translated(1.05, 0.0)
                                 .rotated((-frac + 0.25 - hfrac) * 2.0 *
                                          juce::MathConstants<double>::pi);

                    g.addTransform(t);
                    g.setColour(juce::Colours::white);
                    g.setFont(plainTunEdFont.withHeight(10.0f * scaleTunEd));

                    auto msg = fmt::format("{:.2f}", intervals[i]);
                    auto tr = juce::Rectangle<float>(-0.3f * irsf, -0.2f * irsf, 0.6f * irsf,
                                                     0.2f * irsf);

                    if (frac + hfrac >= 0.25 && frac + hfrac <= 0.75)
                    {
                        // underneath rotated flip
                        g.addTransform(juce::AffineTransform()
                                           .rotated(juce::MathConstants<double>::pi)
                                           .translated(0, -tr.getHeight()));
                    }

                    g.setColour(juce::Colours::white);
                    g.drawText(msg, tr, juce::Justification::centred);

                    // Useful to debug text layout
                    // g.setColour(juce::Colours::red);
                    // g.drawRect(tr, 0.01 * irsf);
                    // g.setColour(juce::Colours::white);
                }
            }

            g.saveState();
            auto rsf = 0.01;
            auto irsf = 1.0 / rsf * scaleTunEd;

            g.addTransform(juce::AffineTransform::rotation((-frac + 0.25) * 2.0 *
                                                           juce::MathConstants<double>::pi));
            g.addTransform(juce::AffineTransform::translation(1.0 + outerRadiusExtension, 0.0));
            g.addTransform(juce::AffineTransform::rotation(juce::MathConstants<double>::pi * 0.5));
            g.addTransform(juce::AffineTransform::scale(-1.0, 1.0));
            g.addTransform(juce::AffineTransform::scale(rsf, rsf));

            if (notesOn[i])
            {
                g.setColour(juce::Colour(255, 255, 255));
            }
            else
            {
                g.setColour(juce::Colour(200, 200, 240));
            }

            // tone labels
            juce::Rectangle<float> textPos(-0.05 * irsf, -0.115 * irsf, 0.1 * irsf, 0.1 * irsf);
            g.setFont(plainTunEdFont.withHeight(7.5f * scaleTunEd));
            g.drawText(juce::String(i), textPos, juce::Justification::centred, 1);

            // PLease leave - useful for debugging bounding boxes
            //g.setColour(juce::Colours::red);
            //g.drawRect(textPos, 0.01 * irsf);
            //g.drawLine(textPos.getCentreX(), textPos.getY(),
                      //textPos.getCentreX(), textPos.getY() + textPos.getHeight(),
                      //0.01 * irsf);

            g.restoreState();
        }
    }
    else
    {
        for (int i = 0; i < scale.count; ++i)
        {
            double frac = 1.0 * i / (scale.count);
            double hfrac = 0.5 / scale.count;
            double sx = std::sin(frac * 2.0 * juce::MathConstants<double>::pi);
            double cx = std::cos(frac * 2.0 * juce::MathConstants<double>::pi);

            g.setColour(juce::Colour(90, 90, 100));

            float dash[2]{0.03f, 0.02f};
            g.drawDashedLine({0.f, 0.f, (float)sx, (float)cx}, dash, 2, 0.003);
        }
    }
    // Draw the ring at 1.0
    g.setColour(juce::Colour(255, 255, 255));
    g.drawEllipse(-1, -1, 2, 2, 0.01);

    // Then draw ellipses for each note
    screenHotSpots.clear();

    if (displayMode == DisplayMode::RADIAL)
    {
        for (int i = 1; i <= scale.count; ++i)
        {
            double frac = 1.0 * i / (scale.count);
            double sx = std::sin(frac * 2.0 * juce::MathConstants<double>::pi);
            double cx = std::cos(frac * 2.0 * juce::MathConstants<double>::pi);

            auto t = scale.tones[i - 1];
            auto c = t.cents;
            auto expectedC = scale.tones.back().cents / scale.count;

            auto rx = 1.0 + dInterval * (c - expectedC * i) / expectedC;
            float dx = 0.1, dy = 0.1;

            if (i == scale.count)
            {
                dx = 0.15;
                dy = 0.15;
            }

            float x0 = rx * sx - 0.5 * dx, y0 = rx * cx - 0.5 * dy;

            juce::Colour drawColour(200, 200, 200);

            // FIXME - this colormap is bad
            if (rx < 0.99)
            {
                // use a blue here
                drawColour = juce::Colour(200 * (1.0 - 0.6 * rx), 200 * (1.0 - 0.6 * rx), 200);
            }
            else if (rx > 1.01)
            {
                // Use a yellow here
                drawColour = juce::Colour(200, 200, 200 * (rx - 1.0));
            }

            auto originalDrawColor = drawColour;

            if (hotSpotIndex == i - 1)
            {
                drawColour = drawColour.brighter(0.6);
            }

            if (i == scale.count)
            {
                g.setColour(drawColour);
                g.fillEllipse(x0, y0, dx, dy);

                if (hotSpotIndex == i - 1)
                {
                    auto p = juce::Path();
                    if (whichSideOfZero < 0)
                    {
                        p.addArc(x0, y0, dx, dy, juce::MathConstants<float>::pi,
                                 juce::MathConstants<float>::twoPi);
                    }
                    else
                    {
                        p.addArc(x0, y0, dx, dy, 0, juce::MathConstants<float>::pi);
                    }
                    g.setColour(juce::Colour(200, 200, 100));
                    g.fillPath(p);
                }
            }
            else
            {
                g.setColour(drawColour);

                g.fillEllipse(x0, y0, dx, dy);

                if (hotSpotIndex != i - 1)
                {
                    g.setColour(drawColour.brighter(0.6));
                    g.drawEllipse(x0, y0, dx, dy, 0.01);
                }
            }

            if (notesOn[i % scale.count])
            {
                g.setColour(juce::Colour(255, 255, 255));
                g.drawEllipse(x0, y0, dx, dy, 0.02);
            }

            dx += x0;
            dy += y0;
            screenTransform.transformPoint(x0, y0);
            screenTransform.transformPoint(dx, dy);
            screenHotSpots.push_back(juce::Rectangle<float>(x0, dy, dx - x0, y0 - dy));
        }
    }
    else
    {
        g.setColour(juce::Colour(110, 110, 120).interpolatedWith(juce::Colours::black, 0.2));
        auto oor = outerRadiusExtension;
        outerRadiusExtension = oor; //* 0.95;
        g.drawEllipse(-1 - outerRadiusExtension, -1 - outerRadiusExtension,
                      2 + 2 * outerRadiusExtension, 2 + 2 * outerRadiusExtension, 0.005);

        g.setColour(juce::Colour(255, 255, 255));
        auto ps = 0.f;
        for (int i = 1; i <= scale.count; ++i)
        {
            auto dAngle = intervals[i - 1] / scale.tones.back().cents;
            auto dAnglePlus = intervals[i % scale.count] / scale.tones.back().cents;

            auto ca = ps + dAngle / 2;

            {
                juce::Graphics::ScopedSaveState gs(g);

                auto rot = fabs(dAngle) < 1.0 / 22.0;

                auto rsf = 0.01;
                auto irsf = 1.0 / rsf;
                auto t = juce::AffineTransform()
                             .scaled(-0.7, 0.7)
                             .scaled(rsf, rsf)
                             .rotated(juce::MathConstants<double>::pi * (rot ? -1.0 : 0.5))
                             .translated(1.05, 0)
                             .rotated((-ca + 0.25) * 2.0 * juce::MathConstants<double>::pi);

                g.addTransform(t);
                g.setColour(juce::Colours::white);
                auto fs = 0.1 * irsf;
                auto pushFac = 1.0;
                if (fabs(dAngle) < 0.02)
                {
                    pushFac = 1.03;
                    fs *= 0.75;
                }
                if (fabs(dAngle) < 0.012)
                {
                    pushFac = 1.07;
                    fs *= 0.8;
                }
                // and that's as far as we go
                g.setFont(plainTunEdFont.withHeight(10.0f * scaleTunEd));

                auto msg = fmt::format("{:.2f}", intervals[i - 1]);
                auto tr =
                    juce::Rectangle<float>(-0.3f * irsf, -0.2f * irsf, 0.6f * irsf, 0.2f * irsf);
                if (rot)
                    tr = juce::Rectangle<float>(0.02f * irsf, -0.1f * irsf, 0.6f * irsf,
                                                0.2f * irsf);

                auto al = rot ? juce::Justification::centredLeft : juce::Justification::centred;
                if (rot && ca > 0.5)
                {
                    // left side rotated flip
                    g.addTransform(juce::AffineTransform()
                                       .rotated(juce::MathConstants<double>::pi)
                                       .translated(tr.getWidth() * pushFac, 0));
                    al = juce::Justification::centredRight;
                }
                if (!rot && ca > 0.25 && ca < 0.75)
                {
                    // underneat rotated flip
                    g.addTransform(juce::AffineTransform()
                                       .rotated(juce::MathConstants<double>::pi)
                                       .translated(0, -tr.getHeight()));
                }
                g.setColour(juce::Colours::white);
                if (centsShowing)
                    g.drawText(msg, tr, al);
                // Useful to debug text layout
                // g.setColour(rot ? juce::Colours::blue  : juce::Colours::red);
                // g.drawRect(tr, 0.01 * irsf);
                // g.setColour(juce::Colours::white);
            }

            ps += dAngle;

            {
                juce::Graphics::ScopedSaveState gs(g);
                g.addTransform(juce::AffineTransform::rotation((-ps + 0.25) * 2.0 *
                                                               juce::MathConstants<double>::pi));
                g.addTransform(juce::AffineTransform::translation(1.0 + oor, 0.0));
                auto angthresh = 0.013; // inside this rotate
                if ((fabs(dAngle) > angthresh && fabs(dAnglePlus) > angthresh) || (i < 10))
                {
                    g.addTransform(
                        juce::AffineTransform::rotation(juce::MathConstants<double>::pi * 0.5));
                }
                else
                {
                    g.addTransform(juce::AffineTransform::translation(0.05, 0.0));
                    if (ps < 0.5)
                    {
                        g.addTransform(
                            juce::AffineTransform::rotation(juce::MathConstants<double>::pi));
                    }
                }
                g.addTransform(juce::AffineTransform::scale(-1.0, 1.0));

                auto rsf = 0.01;
                auto irsf = 1.0 / rsf * scaleTunEd;
                g.addTransform(juce::AffineTransform::scale(rsf, rsf));

                if (notesOn[i])
                {
                    g.setColour(juce::Colour(255, 255, 255));
                }
                else
                {
                    g.setColour(juce::Colour(200, 200, 240));
                }

                // tone labels
                juce::Rectangle<float> textPos(-0.05 * irsf, -0.115 * irsf, 0.1 * irsf, 0.1 * irsf);
                auto fs = 0.075 * irsf;
                g.setFont(plainTunEdFont.withHeight(7.5f * scaleTunEd));
                g.drawText(juce::String((i == scale.count ? 0 : i)), textPos,
                           juce::Justification::centred, 1);

                // Please leave -useful for debugginb  bounding boxes
                // g.setColour(juce::Colours::red);
                // g.drawRect(textPos, 0.01 * irsf);
                // g.drawLine(textPos.getCentreX(), textPos.getY(),
                //           textPos.getCentreX(), textPos.getY() + textPos.getHeight(),
                //           0.01 * irsf);
            }

            double sx = std::sin(ps * 2.0 * juce::MathConstants<double>::pi);
            double cx = std::cos(ps * 2.0 * juce::MathConstants<double>::pi);

            g.setColour(juce::Colour(110, 110, 120));
            g.drawLine(0, 0, (1.0 + outerRadiusExtension) * sx, (1.0 + outerRadiusExtension) * cx,
                       0.01);
            if (notesOn[i % scale.count])
            {
                g.setColour(juce::Colour(255, 255, 255));
                g.drawLine(0, 0, sx, cx, 0.02);
            }

            auto t = scale.tones[i - 1];
            auto c = t.cents;
            auto expectedC = scale.tones.back().cents / scale.count;

            auto rx = 1.0 + dInterval * (c - expectedC * i) / expectedC;

            float dx = 0.1, dy = 0.1;

            if (i == scale.count)
            {
                dx = 0.15;
                dy = 0.15;
            }

            auto drx = 1.0;
            float x0 = drx * sx - 0.5 * dx, y0 = drx * cx - 0.5 * dy;

            juce::Colour drawColour(200, 200, 200);

            // FIXME - this colormap is bad
            if (rx < 0.99)
            {
                // use a blue here
                drawColour = juce::Colour(200 * (1.0 - 0.6 * rx), 200 * (1.0 - 0.6 * rx), 200);
            }
            else if (rx > 1.01)
            {
                // Use a yellow here
                drawColour = juce::Colour(200, 200, 200 * (rx - 1.0));
            }

            auto originalDrawColor = drawColour;

            if (hotSpotIndex == i - 1)
            {
                drawColour = drawColour.brighter(0.6);
            }

            if (i == scale.count)
            {
                g.setColour(drawColour);
                g.fillEllipse(x0, y0, dx, dy);

                if (hotSpotIndex == i - 1)
                {
                    auto p = juce::Path();
                    if (whichSideOfZero < 0)
                    {
                        p.addArc(x0, y0, dx, dy, juce::MathConstants<float>::pi,
                                 juce::MathConstants<float>::twoPi);
                    }
                    else
                    {
                        p.addArc(x0, y0, dx, dy, 0, juce::MathConstants<float>::pi);
                    }
                    g.setColour(juce::Colour(200, 200, 100));
                    g.fillPath(p);
                }
            }
            else
            {
                g.setColour(drawColour);

                g.fillEllipse(x0, y0, dx, dy);

                if (hotSpotIndex != i - 1)
                {
                    g.setColour(drawColour.brighter(0.6));
                    g.drawEllipse(x0, y0, dx, dy, 0.01);
                }
            }

            if (notesOn[i % scale.count])
            {
                g.setColour(juce::Colour(255, 255, 255));
                g.drawEllipse(x0, y0, dx, dy, 0.02);
            }

            dx += x0;
            dy += y0;
            screenTransform.transformPoint(x0, y0);
            screenTransform.transformPoint(dx, dy);
            screenHotSpots.push_back(juce::Rectangle<float>(x0, dy, dx - x0, y0 - dy));
        }
    }
    g.restoreState();
}

struct IntervalMatrix : public juce::Component
{
    IntervalMatrix(TuningEditor *e) : teditor(e)
    {
        viewport = std::make_unique<juce::Viewport>();
        intervalPainter = std::make_unique<IntervalPainter>(this);
        viewport->setViewedComponent(intervalPainter.get(), false);

        whatLabel = std::make_unique<juce::Label>("Interval");
        addAndMakeVisible(*whatLabel);
        whatLabel->setFont(boldTunEdFont.withHeight(12.0f * scaleTunEd));

        explLabel = std::make_unique<juce::Label>("Interval");
        explLabel->setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(*explLabel);
        explLabel->setFont(plainTunEdFont.withHeight(10.0f * scaleTunEd));

        addAndMakeVisible(*viewport);

        setIntervalMode();
    };
    virtual ~IntervalMatrix() = default;

    void setRotationMode()
    {
        whatLabel->setText("Scale Rotation Intervals",
                           juce::NotificationType::dontSendNotification);
        explLabel->setText("If you shift the scale root to note N, show the interval to note M",
                           juce::NotificationType::dontSendNotification);
        intervalPainter->mode = IntervalMatrix::IntervalPainter::ROTATION;

        intervalPainter->setSizeFromTuning();
        repaint();
    }

    void setTrueKeyboardMode()
    {
        whatLabel->setText("True Keyboard Display", juce::NotificationType::dontSendNotification);
        explLabel->setText("Show intervals between any played keys in realtime",
                           juce::NotificationType::dontSendNotification);
        intervalPainter->mode = IntervalMatrix::IntervalPainter::TRUE_KEYS;

        intervalPainter->setSizeFromTuning();
        repaint();
    }

    void setIntervalMode()
    {
        whatLabel->setText("Interval Between Notes", juce::NotificationType::dontSendNotification);
        explLabel->setText(
            "Given any two notes in the loaded scale, show the interval in cents between them",
            juce::NotificationType::dontSendNotification);
        intervalPainter->mode = IntervalMatrix::IntervalPainter::INTERV;

        intervalPainter->setSizeFromTuning();
        repaint();
    }

    void setIntervalRelativeMode()
    {
        whatLabel->setText("Interval to Equal Division",
                           juce::NotificationType::dontSendNotification);
        explLabel->setText("Given any two notes in the loaded scale, show the distance to the "
                           "equal division interval",
                           juce::NotificationType::dontSendNotification);
        intervalPainter->mode = IntervalMatrix::IntervalPainter::DIST;

        intervalPainter->setSizeFromTuning();
        repaint();
    }

    std::vector<float> rcents;

    void setTuning(const Tunings::Tuning &t)
    {
        tuning = t;
        setNotesOn(bitset);

        float lastc = 0;
        rcents.clear();
        rcents.push_back(0);
        for (auto &t : t.scale.tones)
        {
            rcents.push_back(t.cents);
            lastc = t.cents;
        }
        for (auto &t : t.scale.tones)
        {
            rcents.push_back(t.cents + lastc);
        }
        intervalPainter->setSizeFromTuning();
        intervalPainter->repaint();
    }

    struct IntervalPainter : public juce::Component
    {
        enum Mode
        {
            INTERV,
            DIST,
            ROTATION,
            TRUE_KEYS
        } mode{INTERV};
        IntervalPainter(IntervalMatrix *m) : matrix(m) {}

        static constexpr int cellH = 14 * scaleTunEd;
        static constexpr int cellW = 31 * scaleTunEd;
        void setSizeFromTuning()
        {
            if (mode == TRUE_KEYS)
            {
                setSize(matrix->viewport->getBounds().getWidth() - 4,
                        matrix->viewport->getBounds().getHeight() - 4);
            }
            else
            {
                auto ic = matrix->tuning.scale.count + 2;
                auto nh = ic * cellH;
                auto nw = ic * cellW;

                setSize(nw, nh);
            }
        }

        void paintTrueKeys(juce::Graphics &g)
        {
            g.fillAll(intervalBackgroundTunEdColor);

            auto &bs = matrix->bitset;

            int numNotes{0};
            for (int i = 0; i < bs.size(); ++i)
                numNotes += bs[i];

            g.setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));

            if (numNotes == 0)
            {
                g.setColour(intervalHeatmapZeroTunEdColor);
                return;
            }

            int oct_offset = 1;

            auto bx = getLocalBounds().withTrimmedTop(15).reduced(2);
            auto xpos = bx.getX();
            auto ypos = bx.getY();

            auto colWidth = 39 * scaleTunEd;
            auto rowHeight = 19 * scaleTunEd;

            auto noteName = [oct_offset](int nn) {
                std::string s = get_notename(nn, oct_offset);
                s += " (" + std::to_string(nn) + ")";
                return s;
            };
            {
                int hpos = xpos + 2 * colWidth;
                for (int i = 0; i < bs.size(); ++i)
                {
                    if (bs[i])
                    {
                        g.setColour(intervalHeatmapZeroTunEdColor);
                        g.drawText(noteName(i), hpos, ypos, colWidth, rowHeight,
                                   juce::Justification::centred);
                        hpos += colWidth;
                    }
                }
            }

            ypos += rowHeight;
            
            // TODO Tidy, doesn't need to be a func
            auto noteToFreq = [this](auto note) -> float {
                    return matrix->tuning.frequencyForMidiNote(note);
            };
            
            // TODO Tidy, doesn't need to be a func
            auto noteToPitch = [this](auto note) -> float {
                    return matrix->tuning.logScaledFrequencyForMidiNote(note);
            };

            for (int i = 0; i < bs.size(); ++i)
            {
                if (bs[i])
                {
                    auto hpos = xpos;
                    g.setColour(intervalHeatmapZeroTunEdColor);
                    g.drawText(noteName(i), hpos, ypos, colWidth, rowHeight,
                               juce::Justification::centredLeft);
                    hpos += colWidth;
                    g.drawText(fmt::format("{:.2f}Hz", noteToFreq(i)), hpos, ypos, colWidth,
                               rowHeight, juce::Justification::centredLeft);
                    hpos += colWidth;

                    auto pitch0 = noteToPitch(i);

                    for (int j = 0; j < bs.size(); ++j)
                    {
                        if (bs[j])
                        {
                            auto bx = juce::Rectangle<int>(hpos, ypos, colWidth, rowHeight);
                            if (i != j)
                            {
                                g.setColour(intervalHeatmapZeroTunEdColor);
                                g.fillRect(bx);
                                g.setColour(juce::Colours::darkgrey);
                                g.drawRect(bx, 1);
                                g.setColour(intervalTextTunEdColor);
                                auto pitch = noteToPitch(j);
                                g.drawText(fmt::format("{:.2f}", 1200 * (pitch0 - pitch)), bx,
                                           juce::Justification::centred);
                            }
                            else
                            {
                                g.setColour(juce::Colours::darkgrey);
                                g.fillRect(bx);
                            }
                            hpos += colWidth;
                        }
                    }

                    ypos += rowHeight;
                }
            }
        }
        void paint(juce::Graphics &g) override
        {
            if (mode == TRUE_KEYS)
            {
                paintTrueKeys(g);
                return;
            }
            g.fillAll(intervalBackgroundTunEdColor);
            auto ic = matrix->tuning.scale.count;
            int mt = ic + 2;
            g.setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));
            for (int i = 0; i < mt; ++i)
            {
                bool noi = i > 0 ? matrix->notesOn[i - 1] : false;
                for (int j = 0; j < mt; ++j)
                {
                    bool noj = j > 0 ? matrix->notesOn[j - 1] : false;
                    bool isHovered = false;
                    if ((i == hoverI && j == hoverJ) || (i == 0 && j == hoverJ) ||
                        (i == hoverI && j == 0))
                    {
                        isHovered = true;
                    }
                    auto bx = juce::Rectangle<float>(i * cellW, j * cellH, cellW - 1, cellH - 1);
                    if ((i == 0 || j == 0) && (i + j))
                    {
                        auto no = false;
                        if (i > 0)
                            no = no || matrix->notesOn[i - 1];
                        if (j > 0)
                            no = no || matrix->notesOn[j - 1];

                        if (no)
                            g.setColour(intervalNoteLabelBackgroundPlayedTunEdColor);
                        else
                            g.setColour(intervalNoteLabelBackgroundTunEdColor);
                        g.fillRect(bx);

                        auto lb = std::to_string(i + j - 1);

                        if (isHovered && no)
                            g.setColour(intervalNoteLabelForegroundHoverPlayedTunEdColor);
                        if (isHovered)
                            g.setColour(intervalNoteLabelForegroundHoverTunEdColor);
                        else if (no)
                            g.setColour(intervalNoteLabelForegroundPlayedTunEdColor);
                        else
                            g.setColour(intervalNoteLabelForegroundTunEdColor);
                        g.drawText(lb, bx, juce::Justification::centred);
                    }
                    else if (i == j && mode != ROTATION)
                    {
                        g.setColour(juce::Colours::darkgrey);
                        g.fillRect(bx);
                        if (mode == ROTATION && i != 0)
                        {
                            g.setColour(intervalSkippedTunEdColor);
                            g.drawText("0", bx, juce::Justification::centred);
                        }
                    }
                    else if (i > j && mode != ROTATION)
                    {
                        auto centsi = 0.0;
                        auto centsj = 0.0;
                        if (i > 1)
                            centsi = matrix->tuning.scale.tones[i - 2].cents;
                        if (j > 1)
                            centsj = matrix->tuning.scale.tones[j - 2].cents;

                        auto cdiff = centsi - centsj;
                        auto disNote = i - j;
                        auto lastTone =
                            matrix->tuning.scale.tones[matrix->tuning.scale.count - 1].cents;
                        auto evenStep = lastTone / matrix->tuning.scale.count;
                        auto desCents = disNote * evenStep;

                        // ToDo: Skin these endpoints
                        if (fabs(cdiff - desCents) < 0.1)
                        {
                            g.setColour(intervalHeatmapZeroTunEdColor);
                        }
                        else if (cdiff < desCents)
                        {
                            // we are flat of even
                            auto dist = std::min((desCents - cdiff) / evenStep, 1.0);
                            auto c1 = intervalHeatmapNegFarTunEdColor;
                            auto c2 = intervalHeatmapNegNearTunEdColor;
                            g.setColour(c1.interpolatedWith(c2, 1.0 - dist));
                        }
                        else
                        {
                            auto dist = std::min(-(desCents - cdiff) / evenStep, 1.0);
                            auto b = 1.0 - dist;
                            auto c1 = intervalHeatmapPosFarTunEdColor;
                            auto c2 = intervalHeatmapPosNearTunEdColor;
                            g.setColour(c1.interpolatedWith(c2, b));
                        }

                        if (noi && noj)
                        {
                            g.setColour(intervalNoteLabelBackgroundPlayedTunEdColor);
                        }
                        g.fillRect(bx);

                        auto displayCents = cdiff;
                        if (mode == DIST)
                            displayCents = cdiff - desCents;
                        auto lb = fmt::format("{:.1f}", displayCents);
                        if (isHovered)
                            g.setColour(intervalTextHoverTunEdColor);
                        else
                            g.setColour(intervalTextTunEdColor);
                        g.drawText(lb, bx, juce::Justification::centred);
                    }
                    else if (mode == ROTATION && i > 0 && j > 0)
                    {
                        auto centsi = matrix->rcents[j - 1];
                        auto centsj = matrix->rcents[i + j - 2];
                        auto cdiff = centsj - centsi;

                        auto disNote = i - 1;
                        auto lastTone =
                            matrix->tuning.scale.tones[matrix->tuning.scale.count - 1].cents;
                        auto evenStep = lastTone / matrix->tuning.scale.count;
                        auto desCents = disNote * evenStep;

                        juce::Colour bg;
                        if (fabs(cdiff - desCents) < 0.1)
                        {
                            bg = (intervalHeatmapZeroTunEdColor);
                        }
                        else if (cdiff < desCents)
                        {
                            // we are flat of even
                            auto dist = std::min((desCents - cdiff) / evenStep, 1.0);
                            auto r = (1.0 - dist);
                            auto c1 = intervalHeatmapNegFarTunEdColor;
                            auto c2 = intervalHeatmapNegNearTunEdColor;
                            bg = (c1.interpolatedWith(c2, r));
                        }
                        else
                        {
                            auto dist = std::min(-(desCents - cdiff) / evenStep, 1.0);
                            auto b = 1.0 - dist;
                            auto c1 = intervalHeatmapPosFarTunEdColor;
                            auto c2 = intervalHeatmapPosNearTunEdColor;
                            bg = (c1.interpolatedWith(c2, b));
                        }
                        if (i == mt - 1 || i == 1)
                            bg = intervalHeatmapZeroAlpha9TunEdColor;

                        g.setColour(bg);
                        g.fillRect(bx);

                        juce::Colour fg;
                        if (isHovered)
                            fg = intervalTextHoverTunEdColor;
                        else
                            fg = intervalTextTunEdColor;
                        if (i == mt - 1 || i == 1)
                            fg = fg.withAlpha(0.7f);

                        g.setColour(fg);
                        g.drawText(fmt::format("{:.1f}", cdiff), bx, juce::Justification::centred);
                    }
                }
            }
        }

        int hoverI{-1}, hoverJ{-1};

        juce::Point<float> lastMousePos;
        void mouseDown(const juce::MouseEvent &e) override
        {
            if (mode == TRUE_KEYS)
            {
                return;
            }
            
            juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(
                true);
            lastMousePos = e.position;
        }
        void mouseUp(const juce::MouseEvent &e) override
        {
            if (mode == TRUE_KEYS)
            {
                return;
            }
            juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(
                false);
            auto p = localPointToGlobal(e.mouseDownPosition);
            juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(p);
        }
        void mouseDrag(const juce::MouseEvent &e) override
        {
            if (mode == TRUE_KEYS)
            {
                return;
            }

            auto dPos = e.position.getY() - lastMousePos.getY();
            dPos = -dPos;
            auto speed = 0.5;
            if (e.mods.isShiftDown())
                speed = 0.05;
            dPos = dPos * speed;
            lastMousePos = e.position;

            if (mode == ROTATION)
            {
                if (hoverI > 1 && hoverI <= matrix->tuning.scale.count)
                {
                    int tonI = (hoverI - 1 + hoverJ - 2);

                    tonI = tonI % matrix->tuning.scale.count;
                    matrix->teditor->onToneChanged(tonI,
                                                   matrix->tuning.scale.tones[tonI].cents + dPos);
                }
            }
            else
            {
                auto i = hoverI;
                if (i > 1)
                {
                    auto centsi = matrix->tuning.scale.tones[i - 2].cents + dPos;
                    matrix->teditor->onToneChanged(i - 2, centsi);
                }
            }
        }

        void mouseDoubleClick(const juce::MouseEvent &e) override
        {
            if (mode == ROTATION)
            {
                if (hoverI > 0 && hoverJ > 0)
                {
                    int tonI = (hoverI - 1 + hoverJ - 1) % matrix->tuning.scale.count;
                    matrix->teditor->onToneChanged(
                        tonI, (hoverI + hoverJ - 1) *
                                  matrix->tuning.scale.tones[matrix->tuning.scale.count - 1].cents /
                                  matrix->tuning.scale.count);
                }
            }
            else
            {
                if (hoverI > 1 && hoverI > hoverJ)
                {
                    int tonI = (hoverI - 2) % matrix->tuning.scale.count;
                    matrix->teditor->onToneChanged(
                        tonI, (hoverI - 1) *
                                  matrix->tuning.scale.tones[matrix->tuning.scale.count - 1].cents /
                                  matrix->tuning.scale.count);
                }
            }
        }
        void mouseEnter(const juce::MouseEvent &e) override { repaint(); }

        void mouseExit(const juce::MouseEvent &e) override
        {
            hoverI = -1;
            hoverJ = -1;
            repaint();

            setMouseCursor(juce::MouseCursor::NormalCursor);
        }

        void mouseMove(const juce::MouseEvent &e) override
        {
            if (setupHoverFrom(e.position))
                repaint();
            if (hoverI >= 1 && hoverI <= matrix->tuning.scale.count && hoverJ >= 1 &&
                hoverJ <= matrix->tuning.scale.count && hoverI > hoverJ)
            {
                setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
            }
            else
            {
                setMouseCursor(juce::MouseCursor::NormalCursor);
            }
        }

        bool setupHoverFrom(const juce::Point<float> &here)
        {
            int ohi = hoverI, ohj = hoverJ;

            //  auto bx = juce::Rectangle<int>(i * cellW, j * cellH, cellW - 1, cellH - 1);
            // box x is i*cellW to i*cellW + cellW
            // box x / cellW is i to i + 1
            // floor(box x / cellW ) is i
            hoverI = floor(here.x / cellW);
            hoverJ = floor(here.y / cellH);
            if (ohi != hoverI || ohj != hoverJ)
            {
                return true;
            }
            return false;
        }

        IntervalMatrix *matrix;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntervalPainter);
    };

    void resized() override
    {
        auto br = getLocalBounds().reduced(2);
        auto tr = br.withHeight(25);
        auto vr = br.withTrimmedTop(25);
        whatLabel->setBounds(tr);
        explLabel->setBounds(tr);
        viewport->setBounds(vr);
        intervalPainter->setSizeFromTuning();
    }

    std::vector<bool> notesOn;
    std::bitset<128> bitset{0};
    void setNotesOn(const std::bitset<128> &bs)
    {
        bitset = bs;
        notesOn.resize(tuning.scale.count + 1);
        for (int i = 0; i < tuning.scale.count; ++i)
            notesOn[i] = false;
        notesOn[tuning.scale.count] = notesOn[0];

        for (int i = 0; i < 128; ++i)
        {
            if (bitset[i])
            {
                notesOn[tuning.scalePositionForMidiNote(i)] = true;
            }
        }
        intervalPainter->repaint();
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(intervalBackgroundTunEdColor);
    }

    std::unique_ptr<IntervalPainter> intervalPainter;
    std::unique_ptr<juce::Viewport> viewport;
    std::unique_ptr<juce::Label> whatLabel;
    std::unique_ptr<juce::Label> explLabel;

    Tunings::Tuning tuning;
    TuningEditor *teditor{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntervalMatrix);
};

void RadialScaleGraph::mouseMove(const juce::MouseEvent &e)
{
    int owso = whichSideOfZero;
    int ohsi = hotSpotIndex;
    hotSpotIndex = -1;
    int h = 0;
    for (auto r : screenHotSpots)
    {
        if (r.contains(e.getPosition().toFloat()))
        {
            hotSpotIndex = h;
            if (h == scale.count - 1)
            {
                if (e.position.x < r.getCentreX())
                {
                    whichSideOfZero = -1;
                }
                else
                {
                    whichSideOfZero = 1;
                }
            }
        }
        h++;
    }

    if (hotSpotIndex >= 0)
        wheelHotSpotIndex = hotSpotIndex;

    if (ohsi != hotSpotIndex || owso != whichSideOfZero)
        repaint();
}
void RadialScaleGraph::mouseDown(const juce::MouseEvent &e)
{
    if (hotSpotIndex == -1)
    {
        centsAtMouseDown = 0;
        angleAtMouseDown = 0;
    }
    else
    {
        centsAtMouseDown = scale.tones[hotSpotIndex].cents;
        angleAtMouseDown = toneKnobs[hotSpotIndex + 1]->angle;
        dIntervalAtMouseDown = dInterval;
    }
    lastDragPoint = e.position;
}

void RadialScaleGraph::mouseDrag(const juce::MouseEvent &e)
{
    if (hotSpotIndex != -1)
    {
        float dr = 0;
        auto mdp = e.getMouseDownPosition().toFloat();
        auto xd = mdp.getX();
        auto yd = mdp.getY();
        screenTransformInverted.transformPoint(xd, yd);

        auto mp = e.getPosition().toFloat();
        auto x = mp.getX();
        auto y = mp.getY();
        screenTransformInverted.transformPoint(x, y);

        auto lx = lastDragPoint.getX();
        auto ly = lastDragPoint.getY();
        screenTransformInverted.transformPoint(lx, ly);

        if (displayMode == DisplayMode::RADIAL)
        {
            dr = -sqrt(xd * xd + yd * yd) + sqrt(x * x + y * y);

            auto speed = 0.7;
            if (e.mods.isShiftDown())
                speed = speed * 0.1;
            dr = dr * speed;

            toneKnobs[hotSpotIndex + 1]->angle = angleAtMouseDown + 100 * dr / dIntervalAtMouseDown;
            toneKnobs[hotSpotIndex + 1]->repaint();
            selfEditGuard++;
            if (hotSpotIndex == scale.count - 1 && whichSideOfZero > 0)
            {
                auto ct = e.mods.isShiftDown() ? 0.05 : 1;
                onScaleRescaled(dr > 0 ? ct : -ct);
            }
            else
            {
                onToneChanged(hotSpotIndex, centsAtMouseDown + 100 * dr / dIntervalAtMouseDown);
            }
            selfEditGuard--;
        }
        else
        {
            auto xy2ph = [](auto x, auto y) {
                float res{0};
                if (fabs(x) < 0.001)
                {
                    res = (y > 0) ? 1 : -1;
                }
                else
                {
                    res = atan(y / x) / M_PI;
                    if (x > 0)
                        res = 0.5 - res;
                    else
                        res = 1.5 - res;
                }
                return res / 2.0;
            };
            auto phsDn = xy2ph(xd, yd);
            auto phsMs = xy2ph(x, y);

            toneKnobs[hotSpotIndex + 1]->angle = angleAtMouseDown + 100 * dr / dIntervalAtMouseDown;
            toneKnobs[hotSpotIndex + 1]->repaint();
            selfEditGuard++;
            if (hotSpotIndex == scale.count - 1 && whichSideOfZero > 0)
            {
                auto ct = e.mods.isShiftDown() ? 0.05 : 1;
                onScaleRescaled((ly - y) > 0 ? ct : -ct);
            }
            else if (hotSpotIndex == scale.count - 1)
            {
                auto diff = (y - ly);
                auto ct = e.mods.isShiftDown() ? 0.01 : 0.2;
                onToneChanged(hotSpotIndex, (1.0 + ct * diff) * scale.tones.back().cents);
            }
            else
            {
                onToneChanged(hotSpotIndex, phsMs * scale.tones.back().cents);
            }
            selfEditGuard--;
        }
        lastDragPoint = e.position;
    }
}

void RadialScaleGraph::mouseDoubleClick(const juce::MouseEvent &e)
{
    if (hotSpotIndex != -1)
    {
        auto newCents = (hotSpotIndex + 1) * scale.tones[scale.count - 1].cents / scale.count;
        toneKnobs[hotSpotIndex + 1]->angle = 0;
        toneKnobs[hotSpotIndex + 1]->repaint();
        selfEditGuard++;
        onToneChanged(hotSpotIndex, newCents);
        selfEditGuard--;
    }
}

void RadialScaleGraph::mouseWheelMove(const juce::MouseEvent &event,
                                      const juce::MouseWheelDetails &wheel)
{
    if (wheelHotSpotIndex != -1)
    {
        float delta = wheel.deltaX - (wheel.isReversed ? 1 : -1) * wheel.deltaY;
        if (delta == 0)
            return;

#if MAC
        float speed = 1.2;
#else
        float speed = 0.42666;
#endif
        if (event.mods.isShiftDown())
        {
            speed = speed / 10;
        }

        // This is calibrated to give us reasonable speed on a 0-1 basis from the slider
        // but in this widget '1' is the small motion so speed it up some
        auto dr = speed * delta;

        toneKnobs[wheelHotSpotIndex + 1]->angle =
            toneKnobs[wheelHotSpotIndex + 1]->angle + 100 * dr / dInterval;
        toneKnobs[wheelHotSpotIndex + 1]->repaint();
        auto newCents = scale.tones[wheelHotSpotIndex].cents + 100 * dr / dInterval;

        selfEditGuard++;
        onToneChanged(wheelHotSpotIndex, newCents);
        selfEditGuard--;
    }
}
void RadialScaleGraph::textEditorReturnKeyPressed(juce::TextEditor &editor)
{
    for (int i = 0; i <= scale.count - 1; ++i)
    {
        if (&editor == toneEditors[i].get())
        {
            selfEditGuard++;
            onToneStringChanged(i, editor.getText().toStdString());
            selfEditGuard--;
        }
    }
}

void RadialScaleGraph::textEditorFocusLost(juce::TextEditor &editor)
{
    editor.setHighlightedRegion(juce::Range(-1, -1));
}


struct SCLKBMDisplay : public juce::Component,
                       juce::TextEditor::Listener,
                       juce::CodeDocument::Listener
{
    SCLKBMDisplay(TuningEditor *e) : teditor(e)
    {
        sclDocument = std::make_unique<juce::CodeDocument>();
        sclDocument->addListener(this);
        sclTokeniser = std::make_unique<SCLKBMTokeniser>();
        scl = std::make_unique<juce::CodeEditorComponent>(*sclDocument, sclTokeniser.get());
        scl->setLineNumbersShown(false);
        scl->setScrollbarThickness(8);
        addAndMakeVisible(*scl);

        kbmDocument = std::make_unique<juce::CodeDocument>();
        kbmDocument->addListener(this);
        kbmTokeniser = std::make_unique<SCLKBMTokeniser>(false);

        kbm = std::make_unique<juce::CodeEditorComponent>(*kbmDocument, kbmTokeniser.get());
        kbm->setLineNumbersShown(false);
        kbm->setScrollbarThickness(8);
        addAndMakeVisible(*kbm);

        auto teProps = [this](const auto &te) {
            te->setJustification((juce::Justification::verticallyCentred));
            // te->setIndents(4, (te->getHeight() - te->getTextHeight()) / 2);
        };

        auto newL = [this](const std::string &s) {
            auto res = std::make_unique<juce::Label>(s, s);
            res->setText(s, juce::dontSendNotification);
            addAndMakeVisible(*res);
            return res;
        };
        evenDivOfL = newL("Divide");
        evenDivOf = std::make_unique<juce::TextEditor>();
        teProps(evenDivOf);
        evenDivOf->setText("2", juce::dontSendNotification);
        addAndMakeVisible(*evenDivOf);
        evenDivOf->addListener(this);
        evenDivOf->setSelectAllWhenFocused(true);

        evenDivIntoL = newL("into");
        evenDivInto = std::make_unique<juce::TextEditor>();
        teProps(evenDivInto);
        evenDivInto->setText("12", juce::dontSendNotification);
        addAndMakeVisible(*evenDivInto);
        evenDivInto->addListener(this);
        evenDivInto->setSelectAllWhenFocused(true);

        evenDivStepsL = newL("steps");
        
        edoGo = std::make_unique<juce::TextButton>();
        edoGo -> setButtonText("Generate");
        edoGo->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
        edoGo->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
        edoGo->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
        edoGo->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
        edoGo->onClick = [this]() {
            auto txt = evenDivOf->getText();

            try
            {
                if (txt.contains(".") || txt.contains(","))
                {
                    auto spanct = std::atof(evenDivOf->getText().toRawUTF8());
                    auto num = std::atoi(evenDivInto->getText().toRawUTF8());

                    this->teditor->onNewSCLKBM(Tunings::evenDivisionOfCentsByM(spanct, num).rawText,
                                               kbmDocument->getAllContent().toStdString());
                }
                else if (txt.contains("/"))
                {
                    auto tt = Tunings::toneFromString(txt.toStdString());
                    auto spanct = tt.cents;
                    auto num = std::atoi(evenDivInto->getText().toRawUTF8());

                    this->teditor->onNewSCLKBM(
                        Tunings::evenDivisionOfCentsByM(spanct, num, txt.toStdString()).rawText,
                        kbmDocument->getAllContent().toStdString());
                }
                else
                {
                    auto span = std::atoi(evenDivOf->getText().toRawUTF8());
                    auto num = std::atoi(evenDivInto->getText().toRawUTF8());

                    this->teditor->onNewSCLKBM(Tunings::evenDivisionOfSpanByM(span, num).rawText,
                                               kbmDocument->getAllContent().toStdString());
                }
            }
            catch (const Tunings::TuningError &e)
            {
                //std::cout << "Tuning Error" << e.what() << std::endl;
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Error", e.what());
            }
        };
        addAndMakeVisible(*edoGo);

        kbmStartL = newL("Root:");
        kbmStart = std::make_unique<juce::TextEditor>();
        teProps(kbmStart);
        kbmStart->setText("60", juce::dontSendNotification);
        addAndMakeVisible(*kbmStart);
        kbmStart->addListener(this);
        kbmStart->setSelectAllWhenFocused(true);

        kbmConstantL = newL("Constant:");
        kbmConstant = std::make_unique<juce::TextEditor>();
        teProps(kbmConstant);
        kbmConstant->setText("69", juce::dontSendNotification);
        addAndMakeVisible(*kbmConstant);
        kbmConstant->addListener(this);
        kbmConstant->setSelectAllWhenFocused(true);

        kbmFreqL = newL("Freq:");
        kbmFreq = std::make_unique<juce::TextEditor>();
        teProps(kbmFreq);
        kbmFreq->setText("440", juce::dontSendNotification);
        addAndMakeVisible(*kbmFreq);
        kbmFreq->addListener(this);
        kbmFreq->setSelectAllWhenFocused(true);
        
        kbmGo = std::make_unique<juce::TextButton>();
        kbmGo -> setButtonText("Generate");
        kbmGo->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
        kbmGo->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
        kbmGo->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
        kbmGo->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
        kbmGo->onClick = [this]() {
            auto start = std::atoi(kbmStart->getText().toRawUTF8());
            auto constant = std::atoi(kbmConstant->getText().toRawUTF8());
            auto freq = std::atof(kbmFreq->getText().toRawUTF8());

            try
            {
                this->teditor->onNewSCLKBM(
                    sclDocument->getAllContent().toStdString(),
                    Tunings::startScaleOnAndTuneNoteTo(start, constant, freq).rawText);
            }
            catch (const Tunings::TuningError &e)
            {
                //std::cout << "Tuning Error" << e.what() << std::endl;
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Error", e.what());
            }
        };
        addAndMakeVisible(*kbmGo);
        
        // From onSkinChanged
        scl->setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId,
                       sclkbmEditorBackgroundTunEdColor);
        kbm->setColour(juce::CodeEditorComponent::ColourIds::backgroundColourId,
                       sclkbmEditorBackgroundTunEdColor);
        
        for (auto t : {scl.get(), kbm.get()})
        {
            auto cs = t->getColourScheme();

            cs.set("Comment", sclkbmEditorCommentTunEdColor);
            cs.set("Text", sclkbmEditorTextTunEdColor);
            cs.set("Cents", sclkbmEditorCentsTunEdColor);
            cs.set("Ratio", sclkbmEditorRatioTunEdColor);
            cs.set("Played", sclkbmEditorPlayedTunEdColor);

            t->setColourScheme(cs);
        }

        for (const auto &r : {evenDivIntoL.get(), evenDivOfL.get(), evenDivStepsL.get(),
                              kbmStartL.get(), kbmConstantL.get(), kbmFreqL.get()})
        {
            r->setColour(juce::Label::textColourId, radialTonelabelTextTunEdColor);
            r->setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));
        }

        for (const auto &r :
             {evenDivInto.get(), evenDivOf.get(), kbmStart.get(), kbmConstant.get(), kbmFreq.get()})
        {
            r->setFont(plainTunEdFont.withHeight(9.0f * scaleTunEd));

            r->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                         radialTonelabelBackgroundTunEdColor);
            r->setColour(juce::TextEditor::ColourIds::outlineColourId,
                         radialTonelabelBorderTunEdColor);
            r->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                         radialTonelabelBorderTunEdColor);
            r->setColour(juce::TextEditor::ColourIds::textColourId,
                         radialTonelabelTextTunEdColor);
            r->applyColourToAllText(radialTonelabelTextTunEdColor, true);

            // Work around a buglet that the text editor applies fonts only to newly inserted
            // text after setFont
            auto t = r->getText();
            r->setText("--", juce::dontSendNotification);
            r->setText(t, juce::dontSendNotification);
        }

        scl->setFont(plainMonoTunEdFont.withHeight(10.0f * scaleTunEd));
        kbm->setFont(plainMonoTunEdFont.withHeight(10.0f * scaleTunEd));

    }

    struct SCLKBMTokeniser : public juce::CodeTokeniser
    {
        enum
        {
            token_Error,
            token_Comment,
            token_Text,
            token_Cents,
            token_Ratio,
            token_Playing
        };

        bool isSCL{false};
        SCLKBMTokeniser(bool s = true) : isSCL(s) {}

        int readNextToken(juce::CodeDocument::Iterator &source) override
        {
            auto firstChar = source.peekNextChar();
            if (firstChar == '!')
            {
                source.skipToEndOfLine();
                return token_Comment;
            }
            if (!isSCL)
            {
                source.skipToEndOfLine();
                return token_Text;
            }
            source.skipWhitespace();
            auto nc = source.nextChar();
            while (nc >= '0' && nc <= '9' && nc)
            {
                nc = source.nextChar();
            }
            source.previousChar(); // in case we are just numbers
            source.skipToEndOfLine();

            auto ln = source.getLine();
            int toneIdx = -1;
            if (toneToLine.find(ln) != toneToLine.end())
            {
                toneIdx = toneToLine[ln];
            }

            if (toneIdx >= 0 && toneIdx < scale.count)
            {
                if (notesOn.size() == scale.count)
                {
                    auto ni = (toneIdx + scale.count + 1) % scale.count;
                    if (notesOn[ni])
                        return token_Playing;
                }
                const auto &tone = scale.tones[toneIdx];
                if (tone.type == Tunings::Tone::kToneCents)
                    return token_Cents;
                if (tone.type == Tunings::Tone::kToneRatio)
                    return token_Ratio;
            }
            return token_Text;
        }

        std::vector<bool> notesOn;
        Tunings::Scale scale;
        std::unordered_map<int, int> toneToLine;
        void setScale(const Tunings::Scale &s)
        {
            scale = s;
            toneToLine.clear();
            int idx = 0;
            for (const auto &t : scale.tones)
            {
                toneToLine[t.lineno] = idx++;
            }
        }

        void setNotesOn(const std::vector<bool> &no) { notesOn = no; }
        juce::CodeEditorComponent::ColourScheme getDefaultColourScheme() override
        {
            struct Type
            {
                const char *name;
                uint32_t colour;
            };

            // clang-format off
            const Type types[] = {
                {"Error", 0xffcc0000},
                {"Comment", 0xffff0000},
                {"Text", 0xFFFF0000},
                {"Cents", 0xFFFF0000},
                {"Ratio", 0xFFFF0000},
                {"Played", 0xFFFF0000 }
            };
            // clang-format on

            juce::CodeEditorComponent::ColourScheme cs;

            for (auto &t : types)
                cs.set(t.name, juce::Colour(t.colour));

            return cs;
        }
    };

    std::vector<bool> notesOn;
    std::bitset<128> bitset{0};
    void setNotesOn(const std::bitset<128> &bs)
    {
        bitset = bs;
        notesOn.resize(tuning.scale.count);
        for (int i = 0; i < tuning.scale.count; ++i)
            notesOn[i] = false;

        for (int i = 0; i < 128; ++i)
        {
            if (bitset[i])
            {
                notesOn[tuning.scalePositionForMidiNote(i)] = true;
            }
        }

        if (sclTokeniser)
            sclTokeniser->setNotesOn(notesOn);
        if (scl)
            scl->retokenise(0, -1);
        repaint();
    }

    std::unique_ptr<juce::CodeDocument> sclDocument, kbmDocument;
    std::unique_ptr<SCLKBMTokeniser> sclTokeniser, kbmTokeniser;

    Tunings::Tuning tuning;

    void setTuning(const Tunings::Tuning &t)
    {
        tuning = t;
        sclTokeniser->setScale(t.scale);
        sclDocument->replaceAllContent(t.scale.rawText);
        kbmDocument->replaceAllContent(t.keyboardMapping.rawText);
        setApplyEnabled(false);
    }

    void resized() override
    {
        auto w = getWidth();
        auto h = getHeight();
        auto b = juce::Rectangle<int>(0, 0, w / 2, h).reduced(3, 3).withTrimmedBottom(30);

        scl->setBounds(b);
        kbm->setBounds(b.translated(w / 2, 0));

        auto r = juce::Rectangle<int>(0, h - 29, w, 25);
        auto s = r.withWidth(w / 2).withTrimmedLeft(2);

        auto nxt = [&s](int p) {
            auto q = s.withWidth(p * scaleTunEd * 0.73);
            s = s.withTrimmedLeft(p * scaleTunEd * 0.73);
            return q.reduced(0, 2);
        };

        evenDivOfL->setBounds(nxt(37));
        evenDivOf->setBounds(nxt(80));
        evenDivIntoL->setBounds(nxt(30));
        evenDivInto->setBounds(nxt(40));
        evenDivStepsL->setBounds(nxt(30));
        edoGo->setBounds(nxt(60));

        s = r.withTrimmedLeft(w / 2 + 2);
        kbmStartL->setBounds(nxt(30));
        kbmStart->setBounds(nxt(50));
        kbmConstantL->setBounds(nxt(50));
        kbmConstant->setBounds(nxt(50));
        kbmFreqL->setBounds(nxt(30));
        kbmFreq->setBounds(nxt(50));
        s = s.translated(3, 0);
        kbmGo->setBounds(nxt(60));

        auto teProps = [this](const auto &te) {
            te->setFont(plainMonoTunEdFont.withHeight(9.0f * scaleTunEd));
            te->setJustification((juce::Justification::verticallyCentred));
            te->setIndents(4, (te->getHeight() - te->getTextHeight()) / 2);
        };

        teProps(evenDivOf);
        teProps(evenDivInto);
        teProps(kbmStart);
        teProps(kbmConstant);
        teProps(kbmFreq);
    }

    void codeDocumentTextInserted(const juce::String &newText, int insertIndex) override
    {
        setApplyEnabled(true);
    }
    void codeDocumentTextDeleted(int startIndex, int endIndex) override { setApplyEnabled(true); }

    void paint(juce::Graphics &g) override
    {
        g.fillAll(sclkbmBackgroundTunEdColor);
        g.setColour(sclkbmEditorBorderTunEdColor);
        g.drawRect(scl->getBounds().expanded(1), 2);
        g.drawRect(kbm->getBounds().expanded(1), 2);
    }

    void textEditorTextChanged(juce::TextEditor &editor) override { setApplyEnabled(true); }
    void textEditorFocusLost(juce::TextEditor &editor) override
    {
        editor.setHighlightedRegion(juce::Range(-1, -1));
    }
    
    void setApplyEnabled(bool b)
    {
        teditor->setApplyEnabled(b);
    }

    std::function<void(const std::string &scl, const std::string &kbl)> onTextChanged =
        [](auto a, auto b) {};

    std::unique_ptr<juce::CodeEditorComponent> scl;
    std::unique_ptr<juce::CodeEditorComponent> kbm;
    TuningEditor *teditor{nullptr};

    std::unique_ptr<juce::Label> evenDivOfL, evenDivIntoL, evenDivStepsL;
    std::unique_ptr<juce::TextEditor> evenDivOf, evenDivInto;
    std::unique_ptr<juce::TextButton> edoGo;

    std::unique_ptr<juce::Label> kbmStartL, kbmConstantL, kbmFreqL;
    std::unique_ptr<juce::TextEditor> kbmStart, kbmConstant, kbmFreq;
    std::unique_ptr<juce::TextButton> kbmGo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SCLKBMDisplay);
};

struct TuningControlArea : public juce::Component,
                           juce::Button::Listener
{
    TuningEditor *teditor{nullptr};
    TuningControlArea(TuningEditor *te) : teditor(te)
    {
        setAccessible(true);
        setTitle("Controls");
        setDescription("Controls");
        setFocusContainerType(juce::Component::FocusContainerType::keyboardFocusContainer);
    }

    void resized() override
    {
        rebuild();
    }

    void rebuild()
    {
        int labelHeight = 8 * scaleTunEd;
        int numfieldHeight = 12;
        int margin = 5;
        int xpos = 10;
        int actionBtnWidth = 60 * scaleTunEd;
        int actionButtonHeight = 14 * scaleTunEd;
        
        

        removeAllChildren();
        auto h = getHeight();

        {
            int marginPos = xpos + margin;
            
            int ypos = 1 + labelHeight + margin;
            int selectButtonWidth = 48 * scaleTunEd;
            int selectButtonHeight = 14 * scaleTunEd;
            
            selectL = newL("Edit Mode");
            selectL->setBounds(marginPos, 3, 200 * scaleTunEd, labelHeight);
            addAndMakeVisible(*selectL);
            
            auto btnrect = juce::Rectangle<int>(marginPos, ypos - 1, actionBtnWidth, actionButtonHeight);
            
            xpos += margin;
            
            selectScalaButton = std::make_unique<juce::TextButton>();
            selectScalaButton->setButtonText("Scala");
            selectScalaButton->addListener(this);
            selectScalaButton->setRadioGroupId(EditModeButtonGroup);
            selectScalaButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectScalaButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectScalaButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectScalaButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectScalaButton->setClickingTogglesState (true);
            selectScalaButton->setToggleState(true, juce::dontSendNotification);
            selectScalaButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            addAndMakeVisible(*selectScalaButton);
            
            xpos += selectButtonWidth + 2;
            
            selectPolarButton = std::make_unique<juce::TextButton>();
            selectPolarButton->setButtonText("Polar");
            selectPolarButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            selectPolarButton->addListener(this);
            selectPolarButton->setRadioGroupId(EditModeButtonGroup);
            selectPolarButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectPolarButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectPolarButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectPolarButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectPolarButton->setClickingTogglesState (true);
            addAndMakeVisible(*selectPolarButton);
            
            xpos += selectButtonWidth + 2;
            
            selectIntervalButton= std::make_unique<juce::TextButton>();
            selectIntervalButton->setButtonText("Interval");
            selectIntervalButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            selectIntervalButton->addListener(this);
            selectIntervalButton->setRadioGroupId(EditModeButtonGroup);
            selectIntervalButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectIntervalButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectIntervalButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectIntervalButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectIntervalButton->setClickingTogglesState (true);
            addAndMakeVisible(*selectIntervalButton);
            
            xpos += selectButtonWidth + 2;
            
            selectToEqualButton = std::make_unique<juce::TextButton>();
            selectToEqualButton->setButtonText("To Equal");
            selectToEqualButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            selectToEqualButton->setRadioGroupId(EditModeButtonGroup);
            selectToEqualButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectToEqualButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectToEqualButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectToEqualButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectToEqualButton->setClickingTogglesState (true);
            selectToEqualButton->addListener(this);
            addAndMakeVisible(*selectToEqualButton);
            
            xpos += selectButtonWidth + 2;
            
            selectRotationButton = std::make_unique<juce::TextButton>();
            selectRotationButton->setButtonText("Rotation");
            selectRotationButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            selectRotationButton->setRadioGroupId(EditModeButtonGroup);
            selectRotationButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectRotationButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectRotationButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectRotationButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectRotationButton->setClickingTogglesState (true);
            selectRotationButton->addListener(this);
            addAndMakeVisible(*selectRotationButton);
            
            xpos += selectButtonWidth + 2;
            
            selectTrueKeysButton = std::make_unique<juce::TextButton>();
            selectTrueKeysButton->setButtonText("True Keys");
            selectTrueKeysButton->setBounds(xpos, ypos, selectButtonWidth, selectButtonHeight);
            selectTrueKeysButton->setRadioGroupId(EditModeButtonGroup);
            selectTrueKeysButton->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            selectTrueKeysButton->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            selectTrueKeysButton->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            selectTrueKeysButton->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            selectTrueKeysButton->setClickingTogglesState (true);
            selectTrueKeysButton->addListener(this);
            addAndMakeVisible(*selectTrueKeysButton);
             
            xpos += selectButtonWidth / 2;
        }

        {
            int btnWidth = 65 * scaleTunEd;
            xpos += actionBtnWidth;
            int ypos = 1 + labelHeight + margin;
            
            actionL = newL("Actions");
            actionL->setBounds(xpos, 3, 200 * scaleTunEd, labelHeight);
            addAndMakeVisible(*actionL);
            
            savesclS = std::make_unique<juce::TextButton>();
            savesclS->setButtonText("Save Scale");
            savesclS->addListener(this);
            savesclS->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            savesclS->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            savesclS->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            savesclS->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            savesclS->setBounds(xpos, ypos, actionBtnWidth, actionButtonHeight);
            addAndMakeVisible(*savesclS);

            xpos += actionBtnWidth + 5;
            applyS = std::make_unique<juce::TextButton>();
            applyS->setButtonText("Apply");
            applyS->setEnabled(false);
            applyS->addListener(this);
            applyS->setColour (juce::TextButton::textColourOffId, btnTextOffTunEdCol);
            applyS->setColour (juce::TextButton::textColourOnId, btnTextOnTunEdCol);
            applyS->setColour (juce::TextButton::buttonOnColourId, btnOnTunEdCol);
            applyS->setColour (juce::TextButton::buttonColourId, btnOffTunEdCol);
            applyS->setBounds(xpos, ypos, actionBtnWidth, actionButtonHeight);
            addAndMakeVisible(*applyS);
        }
    }

    std::unique_ptr<juce::Label> newL(const std::string &s)
    {
        auto res = std::make_unique<juce::Label>(s, s);
        res->setText(s, juce::dontSendNotification);
        res->setFont(boldTunEdFont.withHeight(10.0f * scaleTunEd));
        res->setColour(juce::Label::textColourId, msegEditorTextTunEdColor);
        return res;
    }

    void buttonClicked(Button* buttonThatWasClicked) override
    {
        if (buttonThatWasClicked == selectScalaButton.get())
        {
            teditor->showEditor(0);
        }
        else if (buttonThatWasClicked == selectPolarButton.get())
        {
            teditor->showEditor(1);
        }
        else if (buttonThatWasClicked == selectIntervalButton.get())
        {
            teditor->showEditor(2);
        }
        else  if (buttonThatWasClicked == selectToEqualButton.get())
        {
            teditor->showEditor(3);
        }
        else if (buttonThatWasClicked == selectRotationButton.get())
        {
            teditor->showEditor(4);
        }
        else if (buttonThatWasClicked == selectTrueKeysButton.get())
        {
            teditor->showEditor(5);
        }
        else if (buttonThatWasClicked == applyS.get())
        {
            if (applyS->isEnabled())
            {
                auto *sck = teditor->sclKbmDisplay.get();
                sck->onTextChanged(sck->sclDocument->getAllContent().toStdString(),
                                   sck->kbmDocument->getAllContent().toStdString());
                applyS->setEnabled(false);
                applyS->repaint();
            }
        }
        else if (buttonThatWasClicked == savesclS.get())
        {
            if (applyS->isEnabled())
            {
                //std::cout << "SCL Save Error" << "You have unapplied changes in your SCL/KBM. Please apply them before saving!" << std::endl;
                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "SCL Save Error", "You have unapplied changes in your SCL/KBM. Please apply them before saving!");
                return;
            }
            
            fileChooser = std::make_unique<juce::FileChooser>("Save SCL", juce::File(), "*.scl");
            fileChooser->launchAsync(
                juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles |
                    juce::FileBrowserComponent::warnAboutOverwriting,
                [this](const juce::FileChooser &c) {
                    auto result = c.getResults();

                    if (result.isEmpty() || result.size() > 1)
                    {
                        return;
                    }
                    
                    auto file = result[0];
                    
                    if (file != juce::File {})
                    {
                        juce::String fileNamePath = file.getFullPathName();
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
                                sclSaveFile.appendText(juce::String(teditor->tuning.scale.rawText), false, false, nullptr);
                            }
                        }
                    }
                });
        }
    }

    std::unique_ptr<juce::Label> selectL, intervalL, actionL;
    std::unique_ptr<juce::TextButton> selectScalaButton, selectPolarButton, selectIntervalButton, selectToEqualButton, selectRotationButton, selectTrueKeysButton, savesclS, applyS;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void paint(juce::Graphics &g) override { g.fillAll(msegEditorPanelTunEdColor); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TuningControlArea);
};




TuningEditor::TuningEditor(ScaleSpaceAudioProcessor *p, const uint32_t tuningIndex)
                :audioProcessor(p)
{
    tuningNumber = tuningIndex;
    tuning = audioProcessor->getTuning(tuningNumber); // was getCopyOfTuning
    currentScale = tuning.scale;
    currentMapping = tuning.keyboardMapping;
    tuningKeyboardTableModel = std::make_unique<TuningTableListBoxModel>(this);
    tuningKeyboardTableModel->tuningUpdated(tuning);
    tuningKeyboardTable =
        std::make_unique<juce::TableListBox>("Tuning", tuningKeyboardTableModel.get());
    tuningKeyboardTableModel->setTableListBox(tuningKeyboardTable.get());
    tuningKeyboardTableModel->setupDefaultHeaders(tuningKeyboardTable.get());
    addAndMakeVisible(*tuningKeyboardTable);

    tuningKeyboardTable->getViewport()->setScrollBarsShown(true, false);
    tuningKeyboardTable->getViewport()->setViewPositionProportionately(0.0, 48.0 / 127.0);

    sclKbmDisplay = std::make_unique<SCLKBMDisplay>(this);
    sclKbmDisplay->onTextChanged = [this](const std::string &s, const std::string &k) {
        this->onNewSCLKBM(s, k);
    };

    radialScaleGraph = std::make_unique<RadialScaleGraph>(this);
    radialScaleGraph->onToneChanged = [this](int note, double d) { this->onToneChanged(note, d); };
    radialScaleGraph->onToneStringChanged = [this](int note, const std::string &d) {
        this->onToneStringChanged(note, d);
    };
    radialScaleGraph->onScaleRescaled = [this](double d) { this->onScaleRescaled(d); };
    radialScaleGraph->onScaleRescaledAbsolute = [this](double d) {
        this->onScaleRescaledAbsolute(d);
    };

    intervalMatrix = std::make_unique<IntervalMatrix>(this);

    controlArea = std::make_unique<TuningControlArea>(this);
    addAndMakeVisible(*controlArea);

    addChildComponent(*sclKbmDisplay);
    sclKbmDisplay->setVisible(true);

    addChildComponent(*radialScaleGraph);
    radialScaleGraph->setVisible(false);

    addChildComponent(*intervalMatrix);
    intervalMatrix->setVisible(false);
    
    setTuning(tuning);
    
    startTimerHz(50);
}

TuningEditor::~TuningEditor()
{

}

void TuningEditor::resized()
{
    auto t = getTransform().inverted();
    auto h = getHeight();
    auto w = getWidth();

    int kbWidth = 130;
    int ctrlHeight = 45;

    t.transformPoint(w, h);

    tuningKeyboardTable->setBounds(0, 0, kbWidth, h);
    tuningKeyboardTable->getHeader().setColour(
        juce::TableHeaderComponent::ColourIds::highlightColourId, juce::Colours::transparentWhite);

    auto contentArea = juce::Rectangle<int>(kbWidth, 0, w - kbWidth, h - ctrlHeight);

    sclKbmDisplay->setBounds(contentArea);
    radialScaleGraph->setBounds(contentArea);
    intervalMatrix->setBounds(contentArea);
    controlArea->setBounds(kbWidth, h - ctrlHeight, w - kbWidth, ctrlHeight);

    tuningKeyboardTableModel->setMiddleCOff(1);
	
	int whichEditorPolarMode = audioProcessor->getIntState(kStateEditorPolarMode);
	if (whichEditorPolarMode >= 0 && whichEditorPolarMode <= 1)
	{
		switch (whichEditorPolarMode)
		{
			case 0:
				radialScaleGraph->setDisplayMode(0);
				radialScaleGraph->radialModeButton->setToggleState(true, juce::dontSendNotification);
				break;
			case 1:
				radialScaleGraph->setDisplayMode(1);
				radialScaleGraph->angularModeButton->setToggleState(true, juce::dontSendNotification);
				break;
			default:
				break;
		}
	}
	
	int whichEditor = audioProcessor->getIntState(kStateEditorMode);
	if (whichEditor >= 0 && whichEditor <= 5)
	{
		switch (whichEditor)
		{
			case 0:
				controlArea->selectScalaButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			case 1:
				controlArea->selectPolarButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			case 2:
				controlArea->selectIntervalButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			case 3:
				controlArea->selectToEqualButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			case 4:
				controlArea->selectRotationButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			case 5:
				controlArea->selectTrueKeysButton->setToggleState(true, juce::dontSendNotification);
				showEditor(whichEditor);
				break;
			default:
				break;
		}
	}
	else
	{
		showEditor(0);
	}
}

void TuningEditor::showEditor(int which)
{
    jassert(which >= 0 && which <= 5);
    if (controlArea->applyS)
    {
        if (which == 0)
            controlArea->applyS->setVisible(true);
        else
            controlArea->applyS->setVisible(false);
    }
    sclKbmDisplay->setVisible(which == 0);
    radialScaleGraph->setVisible(which == 1);
    intervalMatrix->setVisible(which >= 2);
    if (which == 2)
    {
        intervalMatrix->setIntervalMode();
    }
    if (which == 3)
    {
        intervalMatrix->setIntervalRelativeMode();
    }
    if (which == 4)
    {
        intervalMatrix->setRotationMode();
    }
    if (which == 5)
    {
        intervalMatrix->setTrueKeyboardMode();
    }
    
    audioProcessor->setIntState(kStateEditorMode, which);
}

void TuningEditor::onToneChanged(int tone, double newCentsValue)
{
    currentScale.tones[tone].type = Tunings::Tone::kToneCents;
    currentScale.tones[tone].cents = newCentsValue;
    recalculateScaleText();
}

void TuningEditor::onScaleRescaled(double scaleBy)
{
    
    /*
     * OK so we want a 1 cent move on top so that is top -> top+1 for scaleBy = 1
     * top ( 1 + x * scaleBy ) = top+1
     * x = top+1/top-1 since scaleBy is 1 in non shift mode
     */
    auto tCents = std::max(currentScale.tones[currentScale.count - 1].cents, 1.0);
    double sFactor = (tCents + 1) / tCents - 1;

    double scale = (1.0 + sFactor * scaleBy);
    // smallest compression is 20 cents
    static constexpr float smallestRepInterval{100};
    if (tCents <= smallestRepInterval && scaleBy < 0)
    {
        scale = smallestRepInterval / tCents;
    }

    for (auto &t : currentScale.tones)
    {
        t.type = Tunings::Tone::kToneCents;
        t.cents *= scale;
    }
    recalculateScaleText();
}

void TuningEditor::onScaleRescaledAbsolute(double riTo)
{
    /*
     * OK so we want a 1 cent move on top so that is top -> top+1 for scaleBy = 1
     * top ( 1 + x * scaleBy ) = top+1
     * x = top+1/top-1 since scaleBy is 1 in non shift mode
     */
    auto tCents = std::max(currentScale.tones[currentScale.count - 1].cents, 1.0);

    double scale = riTo / tCents;
    for (auto &t : currentScale.tones)
    {
        t.type = Tunings::Tone::kToneCents;
        t.cents *= scale;
    }
    recalculateScaleText();
}

void TuningEditor::onToneStringChanged(int tone, const std::string &newStringValue)
{
    try
    {
        auto parsed = Tunings::toneFromString(newStringValue);
        currentScale.tones[tone] = parsed;
        recalculateScaleText();
    }
    catch (Tunings::TuningError &e)
    {
        //std::cout << "Tuning Tone Conversion" << e.what() << std::endl;
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Tuning Tone Conversion", e.what());
        // If something went wrong, revert to whatever the current tuning is in the audioProcessor
        setTuning(audioProcessor->getTuning(tuningNumber)); // was getCopyOfTuning
    }
    
}

void TuningEditor::onNewSCLKBM(const std::string &scl, const std::string &kbm)
{
    try
    {
        auto s = Tunings::parseSCLData(scl);
        auto k = Tunings::parseKBMData(kbm);
        audioProcessor->applySclKbmPair(tuningNumber, s, k);
    }
    catch (const Tunings::TuningError &e)
    {
        //std::cout << "Error Applying Tuning" << e.what() << std::endl;
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error Applying Tuning", e.what());
    }
    setTuning(audioProcessor->getTuning(tuningNumber)); // was getCopyOfTuning
    repaint();

}

void TuningEditor::setMidiOnKeys(const std::bitset<128> &keys)
{
    tuningKeyboardTableModel->notesOn = keys;
    tuningKeyboardTable->repaint();
    radialScaleGraph->setNotesOn(keys);
    intervalMatrix->setNotesOn(keys);
    sclKbmDisplay->setNotesOn(keys);
}

void TuningEditor::recalculateScaleText()
{
    std::ostringstream oss;
    oss << "! Scale generated by tuning editor\n"
        << currentScale.description << "\n"
        << currentScale.count << "\n"
        << "! \n";
    for (int i = 0; i < currentScale.count; ++i)
    {
        auto tn = currentScale.tones[i];
        if (tn.type == Tunings::Tone::kToneRatio)
        {
            oss << tn.ratio_n << "/" << tn.ratio_d << "\n";
        }
        else
        {
            oss << std::fixed << std::setprecision(5) << tn.cents << "\n";
        }
    }

    try
    {
        auto str = oss.str();
        audioProcessor->retuneToScale(tuningNumber, Tunings::parseSCLData(str));
    }
    catch (const Tunings::TuningError &e)
    {
        
    }
    setTuning(audioProcessor->getTuning(tuningNumber)); // was getCopyOfTuning
}

void TuningEditor::setupForTuning(const uint32_t tuningIndex)
{
    tuningNumber = tuningIndex;
    setTuning(audioProcessor->getTuning(tuningNumber)); // was getCopyOfTuning
}

void TuningEditor::setTuning(const Tunings::Tuning &t)
{
    tuning = t;
    currentScale = tuning.scale;
    currentMapping = tuning.keyboardMapping;
    tuningKeyboardTableModel->tuningUpdated(tuning);
    sclKbmDisplay->setTuning(t);
    radialScaleGraph->setTuning(t);
    intervalMatrix->setTuning(t);

    repaint();
}

void TuningEditor::setApplyEnabled(bool b)
{
    if (controlArea->applyS)
    {
        controlArea->applyS->setEnabled(true);
        controlArea->applyS->repaint();
    }
}

void TuningEditor::setPolarMode(const int m)
{
    if (m >=0 && m <=1)
    {
        audioProcessor->setIntState(kStateEditorPolarMode, m);
    }
}

bool TuningEditor::isInterestedInFileDrag (const juce::StringArray &files)
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

void TuningEditor::filesDropped (const juce::StringArray &files, int x, int y)
{
    if (files.size() !=1)
        return;
        
	for (auto i = files.begin(); i != files.end(); ++i)
	{
		if (i->endsWith(".scl") || i->endsWith(".kbm"))
		{
			audioProcessor->applyDroppedFile(tuningNumber, i->toStdString());
		}
	}
}

void TuningEditor::timerCallback()
{
    if (audioProcessor->keysOnFlag)
    {
        setMidiOnKeys(audioProcessor->getCurrentKeysOn());
        audioProcessor->keysOnFlag = false;
    }
}

ExternalTuningEditorWindow::ExternalTuningEditorWindow(const juce::String& name, juce::Colour backgroundColour, int requiredButtons)
                            : juce::DocumentWindow (name, backgroundColour, requiredButtons)
{
}

ExternalTuningEditorWindow::~ExternalTuningEditorWindow()
{
    
}

void ExternalTuningEditorWindow::closeButtonPressed()
{
    delete this;
}
