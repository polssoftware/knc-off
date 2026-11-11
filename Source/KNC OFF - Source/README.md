# KNC OFF 1.0.0 â€” polS Software

Windows x64 VST3 / C++17 / JUCE 8.0.12 / CMake. Original DSP; no exact proprietary emulation claim. VST3 and source only, without a standalone application.

## Install

Extract the VST3 download, then copy the entire `KNC OFF.vst3` folder into `C:\Program Files\Common Files\VST3`, preserving its Contents hierarchy. Close your DAW before replacing the binary, then rescan. No system-wide installation was performed during this task.

## Artwork and controls

The latest reference supplies the original main GUI lettering, KNC OFF title, PRESETS/BYPASS titles, effect labels, selectors, brackets and centred footer controls. These main labels are not replaced by the alphabet system. The SUB note readout and plugin menus use crops of the supplied alphabet and earlier digits. Menu glyphs share one source-sheet scale, preserving their original relative dimensions rather than stretching every character to the same height.

Knobs rotate cropped original drawings. Drag vertically to adjust; double-click to reset. Hold Shift while moving any knob or slider to snap to whole numbers within its range. Drag horizontally to reorder the eight main effects or the four auxiliary effects within their own rows. Routing changes real processing, and both orders save in presets/session state. There are no square hold highlights.

SAT's 1/2/3 select three saturation modes. CLIP's S/H choose soft/hard curves. SUB's note readout scrolls or drags from C1 to C2; right-click changes sharp/flat spelling. The adjacent vertical SUB decay slider controls 0–100% (20 ms–2 s envelope release), defaults to 20%, and travels with SUB when reordered. The DLY/RVB/CHR brackets open their menus. PRESETS offers factory starting points and Save/Load; About is removed. The original title retains its subtle audio-driven opacity pulse.

Hover values use the supplied speech-bubble image and cropped number/sign/unit artwork, with the bottom arrow aligned over the knob. IN/OUT are +/-24 dB. LOW/MID/HIGH now span -24 to +24 dB with 0 dB centred. WID is -100% mono, 0% original stereo, +100% expanded width. Main HP and LP both cover 20â€“20000 Hz; their hover readouts show actual frequency, such as 20Hz, 6.5kHz or 20kHz, rather than percentages.

The waveform defaults to two bars, following the DAW tempo and time signature (120 BPM and 4/4 fallback: 4 seconds). Click it to choose 2 BARS or one of the shorter millisecond windows. The two-bar view uses stereo min/max history, fixed amplitude scaling and only the supplied cropped hand-drawn waveform lines. Positive/negative levels remain independent. Longer windows aggregate 10 ms extrema without losing transient peaks; no new stroke artwork is generated. Silence has no flat baseline.

## Menus

Menus have plain white backgrounds, the cropped alphabet/digits, and tracks/rectangle handles taken from the separate supplied slider image. Numeric readouts are read-only to preserve bitmap lettering; adjust through sliders/scroll wheel. Native operating-system file dialogs remain native.

- DLY: BEAT/MS share the Time row (default 1/4 beat). Spread has vertically stacked Mono/Stereo/Ping Pong selectors beside it; Stereo is default. Mono pins Spread to 0% and disables its slider. Spread, Ducking, High Pass, Low Pass, Dry and linked Mix follow. Ducking is one 0-100% control, default 0. Dry defaults to 100%.
- RVB: ROOM/HALL/PLATE selectors at the top; Room is default. Rows are Decay (0.1-20 s), Size, Pre-Delay (0-250 ms), Diffusion (0-100%, default 65%), Width, Ducking, High Pass, Low Pass, Dry and linked Mix. Dry defaults to 100%, Ducking to 0.
- CHR: Feedback (0-80%), Delay (0.1-40 ms, default 14 ms), Depth, Rate, Spread and linked Mix. The former chorusDecay parameter ID remains the feedback control for saved-state compatibility.

Values use compact formatting: 6000 Hz -> 6kHz; 6500 -> 6.5kHz; 19700 -> 19.7kHz; 20000 -> 20kHz. Seconds, Hz and percentages appear after the number, not in the slider title. The alphabet sheet is uppercase, so rendered unit letters retain that drawn style.

