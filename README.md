# MIND WARP — Psychedelic FX Plugin
### VST3 / AU / Standalone  |  PsychedelicAudio

A fully native C++ audio plugin built with the JUCE framework.
Applies 6 simultaneous psychedelic effects: vibrato, chorus/flange,
reverb, ring modulation, auto-wah filter, and bit crushing.

---

## Requirements

| Tool | Version |
|------|---------|
| JUCE | 7.x (auto-downloaded by CMake) |
| CMake | ≥ 3.22 |
| C++ compiler | C++17 or newer |
| Windows | Visual Studio 2019/2022 **or** MinGW |
| macOS | Xcode 13+ (for AU + VST3) |
| Linux | GCC 10+, ALSA/JACK dev headers |

---

## Build — macOS / Linux

```bash
# 1. Clone or unzip this project
cd MindWarpPlugin

# 2. Configure (downloads JUCE automatically)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build
cmake --build build --config Release --parallel

# 4. Install (VST3 copied automatically)
# macOS: ~/Library/Audio/Plug-Ins/VST3/Mind Warp.vst3
# Linux: ~/.vst3/Mind Warp.vst3
```

---

## Build — Windows

```powershell
# Open "x64 Native Tools Command Prompt for VS 2022"
cd MindWarpPlugin
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
# VST3: C:\Program Files\Common Files\VST3\Mind Warp.vst3
```

---

## Linux Dependencies (Ubuntu/Debian)

```bash
sudo apt install cmake build-essential \
  libasound2-dev libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxcomposite-dev libfreetype-dev \
  libfontconfig1-dev libgl1-mesa-dev pkg-config \
  libjack-jackd2-dev
```

---

## macOS — also build as Audio Unit (AU)

The CMakeLists.txt already includes `AU` in the FORMATS list.
After building, scan in your DAW (Ableton, Logic, etc.) as normal.

---

## Effects Chain

```
Input
  │
  ├── Drive (1× – 400×, waveshaper)
  ├── Vibrato (LFO pitch modulation, 0–20 Hz, 0–200¢ depth)
  ├── Chorus/Flange (3-voice LFO delay, 0–10 Hz, 0–40 ms)
  ├── Ring Modulator (0–2000 Hz carrier, wet mix)
  ├── Auto-Wah Filter (LP, 20–20kHz, Q 0.1–30, LFO 0–10 Hz)
  ├── Bit Crusher (1–24 bit, 1–64× downsample)
  └── Reverb (Schroeder/Freeverb, room size + mix)
  │
Output Gain (-24 dB – +12 dB)
```

---

## Presets

| # | Name | Description |
|---|------|-------------|
| 0 | Default | Flat / bypass |
| 1 | Acid Trip | Fast vibrato, wah, heavy filter LFO |
| 2 | DMT Blast | Max reverb, ring mod, bit crunch |
| 3 | Space Jazz | Gentle chorus, slight ring mod, long reverb |

---

## Project Structure

```
MindWarpPlugin/
├── CMakeLists.txt          ← Build definition (fetches JUCE)
├── Source/
│   ├── PsychFX.h           ← All DSP classes (vibrato, chorus, reverb, etc.)
│   ├── PsychFX.cpp
│   ├── PluginProcessor.h   ← AudioProcessor: parameter layout + audio thread
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h      ← GUI declaration
│   └── PluginEditor.cpp    ← Psychedelic UI: knobs, groups, VU meter
└── README.md
```

---

## DAW Compatibility

Tested format targets: **VST3**, **AU** (macOS only), **Standalone app**.

Works in: Ableton Live 10+, FL Studio 20+, Reaper, Cubase, Logic Pro X,
Bitwig Studio, Studio One, Pro Tools (AAX coming soon via PACE).

---

## License

MIT. Do whatever you want with this. Make trippy music.
