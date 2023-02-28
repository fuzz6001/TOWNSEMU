#ifndef I486FIDELITY_IS_INCLUDED
#define I486FIDELITY_IS_INCLUDED
/* { */

class i486DXLowFidelity
{
public:
	inline static void SaveESPHighWord(uint8_t,uint32_t){}
	inline static void RestoreESPHighWord(uint8_t,uint32_t &){}

	inline static void Sync_SS_CS_RPL_to_DPL(class i486DX &,i486DX::SegmentRegister &,i486DX::SegmentRegister &){}
};

class i486DXDefaultFidelity : public i486DXLowFidelity
{
public:
};

class i486DXHighFidelity : public i486DXDefaultFidelity
{
public:
	uint32_t ESPHigh;
	inline void SaveESPHighWord(uint8_t addressSize,uint32_t ESP)
	{
		if(16==addressSize)
		{
			ESPHigh=(ESP&0xFFFF0000);
		}
	}
	inline void RestoreESPHighWord(uint8_t addressSize,uint32_t &ESP)
	{
		if(16==addressSize)
		{
			ESP&=0xFFFF;ESP|=ESPHigh;
		}
	}

	inline static void Sync_SS_CS_RPL_to_DPL(class i486DX &cpu,i486DX::SegmentRegister &CS,i486DX::SegmentRegister &setSeg)
	{
		if(&CS==&setSeg)
		{
			cpu.state.CS().value&=0xFFFC;
			cpu.state.CS().value|=cpu.state.CS().DPL;
			cpu.state.SS().value&=0xFFFC;
			cpu.state.SS().value|=cpu.state.CS().DPL;
		}
	}
};

/* } */
#endif