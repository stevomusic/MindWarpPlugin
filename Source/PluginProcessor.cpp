#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout MindWarpProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("vibrato_rate","Vibrato Rate",juce::NormalisableRange<float>(0.0f,20.0f,0.01f,0.5f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return v<1000?juce::String(v,1)+"Hz":juce::String(v/1000.f,2)+"kHz";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("vibrato_depth","Vibrato Depth",juce::NormalisableRange<float>(0.0f,200.0f,0.1f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,0)+"c";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorus_rate","Chorus Rate",juce::NormalisableRange<float>(0.0f,10.0f,0.01f,0.5f),0.5f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return v<1000?juce::String(v,1)+"Hz":juce::String(v/1000.f,2)+"kHz";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorus_depth","Chorus Depth",juce::NormalisableRange<float>(0.0f,40.0f,0.1f),5.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,1)+"ms";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chorus_mix","Chorus Mix",juce::NormalisableRange<float>(0.0f,1.0f,0.001f),0.5f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v*100.f,0)+"%";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverb_size","Reverb Size",juce::NormalisableRange<float>(0.0f,1.0f,0.001f),0.6f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v*100.f,0)+"%";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverb_mix","Reverb Mix",juce::NormalisableRange<float>(0.0f,1.0f,0.001f),0.4f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v*100.f,0)+"%";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ring_freq","Ring Freq",juce::NormalisableRange<float>(0.0f,2000.0f,0.1f,0.4f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return v<1000?juce::String(v,1)+"Hz":juce::String(v/1000.f,2)+"kHz";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ring_mix","Ring Mix",juce::NormalisableRange<float>(0.0f,1.0f,0.001f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v*100.f,0)+"%";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filter_freq","Filter Freq",juce::NormalisableRange<float>(20.0f,20000.0f,1.0f,0.3f),5000.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return v<1000?juce::String(v,1)+"Hz":juce::String(v/1000.f,2)+"kHz";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filter_q","Filter Q",juce::NormalisableRange<float>(0.1f,30.0f,0.01f,0.5f),1.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,2);})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filter_lfo","Filter LFO",juce::NormalisableRange<float>(0.0f,10.0f,0.01f,0.5f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return v<1000?juce::String(v,1)+"Hz":juce::String(v/1000.f,2)+"kHz";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive","Drive",juce::NormalisableRange<float>(1.0f,400.0f,0.1f,0.3f),1.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,0)+"x";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bitcrush","Bit Depth",juce::NormalisableRange<float>(1.0f,24.0f,1.0f),24.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String((int)v)+"bit";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("downsample","Downsample",juce::NormalisableRange<float>(1.0f,64.0f,1.0f),1.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,0)+"x";})));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("output_gain","Output Gain",juce::NormalisableRange<float>(-24.0f,12.0f,0.1f),0.0f,juce::AudioParameterFloatAttributes{}.withStringFromValueFunction([](float v,int){return juce::String(v,1)+"dB";})));

    return { params.begin(), params.end() };
}

MindWarpProcessor::MindWarpProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", createParameterLayout())
{
    pVibratoRate  = apvts.getRawParameterValue("vibrato_rate");
    pVibratoDepth = apvts.getRawParameterValue("vibrato_depth");
    pChorusRate   = apvts.getRawParameterValue("chorus_rate");
    pChorusDepth  = apvts.getRawParameterValue("chorus_depth");
    pChorusMix    = apvts.getRawParameterValue("chorus_mix");
    pReverbSize   = apvts.getRawParameterValue("reverb_size");
    pReverbMix    = apvts.getRawParameterValue("reverb_mix");
    pRingFreq     = apvts.getRawParameterValue("ring_freq");
    pRingMix      = apvts.getRawParameterValue("ring_mix");
    pFilterFreq   = apvts.getRawParameterValue("filter_freq");
    pFilterQ      = apvts.getRawParameterValue("filter_q");
    pFilterLFO    = apvts.getRawParameterValue("filter_lfo");
    pDrive        = apvts.getRawParameterValue("drive");
    pBitCrush     = apvts.getRawParameterValue("bitcrush");
    pDownsample   = apvts.getRawParameterValue("downsample");
    pOutputGain   = apvts.getRawParameterValue("output_gain");
}

MindWarpProcessor::~MindWarpProcessor() {}

void MindWarpProcessor::prepareToPlay(double sampleRate, int)
{
    for (int ch = 0; ch < 2; ++ch) {
        vibrato [ch].prepare(sampleRate);
        chorus  [ch].prepare(sampleRate);
        reverb  [ch].prepare(sampleRate);
        ringMod [ch].prepare(sampleRate);
        autoWah [ch].prepare(sampleRate);
    }
}

void MindWarpProcessor::releaseResources()
{
    for (int ch = 0; ch < 2; ++ch)
        reverb[ch].reset();
}

