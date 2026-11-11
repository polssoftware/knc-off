#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include <cstdint>

namespace knc {
constexpr float pi = 3.14159265358979323846f;
inline float clamp(float x, float a, float b) { return std::clamp(x,a,b); }
inline float db(float x) { return std::pow(10.0f,x/20.0f); }
enum Param {Trans,Comp,Sat,Sub,Air,Clip,In,Vibe,HP,LP,Mix,Out,Bypass,Shift,Bit,Delay,Reverb,Chorus,Width,Low,Mid,High,SatMode,SubNote,ClipMode,DTime,DFeedback,DSpread,DLow,DHigh,RDecay,RSize,RWidth,RLow,RHigh,CDecay,CDepth,CRate,CSpread,DSync,DBeat,SubDecay,DDuck,DMode,DDry,RPre,RDiff,RType,RDry,RDuck,CDelay,CompMode,TransMode,BitDrive,Count};
inline constexpr std::array<const char*,Count> ids={"trans","comp","sat","sub","air","clip","in","vibe","hp","lp","mix","out","bypass","shift","bit","delay","reverb","chorus","width","low","mid","high","satMode","subNote","clipMode","delayTime","delayFeedback","delaySpread","delayLowCut","delayHighCut","reverbDecay","reverbSize","reverbWidth","reverbLowCut","reverbHighCut","chorusDecay","chorusDepth","chorusRate","chorusSpread","delaySync","delayBeat","subDecay","delayDucking","delayMode","delayDry","reverbPreDelay","reverbDiffusion","reverbType","reverbDry","reverbDucking","chorusDelay","compMode","transMode","bitDrive"};
inline constexpr std::array<float,Count> defaults={0,0,0,0,0,0,0,0,20,20000,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,35,0,80,12000,2,65,100,100,10000,15,50,.7f,70,1,2,20.f,0,1,100,0,65,0,100,0,14,0,0,38};
inline float beatMilliseconds(float bpm,int division){constexpr float beats[]={.25f,.5f,1,2,4,.75f,1.5f,1.f/3,2.f/3};return 60000/clamp(bpm,20,400)*beats[std::clamp(division,0,8)];}
// Independent module states. The rack owns routing; modules never access host parameters.
struct Envelope {
    float value=0;
    float next(float x,float attack,float release) {
        const float c=x>value?attack:release;
        value=x+c*(value-x); return value;
    }
};
struct Filter {
    float s1=0,s2=0;
    float process(float x,float hz,float sr,bool high=false) {
        const float g=std::tan(pi*clamp(hz,5,sr*0.45f)/sr);
        const float a=1/(1+g*(g+1.41421356f));
        const float v1=a*(s1+g*(x-s2));
        const float v2=s2+g*v1;
        s1=2*v1-s1; s2=2*v2-s2;
        return high?x-1.41421356f*v1-v2:v2;
    }
};
struct Transient {
 Envelope fast,slow;
 float gain(float peak,float amount,float sr,float mode=0){constexpr float attack[]={.010f,.015f,.003f,.020f,.005f},release[]={.150f,.200f,.080f,.300f,.120f},impact[]={6,12,9,5,13};float m=clamp(mode,0,4);int i=int(m),j=std::min(4,i+1);float t=m-i,at=attack[i]+t*(attack[j]-attack[i]),rel=release[i]+t*(release[j]-release[i]),hit=impact[i]+t*(impact[j]-impact[i]);float f=fast.next(peak,std::exp(-1/(at*sr)),std::exp(-1/(rel*sr))),base=slow.next(peak,std::exp(-1/(at*4*sr)),std::exp(-1/(rel*1.5f*sr)));float onset=clamp((f-base)/(base+.01f),0,1.5f),body=clamp(1-std::abs(m-3),0,1)*clamp(base/(f+.01f),0,1)*4;return amount<=0?1:db(amount*.01f*(hit*onset+body));}
};
struct Compressor {
 struct Setting{float ratio,attack,release,knee;};
 static Setting setting(int mode){constexpr Setting modes[]={{2,.015f,.120f,6},{3,.030f,.300f,8},{4,.020f,.080f,4},{4,.010f,.150f,4},{10,.001f,.100f,0}};return modes[std::clamp(mode,0,4)];}
 Envelope detector,photo;
 float gain(float peak,float amount,float vibe,float sr,float mode=0){float m=clamp(mode,0,4);int index=int(m),next=std::min(4,index+1);float blend=m-index;auto a=setting(index),b=setting(next);Setting p{a.ratio+(b.ratio-a.ratio)*blend,a.attack+(b.attack-a.attack)*blend,a.release+(b.release-a.release)*blend,a.knee+(b.knee-a.knee)*blend};float e=detector.next(peak,std::exp(-1/(p.attack*sr)),std::exp(-1/(p.release*sr))),memory=photo.next(peak,std::exp(-1/(.030f*sr)),std::exp(-1/(.600f*sr)));float optical=clamp(1-std::abs(m-1),0,1);e+=optical*.15f*(std::max(e,memory*.8f)-e);if(amount<=0)return 1;float intensity=clamp(amount*.01f,0,1),threshold=12-54*std::pow(intensity,.65f)-2*intensity*vibe,over=20*std::log10(std::max(e,1.e-9f))-threshold;float reduction=p.knee<.001f?std::max(0.f,over):over<=-p.knee*.5f?0:over>=p.knee*.5f?over:(over+p.knee*.5f)*(over+p.knee*.5f)/(2*p.knee);return db(-reduction*(1-1/p.ratio)*clamp(intensity/.03f,0,1));}
};
struct Saturator {
 Filter tape;float previousTube=0,dc=0;
 float process(float x,float amount,float mode,float sr=48000){float a=clamp(amount*.01f,0,1),drive=1+9*a,v=x*drive,normal=std::sqrt(drive);float t=tape.process(std::tanh(v)/normal,16000-5000*a,sr),bias=.2f+.15f*a,u=(std::tanh(v+bias)-std::tanh(bias))/normal;dc=u-previousTube+std::exp(-2*pi*5/sr)*dc;previousTube=u;float transistor=clamp(v,-1.f,1.f)/normal,m=clamp(mode,0,2),y=m<=1?t+(dc-t)*m:dc+(transistor-dc)*(m-1);return a==0?x:x+a*(y-x);}
};
struct Ducker {
 float envelope=0,reduction=0;
 float next(float dryPeak,float amount,float sr){float inputDb=20*std::log10(std::max(dryPeak,1.e-9f)),target=clamp((inputDb+24)/6,0,1),coefficient=std::exp(-1/(sr*(target>envelope?.010f:.400f)));envelope=target+coefficient*(envelope-target);reduction=24*clamp(amount*.01f,0,1)*envelope;return amount<=0?1:db(-reduction);}
};
inline float subDecaySeconds(float amount){return .02f*std::pow(100.f,clamp(amount,0,100)*.01f);}
struct SubGenerator {
 Filter filter;Envelope fast,slow;float phase=0,amp=0,sm=0;int hold=0;
 float next(float x,float amount,float note,float sr,float decayAmount=20.f){
  float energy=std::max(std::abs(x)*.65f,std::abs(filter.process(x,180,sr)));
  float f=fast.next(energy,std::exp(-1/(.001f*sr)),std::exp(-1/(.035f*sr)));
  float s=slow.next(energy,std::exp(-1/(.04f*sr)),std::exp(-1/(.14f*sr)));
  if(hold>0)--hold;
  if(f>.0025f&&f>s*1.35f&&hold==0){amp=std::max(amp,clamp(std::sqrt(f)*1.6f,.12f,.85f));hold=int(sr*.09f);}
  phase+=2*pi*(32.7031957f*std::pow(2.f,note/12))/sr;if(phase>=2*pi)phase-=2*pi;
  amp*=std::exp(-1/(subDecaySeconds(decayAmount)*sr));float y=std::sin(phase)*amp*amount*.01f;sm+=(y-sm)*(1-std::exp(-1/(.002f*sr)));return sm;
 }
};
struct AirEnhancer {
    Filter low;
    float process(float x,float amount,float sr) {
        const float high=x-low.process(x,6500,sr);
        return x+high*amount*0.01f*1.5f;
    }
};
struct Clipper {
    static float process(float x,float amount,float vibe) {
        const float a=amount*0.01f,drive=db(18*a);
        const float v=x*drive;
        const float hard=clamp(v,-0.98f,0.98f);
        const float soft=0.98f*std::tanh(v/0.98f);
        return x+a*((hard+(soft-hard)*vibe)-x);
    }
};
struct Smooth {
    float value=0,target=0,step=0; int remaining=0;
    void reset(float x) {value=target=x;remaining=0;step=0;}
    void set(float x,int samples) {if(x!=target) {target=x;remaining=samples;step=(x-value)/float(samples);}}
    float next() {if(remaining>0) {value+=step;if(--remaining==0)value=target;}return value;}
};
struct Ring {
 std::vector<float> data;int pos=0;
 void prepare(int n){data.assign(n,0);pos=0;}
 float read(float delay)const{double at=double(pos)-clamp(delay,1,float(data.size()-2));while(at<0)at+=double(data.size());int i=int(at)%int(data.size()),j=(i+1)%int(data.size());return data[i]+(data[j]-data[i])*float(at-std::floor(at));}
 void write(float x){data[pos]=x;if(++pos==int(data.size()))pos=0;}
};
struct Pitch {
 std::array<Ring,2> ring;float phase=0;
 void prepare(float sr){for(auto& r:ring)r.prepare(int(sr*.12f)+8);phase=0;}
 void process(std::array<float,2>& x,int ch,float semitone,float sr){float window=sr*.045f;phase-=(std::pow(2.f,semitone/12)-1)/window;phase-=std::floor(phase);float p2=phase+.5f;p2-=std::floor(p2);float w=.5f-.5f*std::cos(2*pi*phase),wet=clamp(std::abs(semitone)*4,0,1);for(int c=0;c<ch;++c){auto& r=ring[c];float y=r.read(32+phase*window)*w+r.read(32+p2*window)*(1-w);r.write(x[c]);x[c]+=wet*(y-x[c]);}}
};
inline int crusherBits(float drive){return std::clamp(int(std::round(24-22*drive*.01f)),2,24);}
inline int crusherPeriod(float drive){return 1+std::clamp(int(std::round(31*drive*.01f)),0,31);}
struct Crusher {std::array<float,2> held{};int counter=0;void process(std::array<float,2>& x,int ch,float amount,float drive=25){if(drive<=0||amount<=0){counter=0;held=x;return;}int steps=1<<(crusherBits(drive)-1),period=crusherPeriod(drive);if(counter<=0){for(int c=0;c<ch;++c)held[c]=std::round(x[c]*steps)/steps;counter=period;}--counter;for(int c=0;c<ch;++c)x[c]+=amount*.01f*(held[c]-x[c]);}};
struct DelayFx {
 std::array<Ring,2> ring;std::array<Filter,2> hp,lp;Ducker duck;
 void prepare(float sr){for(auto& r:ring)r.prepare(int(sr*16)+8);hp={};lp={};duck={};}
 void process(std::array<float,2>& x,int ch,const std::array<float,Count>& p,float sr,float dryKey=-1){
  std::array<float,2> y{},input=x;float mode=clamp(p[DMode],0,2),monoWeight=clamp(1-mode,0,1),pingWeight=clamp(mode-1,0,1),stereoWeight=1-monoWeight-pingWeight;
  float spread=p[DSpread]*.01f*(1-monoWeight),mono=(x[0]+(ch==2?x[1]:x[0]))*.5f;
  if(dryKey<0)dryKey=std::max(std::abs(x[0]),ch==2?std::abs(x[1]):0.f);float gain=duck.next(dryKey,p[DDuck],sr);
  // Stereo uses matched delay times and independent feedback: identical channels stay identical.
  // Spread scales existing stereo side information; only ping-pong offsets time/cross-feeds.
  for(int c=0;c<ch;++c){float time=p[DTime]*.001f*(1+(c==1?.3f*spread*pingWeight:0));y[c]=lp[c].process(hp[c].process(ring[c].read(time*sr),p[DLow],sr,true),p[DHigh],sr);}
  float mid=(y[0]+(ch==2?y[1]:y[0]))*.5f;
  for(int c=0;c<ch;++c){float fb=(1-pingWeight)*y[c]+pingWeight*y[ch==2?1-c:0];float feed=monoWeight*mono+stereoWeight*input[c]+pingWeight*(c==0?mono:0);ring[c].write(clamp(feed+fb*p[DFeedback]*.01f,-16,16));float stereoWet=mid+(y[c]-mid)*(1+spread);float wet=monoWeight*mid+stereoWeight*stereoWet+pingWeight*y[c];x[c]=input[c]*(1-p[Delay]*.01f)*p[DDry]*.01f+wet*p[Delay]*.01f*gain;}
 }
};
struct ReverbFx {
 std::array<std::array<Ring,4>,2> comb;std::array<std::array<Ring,6>,2> diffusion;std::array<Ring,2> predelay;std::array<Filter,2> hp,lp;Ducker duck;
 void prepare(float sr){for(auto& bank:comb)for(auto& r:bank)r.prepare(int(sr*.30f)+8);for(auto& bank:diffusion)for(auto& r:bank)r.prepare(int(sr*.04f)+8);for(auto& r:predelay)r.prepare(int(sr*.30f)+8);hp={};lp={};duck={};}
 void process(std::array<float,2>& x,int ch,const std::array<float,Count>& p,float sr,float dryKey=-1){constexpr float times[3][4]={{.0297f,.0371f,.0411f,.0437f},{.0431f,.0557f,.0673f,.0761f},{.0137f,.0199f,.0277f,.0311f}},allpass[]={.0035f,.0051f,.0071f,.0117f,.0083f,.0029f};std::array<float,2> y{},input=x;float mono=(x[0]+(ch==2?x[1]:x[0]))*.5f,type=clamp(p[RType],0,2);int lower=int(type),upper=std::min(2,lower+1);float blend=type-lower;if(dryKey<0)dryKey=std::max(std::abs(x[0]),ch==2?std::abs(x[1]):0.f);float gain=duck.next(dryKey,p[RDuck],sr);
 for(int c=0;c<2;++c){float preSamples=p[RPre]*.001f*sr,delayedInput=predelay[c].read(std::max(1.f,preSamples)),feed=preSamples<1?mono+preSamples*(delayedInput-mono):delayedInput;predelay[c].write(mono);float diffused=feed;for(int k=0;k<6;++k){auto& ring=diffusion[c][k];float delayed=ring.read(allpass[k]*(.7f+.6f*p[RSize]*.01f)*sr),out=delayed-.65f*diffused;ring.write(diffused+.65f*out);float stage=clamp((2+2*type-k),0,1);diffused+=stage*(out-diffused);}feed+=p[RDiff]*.01f*(diffused-feed);float sum=0;for(int k=0;k<4;++k){float seconds=(times[lower][k]+blend*(times[upper][k]-times[lower][k]))*(.5f+p[RSize]*.01f)+(c?.0017f:0);auto& r=comb[c][k];float v=r.read(seconds*sr),feedback=std::pow(.001f,seconds/p[RDecay]);r.write(feed*.22f+v*feedback);sum+=v*.25f;}y[c]=lp[c].process(hp[c].process(sum,p[RLow],sr,true),p[RHigh],sr);}float mid=(y[0]+y[1])*.5f,side=(y[0]-y[1])*.5f*p[RWidth]*.01f;for(int c=0;c<ch;++c){float wet=ch==1?mid:mid+(c?-side:side);x[c]=input[c]*(1-p[Reverb]*.01f)*p[RDry]*.01f+wet*p[Reverb]*.01f*gain;}}
};
struct ChorusFx {
 std::array<Ring,2> ring;float phase=0;
 void prepare(float sr){for(auto& r:ring)r.prepare(int(sr*.12f)+8);phase=0;}
 void process(std::array<float,2>& x,int ch,const std::array<float,Count>& p,float sr){phase+=2*pi*p[CRate]/sr;if(phase>=2*pi)phase-=2*pi;for(int c=0;c<ch;++c){float base=p[CDelay]*.001f*sr,lfo=std::sin(phase+c*pi*p[CSpread]*.01f),delay=base+lfo*std::min(base*.85f,sr*.009f)*p[CDepth]*.01f;float y=ring[c].read(delay);ring[c].write(x[c]+y*p[CDecay]*.01f);float wet=(x[c]+y)*.70710678f;x[c]+=p[Chorus]*.01f*(wet-x[c]);}}
};
// Final processed-path ceiling follows CLIP amount/mode, before global MIX/OUT.
inline float clipCeiling(float x,float mode){constexpr float ceiling=.97f,knee=.82f;float magnitude=std::abs(x);if(magnitude<=knee)return x;float soft=std::copysign(knee+(ceiling-knee)*std::tanh((magnitude-knee)/(ceiling-knee)),x),hard=clamp(x,-ceiling,ceiling);return soft+(hard-soft)*clamp(mode,0.f,1.f);}
struct Eq {Filter low,midLow,midHigh,high;float process(float x,float lo,float mid,float hi,float sr){float l=low.process(x,180,sr),m=midLow.process(midHigh.process(x,300,sr,true),3000,sr),h=high.process(x,6000,sr,true);return x+l*(db(lo)-1)+m*(db(mid)-1)+h*(db(hi)-1);}};
inline bool validOrder(uint32_t code,int count){unsigned used=0;for(int i=0;i<count;++i){int v=(code>>(i*4))&15;if(v>=count||(used&(1u<<v)))return false;used|=1u<<v;}return true;}
inline constexpr uint32_t mainDefault=0x76543210u,auxDefault=0x3210u;
enum class ModuleId {Transient,Compressor,Saturation,Sub,EQ,Spectral,Stereo,Space,Shift,Texture,Loudness,Clipper};
enum class MacroBank {Bass,Drums,Kick,Synths,Vocals};
struct Engine {
 bool protectOutput=false;float sr=48000,tempo=120;Smooth delayMs;std::array<Smooth,Count> smooth;
 Transient transient;Compressor compressor;SubGenerator sub;std::array<Saturator,2> saturator;std::array<AirEnhancer,2> air;std::array<Filter,2> hp,lp;std::array<Eq,2> eq;
 Pitch pitch;Crusher crusher;DelayFx delay;ReverbFx reverb;ChorusFx chorus;Smooth routing;
 uint32_t main=mainDefault,aux=auxDefault,pendingMain=mainDefault,pendingAux=auxDefault;
 void prepare(float rate,const std::array<float,Count>& values=defaults){sr=rate;transient={};compressor={};sub={};saturator={};air={};hp={};lp={};eq={};crusher={};pitch.prepare(sr);delay.prepare(sr);reverb.prepare(sr);chorus.prepare(sr);main=pendingMain=mainDefault;aux=pendingAux=auxDefault;routing.reset(1);for(int i=0;i<Count;++i)smooth[i].reset(values[i]);delayMs.reset(values[DSync]>.5f?beatMilliseconds(tempo,int(values[DBeat])):values[DTime]);}
 void targets(const std::array<float,Count>& v){for(int i=0;i<Count;++i)smooth[i].set(v[i],std::max(1,int(sr*.02f)));delayMs.set(v[DSync]>.5f?beatMilliseconds(tempo,int(v[DBeat])):v[DTime],std::max(1,int(sr*.02f)));}
 void order(uint32_t m,uint32_t a){if(!validOrder(m,8)||!validOrder(a,4))return;if(m!=pendingMain||a!=pendingAux){pendingMain=m;pendingAux=a;routing.set(0,std::max(1,int(sr*.008f)));}}
 void process(float* const* data,int ch,int samples,bool hostBypass=false){for(int n=0;n<samples;++n){
  std::array<float,Count> p;for(int i=0;i<Count;++i)p[i]=smooth[i].next();p[DTime]=delayMs.next();float gate=routing.next();if(gate==0&&routing.remaining==0){main=pendingMain;aux=pendingAux;routing.set(1,std::max(1,int(sr*.008f)));}
  std::array<float,2> dry{},x{};for(int c=0;c<ch;++c){dry[c]=std::isfinite(data[c][n])?data[c][n]:0;x[c]=dry[c]*db(p[In]);}
  float dryKey=0;for(int c=0;c<ch;++c)dryKey=std::max(dryKey,std::abs(x[c]));
  for(int slot=0;slot<8;++slot){int mod=(main>>(slot*4))&15;float peak=0,mono=0;for(int c=0;c<ch;++c){peak=std::max(peak,std::abs(x[c]));mono+=x[c]/ch;}
   if(mod==0){float gain=transient.gain(peak,p[Trans],sr,p[TransMode]);for(int c=0;c<ch;++c){x[c]*=gain;float smash=clamp(p[TransMode]-3,0,1)*p[Trans]*.01f;x[c]+=smash*(std::tanh(x[c]*1.8f)/1.3f-x[c]);}}
   if(mod==1)for(int c=0;c<ch;++c)x[c]=saturator[c].process(x[c],p[Sat],p[SatMode],sr);
   if(mod==2){float bass=sub.next(mono,p[Sub],p[SubNote],sr,p[SubDecay]);for(int c=0;c<ch;++c)x[c]+=bass;}
   if(mod==3)for(int c=0;c<ch;++c)x[c]=air[c].process(x[c],p[Air],sr);
   if(mod==4)for(int c=0;c<ch;++c)x[c]=Clipper::process(x[c],p[Clip],1-p[ClipMode]);
   if(mod==5){float gain=compressor.gain(peak,p[Comp],p[Vibe]*.01f,sr,p[CompMode]);for(int c=0;c<ch;++c)x[c]*=gain;}
   if(mod==6)pitch.process(x,ch,p[Shift],sr);if(mod==7)crusher.process(x,ch,p[Bit],p[BitDrive]);
  }
  for(int c=0;c<ch;++c){float h=hp[c].process(x[c],p[HP],sr,true);x[c]+=clamp((p[HP]-20)/10,0,1)*(h-x[c]);float l=lp[c].process(x[c],p[LP],sr);x[c]+=clamp((20000-p[LP])/1000,0,1)*(l-x[c]);}
  for(int slot=0;slot<4;++slot){int mod=(aux>>(slot*4))&15;if(mod==0)delay.process(x,ch,p,sr,dryKey);if(mod==1)reverb.process(x,ch,p,sr,dryKey);if(mod==2)chorus.process(x,ch,p,sr);if(mod==3&&ch==2){float mid=(x[0]+x[1])*.5f,side=(x[0]-x[1])*.5f*(1+p[Width]*.01f);x[0]=mid+side;x[1]=mid-side;}}
  for(int c=0;c<ch;++c){x[c]=eq[c].process(x[c],p[Low],p[Mid],p[High],sr);x[c]+=p[Clip]*.01f*(clipCeiling(x[c],p[ClipMode])-x[c]);float wet=(dry[c]+p[Mix]*.01f*(x[c]-dry[c]))*db(p[Out]);wet=dry[c]+gate*(wet-dry[c]);data[c][n]=wet+(dry[c]-wet)*(hostBypass?1:p[Bypass]);if(protectOutput&&!hostBypass&&p[Bypass]<.5f)data[c][n]=clipCeiling(data[c][n],0);}
 }}
};
}