At 100% feedback, delay echoes can sustain. Feedback buffer writes are bounded at +/-16 internal amplitude to prevent runaway numerical growth; reported tail length is capped at 600 seconds. Reverb Width values above 100% in old states clamp to the new range. State schema is now 10, retaining existing parameter IDs. Existing normalized DAW automation should be reviewed after range changes.

## Signal flow and DSP

IN -> reorderable KNC/SAT/SUB/AIR/CLIP/COMP/SHFT/BIT -> HP/LP -> reorderable DLY/RVB/CHR/WID -> L/M/H EQ -> global MIX -> OUT -> smoothed bypass.

SMAK (formerly KNC) has five transient profiles; COMP has five internally calibrated modes; SAT selects Tape/Tube/Transistor algorithms. SUB adds an input-triggered sine layer, AIR enhances high frequencies, CLIP blends driven clipping plus the final processed-path ceiling, SHFT pitches +/-12 semitones, and BIT mixes depth/sample-rate reduction. New menu controls have real DSP. Continuous parameter changes and mode transitions smooth; buffers allocate during prepare only. These are original algorithms, not exact proprietary/hardware emulations.

Init is neutral: effects zero, WID zero/original stereo, gains 0 dB, EQ flat, HP 20 Hz/off, LP 20000 Hz/off, global MIX 100%. The legacy hidden VIBE parameter remains functional for old-session compatibility.

## Build and tests

Requires Visual Studio 2022 Desktop development with C++, Windows SDK, CMake 3.22+ and a separately supplied JUCE 8.0.12 source checkout.

```powershell
cmake -S . -B build -A x64 -DCMAKE_SUPPRESS_REGENERATION=ON -DBUILD_TESTING=OFF -DJUCE_DIR="C:/path/to/JUCE"
cmake --build build --config Release --target KNCOff_VST3 --parallel 1 -- /nr:false
```

Bundle: `build/KNCOff_artefacts/Release/VST3/KNC OFF.vst3`. Reconfigure after CMakeLists changes because regeneration is disabled to avoid a project-check stall seen on this machine. Standalone format is not configured.

For verification, use a separate build with BUILD_TESTING=ON, build, then run `ctest --test-dir build -C Release --output-on-failure`. Development verification executables are not shipped. Tests cover mono/stereo 44.1â€“192 kHz, DSP action, neutral/bypass/mix behavior, generated sub frequency, delay beat/ms timing, unity feedback/20-second reverb stability, formatting/ranges, state/routing recall, menu links, button order, selector hitboxes, hover bubbles and scrolling. BUILD-RESULTS.md and included logs record observed outcomes.

Known limits: native-rate nonlinear processing can alias; no final true-peak safety limiter; output can exceed 0 dBFS. Pitch shifting can have grain/modulation artifacts. No manual DAW-specific audition/project acceptance or high-DPI verification was performed. Advanced modules, full eight-control macro banks, oversampling, repeated module instances, undo and locks remain future work. See ARCHITECTURE.md and JUCE-LICENSE.md.

## 0.22.0 waveform and menu update

BEAT/MS have dedicated artwork-rendered buttons beside DLY, with a drawn underscore under the active choice. Slider tracks and handles are direct crops of the latest menu reference image and appear in DLY, RVB and CHR. Other text continues to use the supplied alphabet and numbers.

The sample ring is allocated before audio starts. Capturing extrema across channels avoids cancellation of opposite stereo polarity. Min/max aggregation preserves single-sample transients and asymmetric waveforms without automatic gain. The original hand-drawn textures scale to each segment's actual bounds. Display time choices are internal view settings and reset to two bars when a processor is created. Supported validation rates are 44.1–192 kHz; the fixed ring holds over two seconds at 192 kHz.

SUB decay, Shift snapping, all existing parameter IDs remain unchanged. No proprietary DSP emulation is claimed.

## Factory presets

PRESETS has five categories, each with ten distinct starting points (50 total), plus neutral Init:

