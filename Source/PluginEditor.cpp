#include "PluginEditor.h"

//==============================================================================
// PsychLookAndFeel
//==============================================================================
PsychLookAndFeel::PsychLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId,  juce::Colour(0xffcc44ff));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a1540));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffff2df7));
    setColour(juce::Label::textColourId, juce::Colour(0xffccaaff));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0x44cc44ff));
    setColour(juce::GroupComponent::textColourId, juce::Colour(0xffaa88dd));
    setColour(juce::ComboBox::backgroundColourId,  juce::Colour(0xff1a0d2e));
    setColour(juce::ComboBox::textColourId,        juce::Colour(0xffccaaff));
    setColour(juce::ComboBox::outlineColourId,     juce::Colour(0x44cc44ff));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff120820));
    setColour(juce::PopupMenu::textColourId,       juce::Colour(0xffccaaff));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff3a1560));
    setColour(juce::PopupMenu::highlightedTextColourId, juce::Colour(0xffffffff));
}

void PsychLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                        int x, int y, int width, int height,
                                        float sliderPos,
                                        float startAngle, float endAngle,
                                        juce::Slider& slider)
{
    float radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
    float centreX = (float)x + (float)width  * 0.5f;
    float centreY = (float)y + (float)height * 0.5f;
    float rx = centreX - radius;
    float ry = centreY - radius;
    float rw = radius * 2.0f;
    float angle = startAngle + sliderPos * (endAngle - startAngle);

    // Background circle
    g.setColour(juce::Colour(0xff0d0818));
    g.fillEllipse(rx, ry, rw, rw);

    // Outer ring (track)
    juce::Path track;
    track.addCentredArc(centreX, centreY, radius - 1.0f, radius - 1.0f,
                        0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(0x33cc44ff));
    g.strokePath(track, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));

    // Filled arc
    juce::Path filled;
    filled.addCentredArc(centreX, centreY, radius - 1.0f, radius - 1.0f,
                         0.0f, startAngle, angle, true);
    juce::ColourGradient grad(juce::Colour(0xffff2df7), centreX - radius, centreY,
                               juce::Colour(0xff00f5ff), centreX + radius, centreY, false);
    g.setGradientFill(grad);
    g.strokePath(filled, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    // Pointer
    juce::Path pointer;
    float pointerLength = radius * 0.55f;
    float pointerThickness = 2.5f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius + 2.0f,
                         pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colour(0xffffffff));
    g.fillPath(pointer);

    // Centre dot
    g.setColour(juce::Colour(0xff2a1540));
    g.fillEllipse(centreX - 4.0f, centreY - 4.0f, 8.0f, 8.0f);
    g.setColour(juce::Colour(0xffcc44ff));
    g.fillEllipse(centreX - 2.5f, centreY - 2.5f, 5.0f, 5.0f);

    // Glow effect when hovered
    if (slider.isMouseOverOrDragging()) {
        g.setColour(juce::Colour(0x22cc44ff));
        g.fillEllipse(rx - 3, ry - 3, rw + 6, rw + 6);
    }
}

void PsychLookAndFeel::drawGroupComponentOutline(juce::Graphics& g,
                                                  int w, int h,
                                                  const juce::String& text,
                                                  const juce::Justification&,
                                                  juce::GroupComponent&)
{
    const float textH  = 14.0f;
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;

    juce::Font f(juce::FontOptions(textH).withStyle("Bold"));
    juce::Path p;
    float x = indent;
    float y = f.getAscent() - 3.0f;
    float pw = (float)w - x * 2.0f;
    float ph = (float)h - y - indent;
    float labelW = text.isEmpty() ? 0.0f
                                  : juce::jmin(f.getStringWidth(text) + textEdgeGap * 2.0f,
                                               pw - indent * 2.0f);
    float labelX = 14.0f;

    p.startNewSubPath(x + labelX + labelW, y);
    p.lineTo(x + pw, y);
    p.lineTo(x + pw, y + ph);
    p.lineTo(x, y + ph);
    p.lineTo(x, y);
    p.lineTo(x + labelX, y);
    p.closeSubPath();

    g.setColour(juce::Colour(0x44cc44ff));
    g.strokePath(p, juce::PathStrokeType(1.0f));

    if (!text.isEmpty()) {
        g.setColour(juce::Colour(0xffaa88dd));
        g.setFont(f);
        g.drawText(text, (int)(x + labelX), 0, (int)labelW, (int)textH,
                   juce::Justification::centred, true);
    }
}

void PsychLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& l)
{
    g.fillAll(l.findColour(juce::Label::backgroundColourId));
    if (!l.isBeingEdited()) {
        g.setColour(l.findColour(juce::Label::textColourId));
        g.setFont(juce::Font(juce::FontOptions(11.0f)));
        g.drawFittedText(l.getText(), l.getLocalBounds(), l.getJustificationType(), 2, 1.0f);
    }
}

//==============================================================================
// KnobSection
//==============================================================================
KnobSection::KnobSection(const juce::String& paramID,
                          const juce::String& labelText,
                          MindWarpProcessor& p,
                          juce::Colour accent)
    : accent(accent)
{
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffccaaff));
    slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xffaa88cc));

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        p.apvts, paramID, slider);
}