bool MindWarpProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void MindWarpProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    float vibratoRate  = *pVibratoRate;
    float vibratoDepth = *pVibratoDepth;
    float chorusRate   = *pChorusRate;
    float chorusDepth  = *pChorusDepth;
    float chorusMix    = *pChorusMix;
    float reverbSize   = *pReverbSize;
    float reverbMix    = *pReverbMix;
    float ringFreq     = *pRingFreq;
    float ringMix      = *pRingMix;
    float filterFreq   = *pFilterFreq;
    float filterQ      = *pFilterQ;
    float filterLFO    = *pFilterLFO;
    float driveAmt     = *pDrive;
    int   bits         = (int)*pBitCrush;
    int   dsAmt        = (int)*pDownsample;
    float outGain      = juce::Decibels::decibelsToGain((float)*pOutputGain);

    int numChannels = buffer.getNumChannels();
    int numSamples  = buffer.getNumSamples();

    for (int ch = 0; ch < std::min(numChannels, 2); ++ch) {
        vibrato [ch].setRate(vibratoRate);
        vibrato [ch].setDepth(vibratoDepth);
        chorus  [ch].setRate(chorusRate);
        chorus  [ch].setDepth(chorusDepth);
        chorus  [ch].setMix(chorusMix);
        reverb  [ch].setSize(reverbSize);
        reverb  [ch].setMix(reverbMix);
        ringMod [ch].setFreq(ringFreq);
        ringMod [ch].setMix(ringMix);
        autoWah [ch].setFreq(filterFreq);
        autoWah [ch].setQ(filterQ);
        autoWah [ch].setLFORate(filterLFO);
        drive   [ch].setDrive(driveAmt);
        bitCrush[ch].setBits(bits);
        bitCrush[ch].setDownsample(dsAmt);
    }

    for (int ch = 0; ch < std::min(numChannels, 2); ++ch) {
        float* channelData = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i) {
            float s = channelData[i];
            s = drive   [ch].process(s);
            s = vibrato [ch].process(s);
            s = chorus  [ch].process(s);
            s = ringMod [ch].process(s);
            s = autoWah [ch].process(s);
            s = bitCrush[ch].process(s);
            s = reverb  [ch].process(s);
            channelData[i] = s * outGain;
        }
    }
}

struct Preset {
    const char* name;
    float vibratoRate, vibratoDepth;
    float chorusRate, chorusDepth, chorusMix;
    float reverbSize, reverbMix;
    float ringFreq, ringMix;
    float filterFreq, filterQ, filterLFO;
    float drive;
    float bits, downsample;
};

static const Preset kPresets[] = {
    { "Default",    0,0,  0.5f,5,0.5f,  0.6f,0.4f,  0,0,  5000,1,0,  1,  24,1 },
    { "Acid Trip",  5,40, 2.0f,15,0.6f, 0.4f,0.3f,  0,0,  800,18,2,  50, 16,1 },
    { "DMT Blast",  8,80, 0.3f,25,0.8f, 0.95f,0.85f,120,0.5f, 3000,5,0.3f, 200, 6,4 },
    { "Space Jazz", 3,20, 1.0f,10,0.4f, 0.6f,0.5f,  440,0.2f, 8000,2,0.5f, 5, 14,1 },
};

void MindWarpProcessor::setCurrentProgram(int index) {
    if (index < 0 || index >= getNumPrograms()) return;
    currentProgram = index;
    const auto& p = kPresets[index];
    apvts.getParameter("vibrato_rate") ->setValueNotifyingHost(apvts.getParameter("vibrato_rate") ->convertTo0to1(p.vibratoRate));
    apvts.getParameter("vibrato_depth")->setValueNotifyingHost(apvts.getParameter("vibrato_depth")->convertTo0to1(p.vibratoDepth));
    apvts.getParameter("chorus_rate")  ->setValueNotifyingHost(apvts.getParameter("chorus_rate")  ->convertTo0to1(p.chorusRate));
    apvts.getParameter("chorus_depth") ->setValueNotifyingHost(apvts.getParameter("chorus_depth") ->convertTo0to1(p.chorusDepth));
    apvts.getParameter("chorus_mix")   ->setValueNotifyingHost(apvts.getParameter("chorus_mix")   ->convertTo0to1(p.chorusMix));
    apvts.getParameter("reverb_size")  ->setValueNotifyingHost(apvts.getParameter("reverb_size")  ->convertTo0to1(p.reverbSize));
    apvts.getParameter("reverb_mix")   ->setValueNotifyingHost(apvts.getParameter("reverb_mix")   ->convertTo0to1(p.reverbMix));
    apvts.getParameter("ring_freq")    ->setValueNotifyingHost(apvts.getParameter("ring_freq")    ->convertTo0to1(p.ringFreq));
    apvts.getParameter("ring_mix")     ->setValueNotifyingHost(apvts.getParameter("ring_mix")     ->convertTo0to1(p.ringMix));
    apvts.getParameter("filter_freq")  ->setValueNotifyingHost(apvts.getParameter("filter_freq")  ->convertTo0to1(p.filterFreq));
    apvts.getParameter("filter_q")     ->setValueNotifyingHost(apvts.getParameter("filter_q")     ->convertTo0to1(p.filterQ));
    apvts.getParameter("filter_lfo")   ->setValueNotifyingHost(apvts.getParameter("filter_lfo")   ->convertTo0to1(p.filterLFO));
    apvts.getParameter("drive")        ->setValueNotifyingHost(apvts.getParameter("drive")        ->convertTo0to1(p.drive));
    apvts.getParameter("bitcrush")     ->setValueNotifyingHost(apvts.getParameter("bitcrush")     ->convertTo0to1(p.bits));
    apvts.getParameter("downsample")   ->setValueNotifyingHost(apvts.getParameter("downsample")   ->convertTo0to1(p.downsample));
}

const juce::String MindWarpProcessor::getProgramName(int index) {
    if (index < 0 || index >= getNumPrograms()) return {};
    return kPresets[index].name;
}

void MindWarpProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MindWarpProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MindWarpProcessor();
}

juce::AudioProcessorEditor* MindWarpProcessor::createEditor()
{
    return new MindWarpEditor(*this);
}