- Drums: Punch, Glue, Crunch, Room, Broken, Smash, Ripped Tape, Industrial, Broken Room, Fuzz Bus.
- Kick: Weight, Click, Drive, Deep, LoFi, Concrete, Furnace, Sub Hammer, Crushed Punch, Rust.
- Bass: Warm, Round, Growl, Wide, Pluck, Torn Speaker, Acid Grind, Fuzz Octave, Metal Bass, Wrecked Pluck.
- Vocals: Lift, Intimate, Airy, Double, Radio, Megaphone, Razor Double, Octave Dirt, Burnt Radio, Ghost Drive.
- Synths: Shine, Wide, Plush, Retro, Motion, Circuit Melt, Broken Arcade, Fuzz Cloud, Acid Storm, Digital Fire.


Each recipe explicitly assigns the full parameter set. The bank uses transient/compression/saturation/sub/air/clipping, pitch/bit effects, gains/filters, delay/reverb/chorus/width, EQ and global mix with different balances. Pitch changes are reserved for character presets. Bypass stays off. Adjust to the input material; these are starting points, not loudness-matched masters.

Menu controls are now also painted in the panel's final artwork pass, so the supplied track/handle and BEAT/MS labels/selected underscore remain visible independently of inherited child styling. The latest reference is used for KNC/SAT/SUB labels and the original knob sprites. This release was built in a fresh build directory; VST3 module metadata and Windows file resources are checked for version 0.22.0.

## Header and themes (0.13)

The preset brackets show the current factory preset name using the supplied alphabet. The adjacent left/right arrows cycle all 51 programs, wrapping at each end. The PRESETS region still opens category menus and Save/Load. Selecting a factory program updates both audio parameters and this readout.

The power icon toggles host-automatable bypass: normal theme color means processing enabled; red means bypassed. Sun selects light, moon selects dark, with a drawn underscore marking the active theme. Light uses the new light reference; dark uses the supplied black main reference, white alphabet/numbers/symbols/slider parts, and white waveform image. Theme is per instance and saved in session/preset state, defaults to light for older states, and never changes audio processing. Existing theme is retained while cycling factory programs.

Dark DLY/RVB/CHR menus and preset/time-window popups use the same drawn artwork on black paper. Operating-system file picker dialogs remain native. The current factory program name is displayed after loading saved state; edited settings retain that program label.

## Aggressive bank and underlines (0.14)

Each category now has five additional aggressive recipes. All processing knobs are active in those additions, with input/output held at unity, including pitch, bit reduction, effects, EQ, filters and width, with global mix fully wet. Saturation is 72-96%, clipping is fully engaged in a deliberately selected soft or hard mode, and input/output are 0 dB and global mix is 100%. Feedback stays below 60%. Tests exercise hot signals and tails; this is preset headroom, not a new global output limiter or a guarantee for arbitrary user gain changes.

Preset navigation is Init, all ten Drums, all ten Kick, all ten Bass, all ten Vocals, all ten Synths, then wraps. Schema 9 remaps older saved factory program indices to their original names without changing stored sound values. New Init/reset SUB decay is 20%; saved explicit decay values remain intact. States predating the decay parameter retain their original approximate release through migration.

All active underlines use the original supplied underscore sprite with ink-preserving area reduction, including SAT, CLIP, BEAT/MS and sun/moon. No replacement line graphics are generated.

## Remembered theme and unity presets (0.15)

Sun/moon clicks save a preference to the user's application-data folder: `polS Software/KNC OFF/theme.txt`. Every newly constructed instance reads that file; the choice persists across host restarts. Existing instances keep their own theme. Explicit theme stored in an opened project/preset overrides the instance display but does not overwrite the global preference. A missing preference starts in day mode. File access occurs on construction/UI selection, never inside audio processing.

All 50 factory presets now use IN 0 dB, OUT 0 dB and global MIX 100%. Init already uses these values. Loading existing saved projects/presets continues to restore their stored gains/mix. Seven aggressive recipes have EQ adjusted for headroom after removing the output trims; no hidden output-gain compensation or global limiter was added. Theme test runs use `KNC_OFF_SETTINGS_DIR` to isolate their preferences from the real user setting.

## Clipper selector and compressor update (0.16)

Both supplied main references replace the previous light/dark artwork. CLIP's S/H selection split and original underscore indicator move six design pixels right to follow the new drawings; the hit regions continue to travel with the module when reordered.

