/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#include <algorithm>
#include <iostream>
#include <cstring>

#include "ym2612.h"



inline void WordOp_Set(unsigned char *ptr,short value)
{
	if(value<-32767)
	{
		*((short *)ptr)=-32767;
	}
	else if(32767<value)
	{
		*((short *)ptr)=32767;
	}
	else
	{
		*((short *)ptr)=value;
	}

#ifndef YS_LITTLE_ENDIAN
	std::swap(ptr[0],ptr[1]);
#endif
}



// #define YM2612_DEBUGOUTPUT


/*static*/ int YM2612::MULTITable[16]= // Multiple x2.
{
	1,
	2,
	4,
	6,
	8,
	10,
	12,
	14,
	16,
	18,
	20,
	22,
	24,
	26,
	28,
	30,
};

static unsigned int attackTime0to96dB[64]= // 1/100ms
{
897024,//0,
897024,//0,
897024,//0,
897024,//0,
897024,
897024,
598016,
598016, // 7: 897024*2/3
448512, // 8: 897024/2           x=8->897024/(2^1)
358810, // 9: 897024*6/15
299008, //10: 897024/3
258293,
224256, //12: 897024/4           x=12->897024/(2^2)
173405,                 // <- 179405?
149504, //14: 897024/6
128146,

112128, //16: 897024/8           x=16->897024/(2^3)  -> 897024/(2^((x-4)/4))
83702,
74752,  // 897024/12
64073,
56064,  // 897024/16
44851,
37376,  // 897024/24
32037,
28032,  // 897024/32
22425,
18688,  // 897024/48
16018,
14016,
11213,
9344,
8009,

7008,
5806,
4872,
4005,
3504,
2903,
2335,
2002,
1752,
1402,
1168,
1001,
876,
701,
584,
501,

438,
350,
232,
250,
240,
192,
160,
137,
126,
101,
84,
72,
60,
60,
0,
0,
};
static unsigned int sustainDecayReleaseTime0to96dB[64]= // 1/100ms
{
12398592,//0,
12398592,//0,
12398592,//0,
12398592,//0,
12398592,
12398592,
8265726,
8265728,
5199296,
4953437,
4132864,
3542455,
3099648,
2479719,
2066432,
1771227,

1549824,
1239859,
1033215,
885614,
774912,
619930,
516608,
442807,
387455,
309965,
258304,
221403,
193728,
154982,
129152,
110702,

96864,
77491,
64576,
55351,
48432,
38745,
32298,
27675,
24216,
19373,
16144,
13938,
12108,
9686,
8072,
6919,

6054,
4843,
4056,
3459,
3027,
2422,
2018,
1730,
1514,
1211,
1009,
865,
757,
757,
757,
757,
};
static unsigned int attackTime10to90Percent[64]=
{
0,
0,
0,
0,
0,
468891,
333005,
333005,
249446,
200294,
166502,
141926,
124723,
100147,
83251,
70963,

62362,
50074,
41625,
35482,
31181,
25037,
20813,
17741,
15590,
12518,
10406,
8670,
7795,
6259,
5203,
4435,

3899,
3130,
2602,
2218,
1949,
1565,
1301,
1109,
974,
782,
650,
554,
497,
391,
325,
277,

241,
194,
165,
140,
125,
99,
92,
70,
61,
46,
37,
32,
26,
26,
0,
0,
};
static unsigned int sustainDecayReleaseTime10to90Percent[64]=
{
0,
0,
0,
0,
0,
2491484, // 5:
1658880, // 6:
1658880, // 7:
1247232, // 8: 2491484/2
998400,  // 9: 1996800/2
823440,  //10: 
712704,  //11:
623616,  //12: 2491484/4
498200,  //13: 1996800/4  499200?
414720,  //14: 
356352,  //15: 

311808,  //16: 2491484/8
249600,
207360,
178175,
155904,
124800,
103680,
89088,
77952,
62400,
51840,
44544,
38876,
31200,
25920,
22272,

19488,
15800,
12960,
11136,
9744,
7800,
6480,
5568,
4872,
3900,
3240,
2784,
2436,
1950,
1620,
1392,

1218,
975,
810,
696,
509,
480,
407,
350,
305,
243,
203,
173,
152,
152,
152,
152,
};

const struct YM2612::ConnectionToOutputSlot YM2612::connectionToOutputSlots[8]=
{
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{1,{3,-1,-1,-1}},
	{2,{1,3,-1,-1}},
	{3,{1,2,3,-1}},
	{3,{1,2,3,-1}},
	{4,{0,1,2,3}},
};

static unsigned int detune1000Table[]=
{
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,   0,  48,  95,
	   0,  48,  95,  95,
	   0,  48,  95, 143,
	   0,  48,  95, 143,
	   0,  48,  95, 143,
	   0,  48,  95, 191,
	   0,  48, 143, 191,

	   0,  48, 143, 191,
	   0,  48, 143, 238,
	   0,  95, 191, 238,
	   0,  95, 191, 286,
	   0,  95, 191, 288,
	   0,  95, 238, 334,
	   0,  85, 238, 381, // 85?
	   0, 143, 286, 381,
	   0, 143, 298, 429,
	   0, 143, 334, 477,

	   0, 191, 381, 525,
	   0, 191, 381, 572,
	   0, 191, 429, 620,
	   0, 238, 477, 668,
	   0, 238, 525, 763,
	   0, 286, 672, 811,
	   0, 286, 520, 906,
	   0, 334, 668, 354,
	   0, 391, 763,1049,
	   0, 391, 763,1049,

	   0, 391, 763,1049,
	   0, 391, 763,1049,
};

