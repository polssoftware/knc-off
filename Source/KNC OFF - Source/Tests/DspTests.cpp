#include "../Source/Dsp.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using Audio=std::array<std::vector<float>,2>;
void check(bool b,const char* m){if(!b)throw std::runtime_error(m);}
Audio signal(int n,float sr){Audio a;for(auto& c:a)c.resize(n);for(int i=0;i<n;++i){float t=i/sr,hit=std::exp(-std::fmod(t,.25f)*25);a[0][i]=hit*(.4f*std::sin(2*knc::pi*90*t)+.15f*std::sin(2*knc::pi*1800*t)+.1f*std::sin(2*knc::pi*8500*t));a[1][i]=a[0][i]*.7f;}return a;}
void run(knc::Engine& e,Audio& a,int channels=2,int block=257){for(int i=0;i<int(a[0].size());i+=block){float* p[]={a[0].data()+i,a[1].data()+i};e.process(p,channels,std::min(block,int(a[0].size())-i));}}
double diff(const Audio& a,const Audio& b){double d=0;for(int c=0;c<2;++c)for(size_t n=0;n<a[c].size();++n)d+=std::abs(a[c][n]-b[c][n]);return d/a[0].size();}
int main(){try{for(float mode:{0.f,1.f})for(float x:{-20.f,-1.f,-.5f,0.f,.5f,1.f,20.f})check(std::abs(knc::clipCeiling(x,mode))<=.970001f,"Clip safety ceiling exceeded");
 for(float sr:{44100.f,48000.f,96000.f,192000.f})for(int channels:{1,2}){auto dry=signal(int(sr),sr),wet=dry;knc::Engine e;e.prepare(sr);run(e,wet,channels);check(diff(dry,wet)<1e-6,"Init must be neutral");
  for(int id:{knc::Trans,knc::Comp,knc::Sat,knc::Sub,knc::Air,knc::Clip,knc::Bit,knc::Shift,knc::Delay,knc::Reverb,knc::Chorus,knc::HP,knc::LP,knc::Low,knc::Mid,knc::High}){auto p=knc::defaults;p[id]=id==knc::Shift?12:id==knc::HP?300:id==knc::LP?3000:id>=knc::Low&&id<=knc::High?6:75;e.prepare(sr,p);wet=dry;run(e,wet,channels);check(diff(dry,wet)>1e-5,"Effect control must change audio");for(auto& c:wet)for(float x:c)check(std::isfinite(x),"Nonfinite output");}
  auto p=knc::defaults;p[knc::Sat]=100;p[knc::Sub]=100;p[knc::Comp]=100;p[knc::Bit]=100;p[knc::In]=12;p[knc::Out]=12;p[knc::Bypass]=1;e.prepare(sr,p);wet=dry;run(e,wet,channels);check(diff(dry,wet)<1e-5,"Bypass must null dry");p[knc::Bypass]=0;p[knc::Mix]=0;p[knc::Out]=0;e.prepare(sr,p);wet=dry;run(e,wet,channels);check(diff(dry,wet)<1e-6,"Mix 0 must null dry");
  Audio silence;for(auto& c:silence)c.resize(int(sr));e.prepare(sr,p);run(e,silence,channels);for(auto& c:silence)for(float x:c)check(x==0,"No input must not generate sub");
 }
 constexpr float sr=48000;knc::Engine e;
 // A quiet 1 kHz hit contains no deliberate sub: verify a generated sine tail and octave tuning.
 for(float note:{0.f,12.f}){knc::SubGenerator sub;std::vector<float> tail(48000);for(int i=0;i<48000;++i){float hit=i<1440?.03f*std::sin(2*knc::pi*1000*i/sr):0;tail[i]=sub.next(hit,100,note,sr,57.3f);}double power=0;int crossings=0;for(int i=4801;i<24000;++i){power+=tail[i]*tail[i];if(tail[i-1]<0&&tail[i]>=0)++crossings;}check(power>1,"SUB missing on quiet midrange hit");float frequency=crossings/(19200.f/sr);check(std::abs(frequency-(note==0?32.703f:65.406f))<4,"Generated SUB frequency incorrect");}
 // Verify actual echo onset in both beat and millisecond modes.
 for(int mode=0;mode<3;++mode){auto p=knc::defaults;p[knc::Delay]=100;p[knc::DFeedback]=0;p[knc::DLow]=20;p[knc::DHigh]=20000;p[knc::DSync]=mode==0?0:1;p[knc::DTime]=100;e.tempo=mode==2?90:120;e.prepare(sr,p);Audio impulse;for(auto& c:impulse)c.resize(60000);impulse[0][0]=impulse[1][0]=1;run(e,impulse);int first=-1;for(int i=1;i<60000;++i)if(std::abs(impulse[0][i])>.05f){first=i;break;}int expected=mode==0?4800:mode==1?24000:32000;if(std::abs(first-expected)>=8)std::cerr<<"mode="<<mode<<" first="<<first<<" expected="<<expected<<" ms="<<e.delayMs.value<<"\n";check(std::abs(first-expected)<8,"Delay beat/ms timing incorrect");}
 auto dry=signal(96000,sr);auto p=knc::defaults;p[knc::Sat]=80;p[knc::Comp]=70;p[knc::Clip]=50;auto a=dry,b=dry;e.prepare(sr,p);run(e,a);e.prepare(sr,p);e.order(0x76543201,knc::auxDefault);run(e,b);check(diff(a,b)>1e-5,"Main order must affect sound");
 p=knc::defaults;p[knc::Delay]=60;p[knc::Chorus]=70;a=dry;b=dry;e.prepare(sr,p);run(e,a);e.prepare(sr,p);e.order(knc::mainDefault,0x3012);run(e,b);check(diff(a,b)>1e-5,"Aux order must affect sound");
 // Block segmentation and smoothing.
 p=knc::defaults;p[knc::Sat]=80;p[knc::Sub]=50;a=dry;b=dry;e.prepare(sr,p);run(e,a,2,1);e.prepare(sr,p);run(e,b,2,1024);check(diff(a,b)<1e-7,"Block segmentation changed DSP");
 knc::Smooth sm;sm.reset(0);sm.set(1,960);float prev=0;for(int i=0;i<960;++i){float x=sm.next();check(x>=prev&&x-prev<.0011f,"Smoothing discontinuity");prev=x;}check(sm.value==1,"Smoothing target not reached");
 check(knc::defaults[knc::DSync]==1&&knc::defaults[knc::DBeat]==2&&knc::defaults[knc::DSpread]==0,"Delay defaults incorrect");
 // Width endpoints: mono, original stereo, expanded side signal.
 for(float width:{-100.f,0.f,100.f}){auto values=knc::defaults;values[knc::Width]=width;knc::Engine widthEngine;widthEngine.prepare(sr,values);float left=.6f,right=.2f;float* frame[]={&left,&right};widthEngine.process(frame,2,1);float side=(.6f-.2f)*.5f*(1+width*.01f);check(std::abs(left-(.4f+side))<1e-6&&std::abs(right-(.4f-side))<1e-6,"Width endpoint mapping failed");}
 for(float gain:{-24.f,24.f}){auto values=knc::defaults;values[knc::In]=gain;knc::Engine gainEngine;gainEngine.prepare(sr,values);float left=.02f,right=.01f;float* frame[]={&left,&right};gainEngine.process(frame,2,1);check(std::abs(left-.02f*knc::db(gain))<1e-6,"24 dB gain range DSP failed");}
 // Requested expanded extremes remain finite, including unity delay feedback and long reverb.
 {auto values=knc::defaults;values[knc::Delay]=100;values[knc::DFeedback]=100;values[knc::DSync]=0;values[knc::DTime]=10;values[knc::Reverb]=50;values[knc::RDecay]=20;knc::Engine stress;stress.prepare(sr,values);auto audio=signal(480000,sr);run(stress,audio);for(auto& c:audio)for(float x:c)check(std::isfinite(x)&&std::abs(x)<100,"Unity feedback/long reverb must stay bounded");}
 for(int eqId:{knc::Low,knc::Mid,knc::High})for(float gain:{-24.f,24.f}){auto values=knc::defaults;values[eqId]=gain;knc::Engine eqEngine;eqEngine.prepare(sr,values);auto audio=signal(48000,sr),reference=audio;run(eqEngine,audio);check(diff(audio,reference)>1e-4,"Expanded EQ range must change audio");for(auto& c:audio)for(float x:c)check(std::isfinite(x),"Expanded EQ range unstable");}
 {double powers[2]={};for(int setting=0;setting<2;++setting){knc::SubGenerator generator;for(int n=0;n<48000;++n){float hit=n<1440?.06f*std::sin(2*knc::pi*1000*n/sr):0;float output=generator.next(hit,100,0,sr,setting?100:0);if(n>12000)powers[setting]+=output*output;}}check(powers[1]>powers[0]*100&&powers[1]>.01,"Sub decay must change generated tail length");}
 std::cout<<"PASS: mono/stereo 44.1-192k, neutral/mix/bypass, effects, quiet-hit SUB frequency/energy, beat/ms delay timing, routing, silence, finite output, block independence, smoothing\n";
}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}return 0;}
