# KNC OFF 1.0.0 — build and verification

Windows x64 VST3, C++17, JUCE 8.0.12, CMake/MSVC 2022. Source and VST3 archives only; no executable application delivered.

## Changes

- Visualizer menu: 1/8, 1/4, 1/2, 1 BAR, 2 BAR (default). Musical note lengths follow host tempo; bar lengths follow tempo and meter. Selection recalls with saved state; older states default to two bars. Fast eighth-note windows below 100ms are supported.
- Factory and User are separate top-level preset folders. On editor/menu opening, Init plus 50 factory .kncoff files are created beneath %APPDATA%/polS Software/KNC OFF/Presets/Factory, categorized by Drums/Kick/Bass/Vocals/Synths. Existing files are preserved. User saves remain in sibling Presets/User.
- Arrows now browse one sequence: Init, the existing factory category order, then sorted User files, wrapping back to Init. Loaded User filename identifies the current position; invalid User files are skipped.
- Version identifiers and Windows/VST3 metadata are 1.0.0. Existing parameter IDs, schema 10, DSP, preset voicing, randomization safety and original artwork remain intact.

## Verification

Release build succeeded; Windows file/product and VST3 module versions are 1.0.0. CTest: 4/4 passed. Pluginval strictness 5: SUCCESS, exit 0, at 44.1/48/96/192 kHz, blocks 64/257/1024, seed 12345. EffectFeatures verifies mono centering, identical-channel stereo at Spread 100%, independent stereo feedback, alternating Ping Pong echo energy, and existing DSP features. PluginState checks locked mono Spread, equal-row selector placement, User save/load/name recall, User next/previous wrap, all-knob randomization coverage, and repeated +/-8-amplitude randomization tests below .971 peak. The prior light/dark layout is retained. New tests verify five visualizer durations at 120 BPM, fast-tempo duration, visualizer state recall, all 51 exported factory files and recipe values, and navigation across Factory/User boundaries in both directions.

FactoryHeadroom renders all 50 presets at 48/96 kHz against seeded multitone/noise, hot input and impulses up to amplitude 4, then silence for tails; mono/stereo preset transition renders retain active buffers. Highest measured sample peak: 0.613801, approximately -4.24 dBFS. Detailed test results are included.

## Limits and rebuilding

Preset headroom applies to tested signals and sample peaks. Randomized protection is a sample ceiling, not a true-peak limiter; bypass passes original input. No manual DAW audition/project acceptance, hardware listening or high-DPI acceptance was performed. Original DSP algorithms do not claim proprietary emulation.

Configure CMake x64 with JUCE_DIR pointing to JUCE 8 and BUILD_TESTING=ON, build Release, then run ctest --test-dir build -C Release --output-on-failure. Full build steps are in README.md. Verification executables are excluded from both deliveries.
