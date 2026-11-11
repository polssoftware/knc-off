#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Dsp.h"
class KNCProcessor : public juce::AudioProcessor {
public:
 struct WaveRange {float low=0,high=0;};
 struct WaveSlot {std::atomic<float> low{0},high{0};std::atomic<uint64_t> stamp{0};};
 struct WaveRing {static constexpr size_t capacity=524288;std::array<WaveSlot,capacity> samples{};std::atomic<uint64_t> write{0};};
 std::unique_ptr<WaveRing> waveform=std::make_unique<WaveRing>();
 std::atomic<int> waveformRange{4};std::atomic<int> waveformWindowMs{0};std::atomic<double> waveformRate{48000};
 std::vector<WaveRange> waveformSegments(int columns,int windowMs)const;
 void randomizeParameters(juce::Random&);
 std::atomic<bool> analysisPaused{false};
 bool displayPaused()const{return analysisPaused.load(std::memory_order_relaxed)||state.getRawParameterValue("bypass")->load()>.5f;}
 std::atomic<bool> randomizedState{false};
 void chooseTheme(bool dark);
 static juce::File themePreferenceFile();
 static juce::File userPresetDirectory();static juce::File factoryPresetDirectory();void ensureFactoryPresets();
 bool saveUserPreset(const juce::String&);bool loadUserPreset(const juce::File&);bool restoreState(const void*,int);juce::String presetDisplayName();void navigatePreset(int direction);
 juce::CriticalSection userNameLock;juce::String userPresetName;
 std::atomic<bool> darkMode{false};
 KNCProcessor();
 juce::AudioProcessorValueTreeState state;knc::Engine engine;
 std::atomic<uint32_t> mainOrder{knc::mainDefault},auxOrder{knc::auxDefault};std::atomic<int> program{0};
 std::atomic<int> timeNumerator{4},timeDenominator{4};
 int waveformDisplayMs()const;
 std::atomic<float> tempo{120},outputPeak{0};
 std::array<std::atomic<float>,2> meterPeaks{};
    struct HistoryFrame { std::atomic<float> peak{0},rms{0},low{0},high{0};std::atomic<unsigned> stamp{0}; };
    std::array<HistoryFrame,8192> history{};std::atomic<unsigned> historyWrite{0};
    float historyLow=0,historyHigh=0;float historyPeak=0;double historyEnergy=0;int historyCount=0,historyPeriod=480;
std::array<std::atomic<float>,4096> visualSamples{};std::atomic<unsigned> visualWrite{0};
 std::array<float,64> spectrum()const;
 std::array<float,knc::Count> values()const;
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
 void prepareToPlay(double,int)override;void releaseResources()override{}
 bool isBusesLayoutSupported(const BusesLayout&)const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 void run(juce::AudioBuffer<float>&,bool);
 juce::AudioProcessorEditor* createEditor()override;bool hasEditor()const override{return true;}
 const juce::String getName()const override{return "KNC OFF";}
 bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}
 double getTailLengthSeconds()const override;
 int getNumPrograms()override{return 51;}int getCurrentProgram()override{return program.load();}
 void setCurrentProgram(int)override;const juce::String getProgramName(int)override;void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
 juce::AudioProcessorParameter* getBypassParameter()const override{return state.getParameter("bypass");}
};

std::unique_ptr<juce::Component> createEffectSettings(KNCProcessor&,int);

juce::String formatParameterValue(int,float);

std::unique_ptr<juce::PopupMenu::CustomComponent> createRandomizeMenuItem(std::function<void()>);

std::unique_ptr<juce::PopupMenu::CustomComponent> createPresetMenuItem(const juce::String&,std::function<void()>,std::function<bool()>);