////////////////////////////////////////////////////////////

/*static*/ unsigned int YM2612::attackExp[4096];         // 0 to 4095 scale in -> 0 to 4095 scale out.
/*static*/ unsigned int YM2612::attackExpInverse[4096];  // 0 to 4095 scale in -> 0 to 4095 scale out.

////////////////////////////////////////////////////////////



static unsigned int LFOCycleMicroSec[8]=
{
	100000000/ 398, // 1000000/ 3.98,
	100000000/ 556, // 1000000/ 5.56,
	100000000/ 602, // 1000000/ 6.02,
	100000000/ 637, // 1000000/ 6.37,
	100000000/ 688, // 1000000/ 6.88,
	100000000/ 963, // 1000000/ 9.63,
	100000000/4810, // 1000000/48.10,
	100000000/7220  // 1000000/72.20
};

// Definition of cent.
// https://ja.wikipedia.org/wiki/%E3%82%BB%E3%83%B3%E3%83%88_(%E9%9F%B3%E6%A5%BD)
static int PMS16384Table[8]=
{
  0,
 32,// scale(3.400000)=1.001966, (1.001966-1.0)*16384.0=32
 63,// scale(6.700000)=1.003878, (1.003878-1.0)*16384.0=63
 94,// scale(10.000000)=1.005793, (1.005793-1.0)*16384.0=94
133,// scale(14.000000)=1.008120, (1.008120-1.0)*16384.0=133
190,// scale(20.000000)=1.011619, (1.011619-1.0)*16384.0=190
382,// scale(40.000000)=1.023374, (1.023374-1.0)*16384.0=382
774,// scale(80.000000)=1.047294, (1.047294-1.0)*16384.0=774
};

// dB=20log10(P/Pref)
// 0=20log10(P/Pref) -> P/Pref=1.0
// 1.4=20log10(P/Pref) -> 10^(1.4/20)=P/Pref -> 
// 5.9=20log10(P/Pref) -> 10^(5.9/20)=P/Pref ->
// 11.8=20log10(P/Pref) -> 10^(11.8/20)=P/Pref ->
static int AMS4096Table[4]=
{
 4096-4096,  // 1.0
 4812-4096,  // 1.1749
 8079-4096,  // 1.97242
15935-4096, // 3.89045
};

////////////////////////////////////////////////////////////

int YM2612::Slot::DetuneContributionToPhaseStepS12(unsigned int BLOCK,unsigned int NOTE) const
{
	// Hz ranges 1 to roughly 8000.  PHASE_STEPS=4096.  hertz*PHASE_STEPS=2^13*2^12=2^25. Fits in 32 bit.
	long long int detuneStepContribution=0;
	if(0!=DT)
	{
		long long int hertz1000=detune1000Table[(BLOCK<<4)+(NOTE<<2)+(DT&3)];
		detuneStepContribution=(hertz1000*PHASE_STEPS);
		detuneStepContribution<<=12;
		detuneStepContribution/=1000;  // Now it is (hertz*PHASE_STEPS)<<12.
		if(0!=(DT&4))
		{
			detuneStepContribution=-detuneStepContribution;
		}
	}
	return (int)detuneStepContribution;
}