void KnobSection::resized()
{
    auto area = getLocalBounds();
    label.setBounds(area.removeFromBottom(16));
    slider.setBounds(area);
}

//==============================================================================
// MindWarpEditor
//==============================================================================
MindWarpEditor::MindWarpEditor(MindWarpProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      vibratoRateKnob  ("vibrato_rate",  "Rate",    p, juce::Colour(0xffff2df7)),
      vibratoDepthKnob ("vibrato_depth", "Depth",   p, juce::Colour(0xffff2df7)),
      chorusRateKnob   ("chorus_rate",   "Rate",    p, juce::Colour(0xff00f5ff)),
      chorusDepthKnob  ("chorus_depth",  "Depth",   p, juce::Colour(0xff00f5ff)),
      chorusMixKnob    ("chorus_mix",    "Mix",     p, juce::Colour(0xff00f5ff)),
      reverbSizeKnob   ("reverb_size",   "Size",    p, juce::Colour(0xffaaff00)),
      reverbMixKnob    ("reverb_mix",    "Mix",     p, juce::Colour(0xffaaff00)),
      ringFreqKnob     ("ring_freq",     "Freq",    p, juce::Colour(0xffff6a00)),
      ringMixKnob      ("ring_mix",      "Mix",     p, juce::Colour(0xffff6a00)),
      filterFreqKnob   ("filter_freq",   "Cutoff",  p, juce::Colour(0xffcc40ff)),
      filterQKnob      ("filter_q",      "Reso",    p, juce::Colour(0xffcc40ff)),
      filterLFOKnob    ("filter_lfo",    "LFO Spd", p, juce::Colour(0xffcc40ff)),
      driveKnob        ("drive",         "Drive",   p, juce::Colour(0xffff2060)),
      bitCrushKnob     ("bitcrush",      "Bits",    p, juce::Colour(0xffff2060)),
      downsampleKnob   ("downsample",    "Smash",   p, juce::Colour(0xffff2060)),
      outputGainKnob   ("output_gain",   "Output",  p, juce::Colour(0xffffcc00))
{
    setLookAndFeel(&laf);
    setSize(760, 520);

    // Logo
    addAndMakeVisible(logoLabel);
    logoLabel.setText("MIND WARP", juce::dontSendNotification);
    logoLabel.setFont(juce::Font(juce::FontOptions(32.0f).withStyle("Bold")));
    logoLabel.setColour(juce::Label::textColourId, juce::Colour(0xffcc44ff));
    logoLabel.setJustificationType(juce::Justification::centredLeft);

    // Groups
    for (auto* g : {&pitchGroup, &chorusGroup, &reverbGroup,
                    &ringGroup, &filterGroup, &crunchGroup}) {
        addAndMakeVisible(g);
    }
    pitchGroup .setText("Pitch Warp");
    chorusGroup.setText("Chorus / Flange");
    reverbGroup.setText("Space Reverb");
    ringGroup  .setText("Ring Mod");
    filterGroup.setText("Psycho Filter");
    crunchGroup.setText("Bit Crunch");

    // Knobs
    for (auto* k : {&vibratoRateKnob, &vibratoDepthKnob,
                    &chorusRateKnob, &chorusDepthKnob, &chorusMixKnob,
                    &reverbSizeKnob, &reverbMixKnob,
                    &ringFreqKnob, &ringMixKnob,
                    &filterFreqKnob, &filterQKnob, &filterLFOKnob,
                    &driveKnob, &bitCrushKnob, &downsampleKnob,
                    &outputGainKnob}) {
        addAndMakeVisible(k);
    }

    // Preset combo
    addAndMakeVisible(presetBox);
    for (int i = 0; i < p.getNumPrograms(); ++i)
        presetBox.addItem(p.getProgramName(i), i + 1);
    presetBox.setSelectedId(p.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this, &p] {
        p.setCurrentProgram(presetBox.getSelectedId() - 1);
    };

    addAndMakeVisible(presetLabel);
    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xff886699));
    presetLabel.setJustificationType(juce::Justification::centredLeft);

    startTimerHz(30);
}

MindWarpEditor::~MindWarpEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

//==============================================================================
void MindWarpEditor::timerCallback()
{
    repaint(vuRect);
}

