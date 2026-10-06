#pragma once
#include <JuceHeader.h>
#include "PsychFX.h"

//==============================================================================
class MindWarpProcessor  : public juce::AudioProcessor
{
public:
    MindWarpProcessor();
    ~MindWarpProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "Mind Warp"; }
    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    //==============================================================================
    int getNumPrograms() override { return 4; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation  (juce::MemoryBlock& destData) override;
    void setStateInformation  (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    // Per-channel FX
    std::array<VibratoFX,  2> vibrato;
    std::array<ChorusFX,   2> chorus;
    std::array<ReverbFX,   2> reverb;
    std::array<RingModFX,  2> ringMod;
    std::array<AutoWahFX,  2> autoWah;
    std::array<BitCrushFX, 2> bitCrush;
    std::array<DriveFX,    2> drive;

    int currentProgram = 0;

    // Parameter pointers (atomic)
    std::atomic<float>* pVibratoRate  = nullptr;
    std::atomic<float>* pVibratoDepth = nullptr;
    std::atomic<float>* pChorusRate   = nullptr;
    std::atomic<float>* pChorusDepth  = nullptr;
    std::atomic<float>* pChorusMix    = nullptr;
    std::atomic<float>* pReverbSize   = nullptr;
    std::atomic<float>* pReverbMix    = nullptr;
    std::atomic<float>* pRingFreq     = nullptr;
    std::atomic<float>* pRingMix      = nullptr;
    std::atomic<float>* pFilterFreq   = nullptr;
    std::atomic<float>* pFilterQ      = nullptr;
    std::atomic<float>* pFilterLFO    = nullptr;
    std::atomic<float>* pDrive        = nullptr;
    std::atomic<float>* pBitCrush     = nullptr;
    std::atomic<float>* pDownsample   = nullptr;
    std::atomic<float>* pOutputGain   = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MindWarpProcessor)
};