The 25 aggressive recipes now use COMP 22%, 28%, 34%, 40% or 46%, respectively, across each category's five aggressive entries. In 0.22 all 50 recipes are rebuilt with the new modes, effect controls, and individually assigned IN/OUT/global MIX. Existing saved states are not overwritten.

The previous post-effect overs are addressed in 0.18 by a final processed-path ceiling tied to CLIP amount and S/H mode. At the aggressive presets' full CLIP and MIX settings with OUT 0 dB, sample peaks stay at or below 0.97 (about -0.26 dBFS). Soft mode has a smooth knee; hard mode caps directly. Partial CLIP blends that ceiling with the processed signal. Dry mix, bypass, routing crossfades, manual output boosts and true-peak reconstruction are not represented as universally limited output.

## Randomize (0.17)

PRESETS has a Randomize command immediately below Load preset. It randomizes all processing knobs plus effect-menu settings and processing modes. As of 0.23, IN, OUT and global MIX are included; bypass, theme and module order are preserved. A final sample ceiling protects randomized states. Each parameter change notifies the host and uses the existing smoothing. The header reads Randomized, and the generated sound/label recall in saved state. Selecting a factory program clears that label.

Randomization uses defined sound-design ranges rather than unrestricted EQ/feedback extremes: EQ +/-6 dB, auxiliary wet controls up to 40%, feedback up to 65%, reverb decay up to 6 seconds, chorus feedback/decay up to 45%, high passes 20–1000 Hz, low passes 3500–20000 Hz, width +/-60%, SUB up to 55%, AIR up to 60%, and KNC up to 70%. Other controls use their legal ranges, with discrete controls kept on valid steps. The final CLIP ceiling follows the randomized CLIP amount/mode, so partial clip or a positive manual OUT setting can still permit overs.

## Stereo meter and footer (0.18)

The latest light/dark references supply all relocated footer knob drawings, labels, brackets and meter artwork. The stereo output peak meter sits between IN/OUT and HP/LP. It uses original cropped +1/0/-60 dB markings and borders, original blank paper crops to mask the demonstration blocks, and duplicates the supplied L/R block images without generating replacements. Bars fill from -60 to +1 dB using dB mapping aligned to the original -60, 0 and +1 tick positions. The footer L/R letters are removed. Hover shows the dB value at the pointer height in the original speech bubble.

Meter capture is post-DSP/post-OUT/post-bypass. Separate lock-free peak latches retain transients between GUI updates. Bars attack immediately and decay at 30 dB/second at up to 60 FPS, including decay when audio stops. Mono has no fabricated right-channel signal. The readout follows the pointer position on the drawn scale, from -60 to +1 dB. This is an original peak-meter implementation inspired by the requested behaviour, not proprietary meter emulation.

All aggressive recipes now use 22/28/34/40/46% COMP. Soft/hard clip choices are fixed by preset character rather than rerolled on each load, so preset recall is repeatable. Original factory input/output unity and 100% mix are retained.

## Meter peaks, waveform caps and repeat Randomize (0.19)

Both stereo bars use the exact same 17x8 rectangle cropped from the right side of the new MeterReference.png. Dark mode uses the same pixel geometry with its ink color reversed; neither channel uses the earlier irregular left-side block. Meter border and scale artwork remain unchanged; L/R footer labels are removed.

Moving over the meter shows the dB corresponding to that vertical position, rather than current LEFT/RIGHT signal readings. The inverse mapping matches the original -60/0/+1 tick positions; e.g. hovering at -27 dB shows -27dB.

Each channel has an independent original-underscore peak marker. It holds a reached peak for one second; matching/higher incoming peaks restart the hold. After the hold it falls at 15 dB/second until another peak is reached. The line is black in light mode and white in dark mode, and turns red only while its held value exceeds 0 dB. Bars now release at 30 dB/second.

Active bypass tints the original power symbol red, leaving brackets in their theme color. Waveform strokes retain their original theme color: only a small original-stroke cap at the positive/negative 0 dBFS boundary turns red when that signal segment reaches/clips the boundary. Entire waveform columns are not recolored.

Shift-click Randomize to generate repeatedly without dismissing the preset menu. Normal click still randomizes once and closes the menu. The custom menu item handles Shift clicks in place; the menu is not closed and reopened. IN/OUT/global MIX protection and all randomization ranges remain unchanged.