inline int YM2612::Slot::UnscaledOutput(int phase,int phaseShift) const
{
	// phaseShift is input from the upstream slot.
	// -4096 to 4096.  4096 should be counted as 8pi.
	// UNSCALED_MAX is 2048. therefore 8x.

	//phaseShift*=MULTITable[this->MULTI];
	//phaseShift>>=1;

	//                     8.0       * (2PI / 2)     /   1.0
	const int outputScale=SLOTOUT_TO_NPI*(PHASE_STEPS/2)/UNSCALED_MAX;
	return sineTable[(phase+(phaseShift*outputScale))&PHASE_MASK];
}
inline int YM2612::Slot::UnscaledOutput(int phase,int phaseShift,unsigned int FB,int lastSlot0Out) const
{
	if(0!=FB)
	{
		static const int FBScaleTable[]=
		{
			0,1,2,4,8,16,32,64
		};
		// lastSlotOut=1.0=>UNSCALED_MAX   4096=>2PI
		// To make it 4PI(8192) at FB=7(scale=64), must divide by UNSCALED_MAX*64/8192.
		// What was I thinking when I made div=32?  It should be 16.
		const int div=UNSCALED_MAX*64/8192;
		phase+=(lastSlot0Out*FBScaleTable[FB]/div);
	}
	//                     8.0       * (2PI / 2)     /   1.0
	const int outputScale=SLOTOUT_TO_NPI*(PHASE_STEPS/2)/UNSCALED_MAX;
	return sineTable[(phase+(phaseShift*outputScale))&PHASE_MASK];
}
inline int YM2612::Slot::EnvelopedOutputDb(int phase,int phaseShift,unsigned int timeInMS,unsigned int FB,int lastSlot0Out) const
{
	int dB=InterpolateEnvelope(timeInMS);
	lastDb100Cache=dB;
	int ampl=DB100to4095Scale[dB];
	int unscaledOut=UnscaledOutput(phase,phaseShift,FB,lastSlot0Out);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::EnvelopedOutputDb(int phase,int phaseShift,unsigned int timeInMS) const
{
	int dB=InterpolateEnvelope(timeInMS);
	lastDb100Cache=dB;
	int ampl=DB100to4095Scale[dB];
	int unscaledOut=UnscaledOutput(phase,phaseShift);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::EnvelopedOutputLn(int phase,int phaseShift,unsigned int timeInMS,unsigned int FB,int lastSlot0Out) const
{
	int env=InterpolateEnvelope(timeInMS);
	lastDb100Cache=env;
	int ampl=linear9600to4096[env];
	int unscaledOut=UnscaledOutput(phase,phaseShift,FB,lastSlot0Out);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::EnvelopedOutputLn(int phase,int phaseShift,unsigned int timeInMS) const
{
	int env=InterpolateEnvelope(timeInMS);
	lastDb100Cache=env;
	int ampl=linear9600to4096[env];
	int unscaledOut=UnscaledOutput(phase,phaseShift);
	return (unscaledOut*ampl)/4096;
}
inline int YM2612::Slot::InterpolateEnvelope(unsigned int timeInMS) const
{
	if(true!=InReleasePhase)
	{
		if(timeInMS<env[0]) // Attack
		{
			unsigned int x=4096*timeInMS/env[0];
			return (env[1]*attackExp[x])>>12;
		}
		else
		{
			timeInMS-=env[0];
			if(timeInMS<env[2])
			{
				return env[1]-(env[1]-env[3])*timeInMS/env[2];
			}
			else
			{
				timeInMS-=env[2];
				if(timeInMS<env[4])
				{
					return env[3]-env[3]*timeInMS/env[4];
				}
			}
		}
		return 0;
	}
	else
	{
		if(timeInMS<ReleaseEndTime && ReleaseStartTime<ReleaseEndTime)
		{
			auto diff=ReleaseEndTime-timeInMS;
			auto Db100=ReleaseStartDb100;
			Db100*=diff;
			Db100/=(ReleaseEndTime-ReleaseStartTime);
			return Db100;
		}
		return 0; // Not supported yet.
	}
}

unsigned int YM2612::Channel::Note(void) const
{
	// Formula [2] pp.204
	// There is an error.  F_NUM is 11bits.  There is no F11.
	// Probably, F11, F10, F9, F8 should be read F10, F9, F8, F7.
	unsigned int F10=((F_NUM>>10)&1);
	unsigned int F9= ((F_NUM>> 9)&1);
	unsigned int F8= ((F_NUM>> 8)&1);
	unsigned int F7=((F_NUM>>11)&1);
	unsigned int N3=(F10&(F9|F8|F7))|((~F10)&F9&F8&F7);
	unsigned int NOTE=(F10<<1)|N3;
	return NOTE;
}

////////////////////////////////////////////////////////////

void YM2612::KeyOn(unsigned int chNum,unsigned int slotFlags)
{
	if(0==slotFlags)
	{
		return;
	}


	auto &ch=state.channels[chNum];

	const unsigned int hertzX16=BLOCK_FNUM_to_FreqX16(ch.BLOCK,ch.F_NUM);

	state.playingCh|=(1<<chNum);
	ch.playState=CH_PLAYING;
	if(0!=(1&slotFlags))
	{
		ch.lastSlot0Out[0]=0;
		ch.lastSlot0Out[1]=0;
		ch.lastSlot0OutForNextWave[0]=ch.lastSlot0Out[0];
		ch.lastSlot0OutForNextWave[1]=ch.lastSlot0Out[1];
	}

	// Formulat in [2] pp.204 suggests:
 	//   unsigned int KC=(ch.BLOCK<<2)|NOTE;
	// which doesn't make sense.
	// SEGA Genesis Software Manaual tells KC is just top 5 bits of BLOCK|F_NUM2.
	// Which makes more sense.
	unsigned int KC=(ch.BLOCK<<2)|((ch.F_NUM>>9)&3);


	for(int i=0; i<NUM_SLOTS; ++i)
	{
		if(0!=(slotFlags&(1<<i)))
		{
			auto &slot=ch.slots[i];

			slot.InReleasePhase=false;
			slot.phase12=0;

			UpdatePhase12StepSlot(slot,hertzX16,slot.DetuneContributionToPhaseStepS12(ch.BLOCK,ch.Note()));

			// (hertzX16*PHASE_STEPS)<<8==hertz*PHASE_STEPS*4096
			CalculateEnvelope(slot.env,slot.RRCache,KC,slot);
			slot.envDurationCache=slot.env[0]+slot.env[2]+slot.env[4];
			slot.toneDurationMillisecS12=slot.envDurationCache;
			slot.toneDurationMillisecS12<<=12;


			// Observation tells that if key is turned on while the previous tone is still playing, 
			// The initial output level must start from the last output level, in which case
			// microsec12 must fast-forwarded so that the output matches the lastDb100Cache.
			// Linear interpolation will have error, but should be better than nothing.
			if(slot.lastDb100Cache<=0)
			{
				slot.microsecS12=0;
			}
			else if(slot.env[1]<=slot.lastDb100Cache)
			{
				slot.microsecS12=(slot.env[0]*1000)<<12;
			}
			else
			{
				unsigned int scale=attackExpInverse[4096*slot.lastDb100Cache/slot.env[1]];
				slot.microsecS12=(slot.env[0]*scale/4096)*1000<<12;
			}
			slot.lastDb100Cache=0;


			slot.nextMicrosecS12=slot.microsecS12;
			slot.nextPhase12=slot.phase12;
		}
	}

#ifdef YM2612_DEBUGOUTPUT
	printf("%d BLOCK %03xH F_NUM %03xH Hertz %d\n",KC,ch.BLOCK,ch.F_NUM,hertzX16/16);
#endif
}

void YM2612::UpdatePhase12StepSlot(Slot &slot,const unsigned int hertzX16,int detuneContribution)
{
	// Phase runs hertz*PHASE_STEPS times per second.
	//            hertz*PHASE_STEPS/WAVE_SAMPLING_RATE times per step.
	// Phase 12 runs
	//            0x1000*hertz*PHASE_STEPS/WAVE_SAMPLING_RATE per step.
	unsigned long long phase12Step;
	phase12Step=MULTITable[slot.MULTI]*hertzX16*PHASE_STEPS; // 2X from MULTITable, 16X from hertzX16
	phase12Step<<=7;                                         // 128X  Overall 2x16x128=4096X
	phase12Step+=MULTITable[slot.MULTI]*detuneContribution/2;
	phase12Step/=WAVE_SAMPLING_RATE;
	slot.phase12Step=(unsigned int)phase12Step;
}

void YM2612::UpdatePhase12StepSlot(Channel &ch)
{
	const unsigned int hertzX16=BLOCK_FNUM_to_FreqX16(ch.BLOCK,ch.F_NUM);
	for(auto &slot : ch.slots)
	{
		UpdatePhase12StepSlot(slot,hertzX16,slot.DetuneContributionToPhaseStepS12(ch.BLOCK,ch.Note()));
	};
}

void YM2612::KeyOff(unsigned int chNum,unsigned int slotFlags)
{
	if(0!=(state.playingCh&(1<<chNum)))
	{
		auto &ch=state.channels[chNum];
		for(auto &slot : ch.slots)
		{
			if(true!=slot.InReleasePhase)
			{
				slot.InReleasePhase=true;

				// 2020/12/15 nextMicrosecS12 retains up to what time point wave has been generated.
				//            Therefore, here must be nextMicrosecS12, instead of microsecS12.
				//            If microsecS12 is used, it virtually skips first 20ms of release,
				//            and the amplitude drops like a stairstep.
				//            It is inaudible in many situations, but clearly audible in Super Daisenryaku opening.
				slot.ReleaseStartTime=(slot.nextMicrosecS12>>12)/1000;
				slot.ReleaseStartDb100=slot.lastDb100Cache;

				uint64_t releaseTime=sustainDecayReleaseTime0to96dB[std::min<unsigned int>(slot.RRCache,63)];
				releaseTime*=slot.lastDb100Cache;
				releaseTime/=960000;
				slot.ReleaseEndTime=slot.ReleaseStartTime+releaseTime;
			#ifdef YM2612_DEBUGOUTPUT
				std::cout << "Release Time " << releaseTime << "ms  " << 
				             "Start " << slot.ReleaseStartTime << "ms  " << 
				             "End " << slot.ReleaseEndTime << "ms" << std::endl;
			#endif
			}
		}
	}
}

void YM2612::CheckToneDone(unsigned int chNum)
{
	auto &ch=state.channels[chNum];
	if(CH_PLAYING!=ch.playState)
	{
		state.playingCh&=~(1<<chNum);
	}
	else
	{
		bool slotStillPlaying=false;
		for(int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
		{
			auto &slot=ch.slots[connectionToOutputSlots[ch.CONNECT].slots[i]];
			auto millisec=(slot.microsecS12>>12)/1000;
			if(true==slot.InReleasePhase && millisec<slot.ReleaseEndTime)
			{
				slotStillPlaying=true;
				break;;
			}
			else if(true!=slot.InReleasePhase && millisec<slot.envDurationCache)
			{
				slotStillPlaying=true;
				break;;
			}
		}
		if(true!=slotStillPlaying)
		{
			state.playingCh&=~(1<<chNum);
			ch.playState=CH_IDLE;
			for(auto &s : ch.slots)
			{
				s.lastDb100Cache=0;
				s.InReleasePhase=false;
			}
		}
	}
}

void YM2612::CheckToneDoneAllChannels(void)
{
	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		CheckToneDone(chNum);
	}
}

std::vector <unsigned char> YM2612::MakeWaveAllChannels(unsigned long long int millisec) const
{
	std::vector <unsigned char> wave;

	unsigned long long int numSamples=(millisec<<12);
	numSamples*=WAVE_SAMPLING_RATE;
	numSamples/=1000;
	numSamples>>=12;

	wave.resize(4*numSamples);

	unsigned int nPlayingCh=0;
	unsigned int playingCh[NUM_CHANNELS];
	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		if(0!=(state.playingCh&(1<<chNum)))
		{
			playingCh[nPlayingCh++]=chNum;
		}
	}
	MakeWaveForNSamples(wave.data(),nPlayingCh,playingCh,numSamples);
	return wave;
}

std::vector <unsigned char> YM2612::MakeWave(unsigned int ch,unsigned long long int millisec) const
{
	std::vector <unsigned char> wave;

	unsigned long long int numSamples=(millisec<<12);
	numSamples*=WAVE_SAMPLING_RATE;
	numSamples/=1000;
	numSamples>>=12;

	wave.resize(4*numSamples);

	unsigned int playingCh[1]={ch};
	MakeWaveForNSamples(wave.data(),1,playingCh,numSamples);
	return wave;
}

class YM2612::WithLFO
{
public:
	static inline void CalculateLFO(int AMSAdjustment[4],int PMSAdjustment[4],unsigned int FREQCTRL,const Channel &ch)
	{
		if(0!=ch.PMS)
		{
			for(unsigned int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
			{
				auto sl=connectionToOutputSlots[ch.CONNECT].slots[i];
				int signedStep=ch.slots[sl].phase12Step;

				unsigned long long int LFOPhase=(ch.slots[sl].microsecS12>>12);
				LFOPhase=LFOPhase*PHASE_STEPS/LFOCycleMicroSec[FREQCTRL];

				int PMSAdj=PMS16384Table[ch.PMS]*sineTable[LFOPhase&PHASE_MASK]/UNSCALED_MAX;
				PMSAdjustment[sl]=signedStep*PMSAdj/16384/2;
			}
		}
		{
			for(unsigned int i=0; i<connectionToOutputSlots[ch.CONNECT].nOutputSlots; ++i)
			{
				auto sl=connectionToOutputSlots[ch.CONNECT].slots[i];
				if(0!=ch.slots[sl].AM)
				{
					unsigned long long int LFOPhase=(ch.slots[sl].microsecS12>>12);
					LFOPhase=LFOPhase*PHASE_STEPS/LFOCycleMicroSec[FREQCTRL];
					AMSAdjustment[sl]=4096+(AMS4096Table[ch.AMS]*sineTable[LFOPhase&PHASE_MASK])/UNSCALED_MAX;
				}
			}
		}
	}
};

class YM2612::WithoutLFO
{
public:
	static inline void CalculateLFO(int AMSAdjustment[4],int PMSAdjustment[4],unsigned int FREQCTRL,const Channel &ch)
	{
	}
};

template <class LFOClass>
long long int YM2612::MakeWaveForNSamplesTemplate(unsigned char wave[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamples) const
{
	const unsigned int microsec12Step=4096000000/WAVE_SAMPLING_RATE;
	// Time runs 1/WAVE_SAMPLING_RATE seconds per step
	//           1000/WAVE_SAMPLING_RATE milliseconds per step
	//           1000000/WAVE_SAMPLING_RATE microseconds per step
	//           1000000000/WAVE_SAMPLING_RATE nanoseconds per step

	// If microSec12=4096*microseconds, tm runs
	//           4096000000/WAVE_SAMPLING_RATE per step

	uint64_t microsec12[NUM_CHANNELS][NUM_SLOTS];
	unsigned int phase12[NUM_CHANNELS][NUM_SLOTS];
	int lastSlot0Out[NUM_CHANNELS][2];
	unsigned int LeftANDPtn[NUM_CHANNELS];
	unsigned int RightANDPtn[NUM_CHANNELS];
	unsigned long long int toneDurationMicrosecS12[NUM_CHANNELS][NUM_SLOTS];

	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		auto &ch=state.channels[chNum];
		microsec12[chNum][0]=ch.slots[0].microsecS12;
		microsec12[chNum][1]=ch.slots[1].microsecS12;
		microsec12[chNum][2]=ch.slots[2].microsecS12;
		microsec12[chNum][3]=ch.slots[3].microsecS12;
		phase12[chNum][0]=ch.slots[0].phase12;
		phase12[chNum][1]=ch.slots[1].phase12;
		phase12[chNum][2]=ch.slots[2].phase12;
		phase12[chNum][3]=ch.slots[3].phase12;
		lastSlot0Out[chNum][0]=ch.lastSlot0Out[0];
		lastSlot0Out[chNum][1]=ch.lastSlot0Out[1];
		LeftANDPtn[chNum]=(0!=ch.L ? ~0 : 0);
		RightANDPtn[chNum]=(0!=ch.R ? ~0 : 0);
		toneDurationMicrosecS12[chNum][0]=ch.slots[0].toneDurationMillisecS12*1000;
		toneDurationMicrosecS12[chNum][1]=ch.slots[1].toneDurationMillisecS12*1000;
		toneDurationMicrosecS12[chNum][2]=ch.slots[2].toneDurationMillisecS12*1000;
		toneDurationMicrosecS12[chNum][3]=ch.slots[3].toneDurationMillisecS12*1000;
	}

	unsigned int i;
	for(i=0; i<numSamples && 0<nPlayingCh; ++i)
	{
		int leftOut=0,rightOut=0;
		for(int j=nPlayingCh-1; 0<=j; --j)
		{
			auto chNum=playingCh[j];
			auto &ch=state.channels[chNum];
			if(toneDurationMicrosecS12[chNum][0]<=microsec12[chNum][0] &&
			   toneDurationMicrosecS12[chNum][1]<=microsec12[chNum][1] &&
			   toneDurationMicrosecS12[chNum][2]<=microsec12[chNum][2] &&
			   toneDurationMicrosecS12[chNum][3]<=microsec12[chNum][3])
			{
				playingCh[j]=playingCh[nPlayingCh-1];
				--nPlayingCh;
				break;
			}

			int PMSAdjustment[4]=
			{
				0,0,0,0
			};
			int AMSAdjustment[4]=
			{
				4096,4096,4096,4096
			};

			LFOClass::CalculateLFO(AMSAdjustment,PMSAdjustment,state.FREQCTRL,ch);


			// Why take an average of last two samples, not just the last sample, for feedback?
			// Cisc's FMGEN YM emulator does it.  I was wondering why.  But, it works as a damper to prevent
			// premature divergence.
			// 
			// When feedback is given, the output from slot 0 is calculated as:
			// 
			//     y(i+1)=A*sin(i*dt+C*y(i))
			// 
			// A is amplitude calculated from the envelope, and C is defined by feedback level, and
			// dt depends on the frequency of the tone.
			// 
			// Let's denote the angle given to the sine function as:
			// 
			//     X(i)=i*dt+C*y(i)
			// 
			// When the slope of sin(X(i)) is negative (0.5PI<X(i)<1.5PI), means the value is decreasing with
			// increasing X, the function can become unstable and diverge, until the function returns to the
			// positive slope.  Here's what can happen.
			// 
			// X(i) monotonicly increase if C=0, means no feedback.  However, if C is non-zero, i*dt increases, 
			// but C*y(i) can increase or decrease depending on the slope of y(i).
			// 
			// If the one-step decrease of C*y(i) exceeds the increase of i*dt (which is dt), 
			// X(i) will decrease overall, which means the input to the sine functions goes backward.
			// Amplitude is positive, and if the slope of the sine function at X(i) was negative, and if 
			// the input goes backward, y suddenly increases. i.e., y(i+1) is greater than y(i).
			// 
			// Then in the next step, both C*y and i*dt terms increase.  This makes a large jump of X.
			// The sine function is going down with X, as a result y dives down bigger than the last 
			// increase.  Then, the next comes even bigger increase of y, followed by even bigger dive, 
			// and the function of y(i) starts oscillating every sample.
			// 
			// From the above, the condition that starts this divergence is:
			// 
			//     dt<-dY
			// 
			// where Y=C*y(i).  Interestingly, because of this condition, this particular mode of oscillation
			// only appears when dY<0.
			// 
			// While it is a legitimate divergence that adds some noise component to the tone in some settings,
			// Slot 0 starts this oscillation too easily.
			// 
			// Cisc's FMGEN YM-chip emulator, used in XM7 and M88 emulators (and maybe other emulators as well)
			// solves this problem by feeding the average of the last two samples back to slot 0.
			// 
			// During the oscillation, the value of y jumps up and down.  But, if you look at the middle of the two
			// consecutive outputs, it goes through a smooth decreasing curve.
			// 
			// The cause of this divergence is from the oscillation of the feedback term, C*y.  By taking average
			// of the last two samples, it essentially damps the oscillation, and effectively prevents this divergence.
			//
			// So the modified formulation is:
			//
			//     y(i+1)=A*sin(i*dt+C*(y(i)+y(i-1))/2)
			//
			// Here I am explicitly writing it out, but Cisc's implementation embedded this division by two
			// in the anyway-required bit shift.
			//
			// Very genious solution it is.

			auto s0Out=(lastSlot0Out[chNum][1]+lastSlot0Out[chNum][0])/2;
			auto ampl=CalculateAmplitude(chNum,microsec12[chNum],phase12[chNum],AMSAdjustment,s0Out);
			lastSlot0Out[chNum][1]=lastSlot0Out[chNum][0];
			lastSlot0Out[chNum][0]=s0Out;

			leftOut+=(LeftANDPtn[chNum]&ampl);
			rightOut+=(RightANDPtn[chNum]&ampl);

			phase12[chNum][0]+=ch.slots[0].phase12Step+PMSAdjustment[0];
			phase12[chNum][1]+=ch.slots[1].phase12Step+PMSAdjustment[1];
			phase12[chNum][2]+=ch.slots[2].phase12Step+PMSAdjustment[2];
			phase12[chNum][3]+=ch.slots[3].phase12Step+PMSAdjustment[3];
			microsec12[chNum][0]+=microsec12Step;
			microsec12[chNum][1]+=microsec12Step;
			microsec12[chNum][2]+=microsec12Step;
			microsec12[chNum][3]+=microsec12Step;
		}
		WordOp_Set(wave+i*4  ,leftOut);
		WordOp_Set(wave+i*4+2,rightOut);
	}

	std::memset(wave+i*4,0,(numSamples-i)*4);

	for(unsigned int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		auto &ch=state.channels[chNum];
		ch.lastSlot0OutForNextWave[0]=lastSlot0Out[chNum][0];
		ch.lastSlot0OutForNextWave[1]=lastSlot0Out[chNum][1];
		ch.slots[0].nextMicrosecS12=microsec12[chNum][0];
		ch.slots[1].nextMicrosecS12=microsec12[chNum][1];
		ch.slots[2].nextMicrosecS12=microsec12[chNum][2];
		ch.slots[3].nextMicrosecS12=microsec12[chNum][3];
		ch.slots[0].nextPhase12=phase12[chNum][0];
		ch.slots[1].nextPhase12=phase12[chNum][1];
		ch.slots[2].nextPhase12=phase12[chNum][2];
		ch.slots[3].nextPhase12=phase12[chNum][3];
	}

// std::cout << (microsec12>>12) << "us " << std::endl;
// std::cout << phase12[0] << "," << (phase12[0]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[1] << "," << (phase12[1]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[2] << "," << (phase12[2]>>12)/PHASE_STEPS << "cycles" << std::endl;
// std::cout << phase12[3] << "," << (phase12[3]>>12)/PHASE_STEPS << "cycles" << std::endl;

	return i;
}

long long int YM2612::MakeWaveForNSamples(unsigned char wave[],unsigned int nPlayingCh,unsigned int playingCh[],unsigned long long int numSamples) const
{
	if(true==state.LFO)
	{
		return MakeWaveForNSamplesTemplate <WithLFO> (wave,nPlayingCh,playingCh,numSamples);
	}
	else
	{
		return MakeWaveForNSamplesTemplate <WithoutLFO> (wave,nPlayingCh,playingCh,numSamples);
	}
}

void YM2612::NextWave(unsigned int chNum)
{
	auto &ch=state.channels[chNum];
	if(CH_PLAYING==ch.playState)
	{
		ch.lastSlot0Out[0]=ch.lastSlot0OutForNextWave[0];
		ch.lastSlot0Out[1]=ch.lastSlot0OutForNextWave[1];
		ch.slots[0].microsecS12=ch.slots[0].nextMicrosecS12;
		ch.slots[1].microsecS12=ch.slots[1].nextMicrosecS12;
		ch.slots[2].microsecS12=ch.slots[2].nextMicrosecS12;
		ch.slots[3].microsecS12=ch.slots[3].nextMicrosecS12;
		ch.slots[0].phase12=ch.slots[0].nextPhase12;
		ch.slots[1].phase12=ch.slots[1].nextPhase12;
		ch.slots[2].phase12=ch.slots[2].nextPhase12;
		ch.slots[3].phase12=ch.slots[3].nextPhase12;
	}
}

void YM2612::NextWaveAllChannels(void)
{
	for(int chNum=0; chNum<NUM_CHANNELS; ++chNum)
	{
		NextWave(chNum);
	}
}

bool YM2612::CalculateEnvelope(unsigned int env[6],unsigned int &RR,unsigned int KC,const Slot &slot) const
{
	KC&=31;

#ifdef YM2612_DEBUGOUTPUT
	std::cout << KC << "," << slot.KS << "," << (KC>>(3-slot.KS)) << ", ";
#endif

	unsigned int AR=slot.AR*2+(KC>>(3-slot.KS));
	unsigned int DR=slot.DR*2+(KC>>(3-slot.KS));
	unsigned int SR=slot.SR*2+(KC>>(3-slot.KS));
	             RR=(slot.RR*2+1)*2+(KC>>(3-slot.KS));  // [2] pp.206 Double RR and add 1.
	AR=std::min(AR,63U);
	DR=std::min(DR,63U);
	SR=std::min(SR,63U);
	RR=std::min(RR,63U);

	if(AR<4)
	{
		return NoTone(env,RR);
	}

	auto TLdB100=TLtoDB100[slot.TL];
	auto SLdB100=SLtoDB100[slot.SL];

	if(9600<=TLdB100)
	{
		return NoTone(env,RR);
	}

	const unsigned int TLinv=9600-TLdB100;

#ifdef YM2612_DEBUGOUTPUT
	std::cout << "AR=" << AR << " DR=" << DR << " SR=" << SR << " TL=" << slot.TL  << " SL=" << slot.SL ;
	std::cout << " ";
#endif

	// Ealier I was linearly interpolating the amplitude, but maybe it is linear in dB scale.
	env[1]=TLinv;
	env[3]=(SLdB100<TLinv ? TLinv-SLdB100 : 0);
	env[5]=0;


	// After reading fmgen.c (Written by cisc, Author of M88 PC8801 emulator), it looks to be that
	// the time for attack doesn't depend on the total level, but it takes time to raise 0dB to 96dB all the time.
	// Then, the time for decay is based on SL only.  Just like dropping from 96dB to 96-dB(SL) dB.
	// The secondary decay duration should also depend only on SL, the time for 96-dB(SL)dB to 0dB.
	// The amplitude change is not linear, but I approximate by a linear function.  I may come back to the envelope
	// generation once I get a good enough approximation.
	unsigned long long int mul;
	env[0]=attackTime0to96dB[AR]/100;
	mul=SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[DR];
	mul/=960000;
	env[2]=(unsigned int)mul;
	mul=9600-SLdB100;
	mul*=sustainDecayReleaseTime0to96dB[SR];
	mul/=960000;
	env[4]=(unsigned int)mul;

	// ?
	// If, AR, DR, SR, and RR are really rates, the duration for attack, decay, and sustain should depend
	// on the amplitude.  If the amplitude for total level is high, it should take longer to get to the level.
	// Or, if it is the rate, the slope of the decay should be the same regardless of the TL amplitude.
	// But, the value calculated from this assumption doesn't make sense at all.
	// It rather makes sense if I take the number from the table without scaling by the amplitude.

	// env[0]=attackTime0to96dB[AR]/100;
	// env[2]=sustainDecayReleaseTime0to96dB[DR]/100;
	// env[4]=sustainDecayReleaseTime0to96dB[SR]/100;

	// ?
	// If it is really rate, the following code should better emulate, but doesn't look to be.
	// unsigned long long int attackTime=attackTime0to96dB[AR]; // 1/100 milliseconds for jumping from 0 to 127 (96dB)
	// // If TLampl==127, it takes attackTime/100 milliseconds to reach TLampl.
	// attackTime*=TLampl;
	// env[0]=(unsigned int)(attackTime/12700);

	// unsigned long long int decayTime=sustainDecayReleaseTime0to96dB[DR];
	// decayTime*=(TLampl-SLampl);
	// env[2]=(unsigned int)(decayTime/12700);

	// unsigned long long int sustainTime=sustainDecayReleaseTime0to96dB[SR];
	// sustainTime*=SLampl;
	// sustainTime/=127;
	// if(sustainTime<TONE_CHOPOFF_MILLISEC*100)
	// {
	// 	env[4]=(unsigned int)(sustainTime/12700);
	// }
	// else
	// {
	// 	env[4]=TONE_CHOPOFF_MILLISEC;
	// }

#ifdef YM2612_DEBUGOUTPUT
	for(int i=0; i<6; ++i){std::cout << env[i] << ",";}
	std::cout << "  RR=" << RR << "(" << sustainDecayReleaseTime0to96dB[RR]/100 << ")";
	std::cout << std::endl;
#endif

	return true;
}

int YM2612::CalculateAmplitude(int chNum,const uint64_t timeInMicrosecS12[NUM_SLOTS],const unsigned int slotPhase12[NUM_SLOTS],const int AMS4096[4],int &lastSlot0Out) const
{
	if(true==channelMute[chNum])
	{
		return 0;
	}

	auto &ch=state.channels[chNum];
	bool slotActive[4]=
	{
		0!=(ch.usingSlot&1) || ch.slots[0].InReleasePhase,
		0!=(ch.usingSlot&2) || ch.slots[1].InReleasePhase,
		0!=(ch.usingSlot&4) || ch.slots[2].InReleasePhase,
		0!=(ch.usingSlot&8) || ch.slots[3].InReleasePhase,
	};

	unsigned int timeInMS[NUM_SLOTS]=
	{
		(unsigned int)((timeInMicrosecS12[0]>>12)/1000),
		(unsigned int)((timeInMicrosecS12[1]>>12)/1000),
		(unsigned int)((timeInMicrosecS12[2]>>12)/1000),
		(unsigned int)((timeInMicrosecS12[3]>>12)/1000),
	};

	#define SLOTOUTEV_Db_0(phaseShift) ((true!=slotActive[0] ? 0 : ch.slots[0].EnvelopedOutputDb((slotPhase12[0]>>12),phaseShift,timeInMS[0],ch.FB,lastSlot0Out))*AMS4096[0]/4096)
	#define SLOTOUTEV_Db_1(phaseShift) ((true!=slotActive[1] ? 0 : ch.slots[1].EnvelopedOutputDb((slotPhase12[1]>>12),phaseShift,timeInMS[1]))*AMS4096[1]/4096)
	#define SLOTOUTEV_Db_2(phaseShift) ((true!=slotActive[2] ? 0 : ch.slots[2].EnvelopedOutputDb((slotPhase12[2]>>12),phaseShift,timeInMS[2]))*AMS4096[2]/4096)
	#define SLOTOUTEV_Db_3(phaseShift) ((true!=slotActive[3] ? 0 : ch.slots[3].EnvelopedOutputDb((slotPhase12[3]>>12),phaseShift,timeInMS[3]))*AMS4096[3]/4096)

	#define SLOTOUTEV_Ln_0(phaseShift) ((true!=slotActive[0] ? 0 : ch.slots[0].EnvelopedOutputLn((slotPhase12[0]>>12),phaseShift,timeInMS[0],ch.FB,lastSlot0Out))*AMS4096[0]/4096)
	#define SLOTOUTEV_Ln_1(phaseShift) ((true!=slotActive[1] ? 0 : ch.slots[1].EnvelopedOutputLn((slotPhase12[1]>>12),phaseShift,timeInMS[1]))*AMS4096[1]/4096)
	#define SLOTOUTEV_Ln_2(phaseShift) ((true!=slotActive[2] ? 0 : ch.slots[2].EnvelopedOutputLn((slotPhase12[2]>>12),phaseShift,timeInMS[2]))*AMS4096[2]/4096)
	#define SLOTOUTEV_Ln_3(phaseShift) ((true!=slotActive[3] ? 0 : ch.slots[3].EnvelopedOutputLn((slotPhase12[3]>>12),phaseShift,timeInMS[3]))*AMS4096[3]/4096)

	#define SLOTOUT_0(phaseShift) ((true!=slotActive[0] ? 0 : ch.slots[0].UnscaledOutput((slotPhase12[0]>>12),phaseShift,ch.FB,lastSlot0Out))*AMS4096[0]/4096)
	#define SLOTOUT_1(phaseShift) ((true!=slotActive[1] ? 0 : ch.slots[1].UnscaledOutput((slotPhase12[1]>>12),phaseShift))*AMS4096[1]/4096)
	#define SLOTOUT_2(phaseShift) ((true!=slotActive[2] ? 0 : ch.slots[2].UnscaledOutput((slotPhase12[2]>>12),phaseShift))*AMS4096[2]/4096)
	#define SLOTOUT_3(phaseShift) ((true!=slotActive[3] ? 0 : ch.slots[3].UnscaledOutput((slotPhase12[3]>>12),phaseShift))*AMS4096[3]/4096)

	int s0out,s1out,s2out,s3out;
	switch(ch.CONNECT)
	{
	default:
	case 0:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(s1out);
		return SLOTOUTEV_Db_3(s2out)*state.volume/UNSCALED_MAX;
	case 1:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(s0out+s1out);
		return SLOTOUTEV_Db_3(s2out)*state.volume/UNSCALED_MAX;
	case 2:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(s1out);
		return SLOTOUTEV_Db_3(s0out+s2out)*state.volume/UNSCALED_MAX;
	case 3:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0);
		return SLOTOUTEV_Db_3(s1out+s2out)*state.volume/UNSCALED_MAX;
	case 4:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0);
		s3out=SLOTOUTEV_Db_3(s2out);
		return ((s1out+s3out)*state.volume/UNSCALED_MAX);
		// Test only Slot 3 -> return SLOTOUTEV_Db_3(0)*state.volume/UNSCALED_MAX;
	case 5:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(s0out);
		s3out=SLOTOUTEV_Db_3(s0out);
		return ((s1out+s2out+s3out)*state.volume/UNSCALED_MAX);
	case 6:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(s0out);
		s2out=SLOTOUTEV_Db_2(0    );
		s3out=SLOTOUTEV_Db_3(0    );
		return ((s1out+s2out+s3out)*state.volume/UNSCALED_MAX);
	case 7:
		s0out=SLOTOUTEV_Db_0(0);
		lastSlot0Out=s0out;
		s1out=SLOTOUTEV_Db_1(0);
		s2out=SLOTOUTEV_Db_2(0);
		s3out=SLOTOUTEV_Db_3(0);
		return ((s0out+s1out+s2out+s3out)*state.volume/UNSCALED_MAX);
	}
}