//==============================================================================
void MindWarpEditor::paint(juce::Graphics& g)
{
    // Deep dark background
    g.fillAll(juce::Colour(0xff050308));

    // Subtle gradient overlay
    juce::ColourGradient bg(juce::Colour(0xff120820), 0.0f, 0.0f,
                            juce::Colour(0xff050308), (float)getWidth(), (float)getHeight(), false);
    g.setGradientFill(bg);
    g.fillAll();

    // Ambient glow blobs
    auto drawBlob = [&](float cx, float cy, float r, juce::Colour c) {
        juce::ColourGradient blob(c.withAlpha(0.12f), cx, cy,
                                   c.withAlpha(0.0f),  cx + r, cy, true);
        g.setGradientFill(blob);
        g.fillEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f);
    };
    drawBlob(100, 100, 200, juce::Colour(0xffcc44ff));
    drawBlob(660, 400, 180, juce::Colour(0xff00f5ff));
    drawBlob(400, 500, 150, juce::Colour(0xffaaff00));

    // Top separator line
    g.setColour(juce::Colour(0x33cc44ff));
    g.drawHorizontalLine(52, 0.0f, (float)getWidth());

    // Bottom bar
    g.setColour(juce::Colour(0x22cc44ff));
    g.fillRect(0, getHeight() - 38, getWidth(), 38);

    // VU meter
    g.setColour(juce::Colour(0xff1a0d2e));
    g.fillRect(vuRect);
    g.setColour(juce::Colour(0x44cc44ff));
    g.drawRect(vuRect);
    if (vuLevel > 0.001f) {
        juce::ColourGradient vu(juce::Colour(0xff00f5ff), vuRect.getX(), 0.0f,
                                 juce::Colour(0xffff2060),
                                 vuRect.getX() + vuRect.getWidth(), 0.0f, false);
        g.setGradientFill(vu);
        g.fillRect(vuRect.withWidth((int)(vuRect.getWidth() * juce::jmin(1.0f, vuLevel))));
    }

    // Subtitle
    g.setColour(juce::Colour(0xff554466));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("PSYCHEDELIC FX PROCESSOR  |  v1.0  |  PsychedelicAudio",
               0, 34, getWidth(), 16, juce::Justification::centred);
}

void MindWarpEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    // Top bar
    auto topBar = area.removeFromTop(52);
    logoLabel.setBounds(topBar.removeFromLeft(220).withTrimmedTop(8));
    presetLabel.setBounds(topBar.removeFromLeft(60).withTrimmedTop(20));
    presetBox  .setBounds(topBar.removeFromLeft(160).withTrimmedTop(16).withHeight(24));

    // VU meter on the right of top bar
    vuRect = topBar.withTrimmedTop(20).withHeight(12)
                   .withTrimmedLeft(20).withTrimmedRight(10);

    area.removeFromTop(4);

    // 3 columns of sections
    int colW = area.getWidth() / 3;
    auto col1 = area.removeFromLeft(colW).reduced(4);
    auto col2 = area.removeFromLeft(colW).reduced(4);
    auto col3 = area.reduced(4);

    int sectionH = (col1.getHeight() - 8) / 2;

    // Col 1: Pitch Warp + Ring Mod
    auto pitchArea = col1.removeFromTop(sectionH);
    col1.removeFromTop(8);
    auto ringArea = col1;

    pitchGroup.setBounds(pitchArea);
    {
        auto inner = pitchArea.reduced(10, 18);
        int kw = inner.getWidth() / 2;
        vibratoRateKnob .setBounds(inner.removeFromLeft(kw));
        vibratoDepthKnob.setBounds(inner);
    }

    ringGroup.setBounds(ringArea);
    {
        auto inner = ringArea.reduced(10, 18);
        int kw = inner.getWidth() / 2;
        ringFreqKnob.setBounds(inner.removeFromLeft(kw));
        ringMixKnob .setBounds(inner);
    }

    // Col 2: Chorus + Filter
    auto chorusArea = col2.removeFromTop(sectionH);
    col2.removeFromTop(8);
    auto filterArea = col2;

    chorusGroup.setBounds(chorusArea);
    {
        auto inner = chorusArea.reduced(10, 18);
        int kw = inner.getWidth() / 3;
        chorusRateKnob .setBounds(inner.removeFromLeft(kw));
        chorusDepthKnob.setBounds(inner.removeFromLeft(kw));
        chorusMixKnob  .setBounds(inner);
    }

    filterGroup.setBounds(filterArea);
    {
        auto inner = filterArea.reduced(10, 18);
        int kw = inner.getWidth() / 3;
        filterFreqKnob.setBounds(inner.removeFromLeft(kw));
        filterQKnob   .setBounds(inner.removeFromLeft(kw));
        filterLFOKnob .setBounds(inner);
    }

    // Col 3: Reverb + Bit Crunch + Output
    auto reverbArea = col3.removeFromTop(sectionH);
    col3.removeFromTop(8);
    auto crunchArea = col3.removeFromTop(sectionH - 50);
    col3.removeFromTop(4);
    auto outArea = col3;

    reverbGroup.setBounds(reverbArea);
    {
        auto inner = reverbArea.reduced(10, 18);
        int kw = inner.getWidth() / 2;
        reverbSizeKnob.setBounds(inner.removeFromLeft(kw));
        reverbMixKnob .setBounds(inner);
    }

    crunchGroup.setBounds(crunchArea);
    {
        auto inner = crunchArea.reduced(10, 18);
        int kw = inner.getWidth() / 3;
        driveKnob     .setBounds(inner.removeFromLeft(kw));
        bitCrushKnob  .setBounds(inner.removeFromLeft(kw));
        downsampleKnob.setBounds(inner);
    }

    // Output gain — standalone knob
    outputGainKnob.setBounds(outArea.withSizeKeepingCentre(70, 80));
}