## Updated artwork and auditioning (0.20)

The meter is moved 15 pixels right and 2 pixels down to match the latest full GUI reference, consistently in both themes. Both bars copy the right-hand rectangle from the latest MeterReference image. Every underscore-based indicator uses the newly supplied Underscore.png crop, including SAT, S/H, BEAT/MS, sun/moon and the held meter peaks. Dark mode uses the same shape in white; held over-zero peaks remain red.

Waveform clipping caps are enlarged from three to eight design pixels, preserving original stroke shape and normal-color bodies. Only the power symbol turns red during bypass; its brackets remain neutral. A 30% grey wash covers the main interface to show bypass without blocking controls. The power symbol remains red above that wash.

Shift-click Init or any factory preset to audition it without dismissing the preset menu/category submenu. Shift-click Randomize still generates repeatedly in place. Normal clicks keep their single-action-and-close behaviour. Preset selection indicators are drawn from live current-program state. Direct Enter/Space activation is supported after a custom row has focus.

Bar release is now 30 dB/second and held-peak fall is 15 dB/second (both 25% faster); the one-second peak hold is unchanged. DSP, protected randomization controls, theme persistence and aggressive preset values/ceilings are unchanged.

## New processing and SMAK label (0.21)

Only the original SMAK label crop from the latest reference replaces the former KNC knob lettering, in both themes. The rest of that reference is not applied, preserving the moved meter and current layout. Legacy parameter ID `trans` remains stable. SMAK and COMP each have original numbered 1-5 selectors above their knobs. All new text uses the supplied bitmap alphabet/numbers; all slider parts and underlines use the existing supplied art.

SAT: 1 Tape = soft waveshaping with high-frequency roll-off; 2 Tube = asymmetric shaping with DC removal and even harmonics; 3 Transistor = harder clipping with stronger odd harmonics. Amount 0 is exact bypass.

COMP modes (ratio / attack / release / knee):

| # | Mode | Ratio | Attack | Release | Knee |
|---|---|---|---|---|---|
| 1 | Clean | 2:1 | 15 ms | 120 ms | Soft |
| 2 | Smooth | 3:1 | 30 ms | 300 ms base | Soft, optical-style memory |
| 3 | Punch | 4:1 | 20 ms | 80 ms | Medium |
| 4 | Glue | 4:1 | 10 ms | 150 ms | Medium |
| 5 | Smash | 10:1 | 1 ms | 100 ms | Hard |

COMP amount moves a continuous calibrated internal threshold; 0% is exact compression bypass. Ratio/timing/knee stay mode-specific. Smooth includes a slow program-dependent memory term. No separate attack/release/ratio/threshold/knee/makeup controls are added.

SMAK profiles are Clean 10/150 ms (transparent), Punch 15/200 ms (impact), Snap 3/80 ms (crisp), Body 20/300 ms (sustain/fullness), Smash 5/120 ms (aggressive/saturated). Amount 0 is unity. Timing/gain characteristics morph smoothly during mode changes.

The drawn slider above BIT jointly changes bit depth from 24 to 2 and sample-rate hold period from 1 to 32 samples. Default is 38% (D: 16, SR: 3.7kHz at 48 kHz); zero is transparent. BIT's main knob is wet amount. Hover over the slider shows D (bit depth) and SR (effective sample rate), e.g. D: 16 SR: 3.7kHz. The slider travels with BIT when reordered.

Delay and reverb each have exactly one Ducking slider. The linked stereo detector uses the dry plugin input after IN and before processing. Threshold onset is -24 dBFS, with a gentle 6 dB onset span to avoid threshold chatter. Fixed attack is 10 ms, release 400 ms; maximum attenuation is 24 dB at 100%. Amount 0 produces exact unity gain. Amount automation scales the envelope smoothly, so switching off does not jump gain. Only wet output is attenuated: internal feedback and tail buffers are untouched.

Dry controls trim the dry leg of each effect crossfade. With normalized controls: output = Dry * (1-Mix) * input + Mix * duckedWet. Default Dry 100% preserves the original crossfade. Reverb pre-delay, all-pass diffusion and type-dependent comb networks are preallocated and operate independently of ducking. Chorus now has separate real feedback and delay controls.

