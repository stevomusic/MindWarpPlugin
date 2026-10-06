#pragma once
#include <JuceHeader.h>
#include <array>
#include <cmath>

static inline float softClip(float x, float drive) {
    x *= drive;
    return x / (1.0f + std::abs(x));
}

static inline float hardBitCrush(float x, int bits) {
    if (bits >= 24) return x;
    float levels = std::pow(2.0f, (float)bits);
    return std::round(x * levels) / levels;
}

class VibratoFX {
public:
    void prepare(double sampleRate) {
        sr = sampleRate;
        phase = 0.0;
        buffer.resize((size_t)(sampleRate * 0.03), 0.0f);
        writePos = 0;
    }
    void setRate(float rateHz)  { rate = rateHz; }
    void setDepth(float cents)  { depth = cents; }
    float process(float in) {
        buffer[writePos] = in;
        double delaySamples = (depth / 1200.0) * sr * 0.5
                              * (1.0 + std::sin(2.0 * juce::MathConstants<double>::pi * phase));
        phase += rate / sr;
        if (phase >= 1.0) phase -= 1.0;
        double readPos = (double)writePos - delaySamples;
        while (readPos < 0) readPos += (double)buffer.size();
        size_t i0 = (size_t)readPos % buffer.size();
        size_t i1 = (i0 + 1) % buffer.size();
        float frac = (float)(readPos - std::floor(readPos));
        float out = buffer[i0] * (1.0f - frac) + buffer[i1] * frac;
        writePos = (writePos + 1) % buffer.size();
        return out;
    }
private:
    std::vector<float> buffer;
    size_t writePos = 0;
    double phase = 0.0;
    float rate = 5.0f, depth = 30.0f;
    double sr = 44100.0;
};

class ChorusFX {
public:
    void prepare(double sampleRate) {
        sr = sampleRate;
        size_t len = (size_t)(sampleRate * 0.05);
        for (auto& b : buffers) { b.resize(len, 0.0f); }
        phases = {0.0, 0.33, 0.66};
        writePos = 0;
    }
    void setRate(float rateHz)  { rate = rateHz; }
    void setDepth(float ms)     { depth = ms; }
    void setMix(float m)        { mix = m; }
    float process(float in) {
        for (auto& b : buffers) b[writePos] = in;
        float wet = 0.0f;
        for (int v = 0; v < 3; ++v) {
            phases[v] += rate / sr;
            if (phases[v] >= 1.0) phases[v] -= 1.0;
            double delaySamples = (depth * 0.001 * sr) *
                                  (0.5 + 0.5 * std::sin(2.0 * juce::MathConstants<double>::pi * phases[v]));
            double readPos = (double)writePos - delaySamples;
            if (readPos < 0) readPos += (double)buffers[v].size();
            size_t i0 = (size_t)readPos % buffers[v].size();
            size_t i1 = (i0 + 1) % buffers[v].size();
            float frac = (float)(readPos - std::floor(readPos));
            wet += buffers[v][i0] * (1.0f - frac) + buffers[v][i1] * frac;
        }
        wet /= 3.0f;
        writePos = (writePos + 1) % buffers[0].size();
        return in * (1.0f - mix) + wet * mix;
    }
private:
    std::array<std::vector<float>, 3> buffers;
    std::array<double, 3> phases{};
    size_t writePos = 0;
    float rate = 1.0f, depth = 8.0f, mix = 0.5f;
    double sr = 44100.0;
};

class ReverbFX {
public:
    void prepare(double sampleRate) {
        sr = sampleRate;
        params.roomSize = 0.6f;
        params.damping  = 0.5f;
        params.wetLevel = 0.4f;
        params.dryLevel = 0.6f;
        params.width    = 1.0f;
        rev.setParameters(params);
        rev.reset();
    }
    void setSize(float s) {
        params.roomSize = s;
        params.damping  = 1.0f - s * 0.5f;
        rev.setParameters(params);
    }
    void setMix(float m) {
        params.wetLevel = m;
        params.dryLevel = 1.0f - m;
        rev.setParameters(params);
    }
    float process(float in) {
        float out = in;
        rev.processMono(&out, 1);
        return out;
    }
    void reset() { rev.reset(); }
private:
    juce::Reverb rev;
    juce::Reverb::Parameters params;
    double sr = 44100.0;
};

class RingModFX {
public:
    void prepare(double sampleRate) { sr = sampleRate; phase = 0.0; }
    void setFreq(float hz) { freq = hz; }
    void setMix(float m)   { mix = m; }
    float process(float in) {
        float carrier = (float)std::sin(2.0 * juce::MathConstants<double>::pi * phase);
        phase += freq / sr;
        if (phase >= 1.0) phase -= 1.0;
        return in * (1.0f - mix) + (in * carrier) * mix;
    }
private:
    double phase = 0.0;
    float freq = 200.0f, mix = 0.0f;
    double sr = 44100.0;
};

class AutoWahFX {
public:
    void prepare(double sampleRate) {
        sr = sampleRate;
        phase = 0.0;
        juce::dsp::ProcessSpec spec{ sampleRate, 512, 1 };
        filter.prepare(spec);
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        filter.setCutoffFrequency(5000.0f);
        filter.setResonance(1.0f);
    }
    void setFreq(float hz)   { baseFreq = hz; }
    void setQ(float q)       { filter.setResonance(juce::jlimit(0.1f, 30.0f, q)); }
    void setLFORate(float r) { lfoRate = r; }
    float process(float in) {
        phase += lfoRate / sr;
        if (phase >= 1.0) phase -= 1.0;
        float lfo = 0.5f + 0.5f * (float)std::sin(2.0 * juce::MathConstants<double>::pi * phase);
        float cutoff = juce::jlimit(20.0f, 20000.0f, baseFreq * (0.3f + 2.5f * lfo));
        filter.setCutoffFrequency(cutoff);
        return filter.processSample(0, in);
    }
private:
    juce::dsp::StateVariableTPTFilter<float> filter;
    double phase = 0.0;
    float baseFreq = 800.0f, lfoRate = 0.0f;
    double sr = 44100.0;
};

class BitCrushFX {
public:
    void setBits(int b)       { bits = juce::jlimit(1, 24, b); }
    void setDownsample(int d) { downsampleFactor = juce::jlimit(1, 64, d); }
    float process(float in) {
        counter++;
        if (counter >= downsampleFactor) {
            counter = 0;
            held = hardBitCrush(in, bits);
        }
        return held;
    }
private:
    int bits = 16, downsampleFactor = 1, counter = 0;
    float held = 0.0f;
};

class DriveFX {
public:
    void setDrive(float d) { drive = juce::jlimit(1.0f, 400.0f, d); }
    float process(float in) {
        return softClip(in, drive) * (1.0f / std::tanh(drive * 0.01f + 0.01f));
    }
private:
    float drive = 1.0f;
};
