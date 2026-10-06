#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// Custom look and feel for the psychedelic aesthetic
//==============================================================================
class PsychLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PsychLookAndFeel();

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawGroupComponentOutline(juce::Graphics& g, int w, int h,
                                   const juce::String& text,
                                   const juce::Justification& j,
                                   juce::GroupComponent& gc) override;

    void drawLabel(juce::Graphics& g, juce::Label& l) override;

    juce::Colour accentColour { 0xffcc44ff };
};

//==============================================================================
// A labelled knob component
//==============================================================================
class KnobSection : public juce::Component
{
public:
    KnobSection(const juce::String& paramID,
                const juce::String& labelText,
                MindWarpProcessor& p,
                juce::Colour accent);

    void resized() override;

    juce::Slider slider;
    juce::Label  label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;

private:
    juce::Colour accent;
};

//==============================================================================
// Main editor
//==============================================================================
class MindWarpEditor  : public juce::AudioProcessorEditor,
                        private juce::Timer
{
public:
    MindWarpEditor(MindWarpProcessor&);
    ~MindWarpEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void buildUI();
    void layoutSection(juce::Rectangle<int> area,
                       std::vector<KnobSection*>& knobs,
                       juce::GroupComponent& group);

    MindWarpProcessor& audioProcessor;
    PsychLookAndFeel laf;

    // Groups
    juce::GroupComponent pitchGroup, chorusGroup, reverbGroup,
                         ringGroup, filterGroup, crunchGroup;

    // Knobs
    KnobSection vibratoRateKnob, vibratoDepthKnob;
    KnobSection chorusRateKnob, chorusDepthKnob, chorusMixKnob;
    KnobSection reverbSizeKnob, reverbMixKnob;
    KnobSection ringFreqKnob, ringMixKnob;
    KnobSection filterFreqKnob, filterQKnob, filterLFOKnob;
    KnobSection driveKnob, bitCrushKnob, downsampleKnob;
    KnobSection outputGainKnob;

    // Preset combo
    juce::ComboBox presetBox;
    juce::Label    presetLabel;

    // VU meter (simple)
    float vuLevel = 0.0f;
    juce::Rectangle<int> vuRect;

    // Logo label
    juce::Label logoLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MindWarpEditor)
};