Bypass uses a stronger 55% grey main-interface wash. Waveform/meter analysis and display motion pause while plugin or host bypass is active; audio still follows normal bypass. Display updates resume when audio processing is enabled. State schema 10 appends the new controls without replacing existing IDs; older states receive safe defaults (ducking 0%, stereo delay, Dry 100%, pre-delay 0, diffusion 65%, Room, chorus delay 14 ms, Clean modes, bit drive 25%). Factory programs explicitly include the new controls.

## 0.22 preset library and artwork

PRESETS > User lists saved .kncoff files from %APPDATA%/polS Software/KNC OFF/Presets/User. Save always writes into this folder using the chosen filename, even if a different directory is selected in the native dialog. Load can also read external presets. User names recall in project state; Shift-click supports repeated user-preset loading without closing the menu. The User folder is refreshed when opening PRESETS.

Factory order and names stay compatible. All 50 creative recipes now explicitly use the new modes, ducking, dry levels, pre-delay, diffusion, chorus delay/feedback and BIT drive. IN, OUT and global MIX vary per recipe. Init remains neutral. Aggressive COMP stays 22–46%. Conservative visible OUT trims provide headroom; the included peak audit describes tested signals, not a true-peak guarantee for arbitrary input or manual changes.

DLY M/S/PP appears horizontally beside Feedback. All effect slider labels and BIT D:/SR: use the supplied colon crop. BEAT/MS and M/S/PP have no colon. Default BIT drive 38% produces 16 bits and a 13-sample hold: 3.692 kHz (display 3.7kHz) at 48 kHz; the displayed hold rate follows the host sample rate.

## 0.23 delay modes, User browsing and safe Randomize

The DLY Spread row contains vertically stacked MONO / STEREO / PING PONG bitmap buttons with original-art underlines. All eight slider rows retain the same 54-pixel spacing. Mono forces effective Spread 0%, resets its displayed value and disables its slider. Stereo has matched left/right delay times and independent feedback; Spread scales existing side information without creating stereo width from identical mono channels. Ping Pong feeds the initial mono sum into the left delay and cross-feeds successive echoes, actively alternating channels. In Ping Pong, Spread offsets the right delay time. Every factory recipe has revised delay routing/timing/feedback/ducking settings and conservative OUT trim.

As of v1.0.0, previous/next arrows cycle through Init, the factory categories, then the sorted User folder in one shared sequence. The loaded User filename determines its place in that sequence. Saved project state recalls the User name used by navigation. Missing/invalid User files are skipped where possible.

Randomize now includes IN, OUT and global MIX as well as processing and menu controls. IN spans -6 to +6 dB, OUT -9 to 0 dB, MIX 60–100%; bypass, theme and routing are preserved. Randomized states use a final smooth-knee sample ceiling at 0.97 (-0.26 dBFS), after OUT and global MIX, protecting against gain stacking and parameter transitions. The randomized flag recalls this protection in saved presets/session state. Selecting a factory preset clears it. Bypass still passes the original signal; this is sample-peak protection, not a true-peak guarantee.

## v1.0.0 library and visualizer

The visualizer menu contains exactly 1/8, 1/4, 1/2, 1 BAR, 2 BAR (default). Note lengths follow host tempo; bars also follow host meter. Selection recalls in project/preset state. At 120 BPM in 4/4, these display 250, 500, 1000, 2000 and 4000 ms.

PRESETS now has separate Factory and User folders. The plugin materializes Init plus 50 categorized factory files into %APPDATA%/polS Software/KNC OFF/Presets/Factory when opening its editor. Existing files are preserved. Saved user presets stay in the sibling Presets/User folder. The arrows follow one sequence: Init, all factory categories in bank order, then alphabetically sorted User presets, wrapping back to Init. Invalid User files are skipped. Navigation uses the loaded User filename instead of the factory ID embedded in saved state.

Version metadata is 1.0.0; filenames use v1.0.0. Existing parameter IDs, state schema and DSP/preset voicing remain unchanged from 0.23.

The VST3-Only delivery contains only KNC OFF.vst3. The plugin creates its normal Factory library in the application-data location automatically when its editor opens. No separate preset installation is required. The source archive includes reference Factory preset files.
