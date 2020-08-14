/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef I486INST_IS_INCLUDED
#define I486INST_IS_INCLUDED
/* { */


// Adding support for a new instruction:
// Instruction-Set /digit means the real instruction depends on the REG bits of MODR/M byte (operand[0])
//  (1) Add OPCODE enum.
//  (2) Add RENUMBER enum.  (Make sure to delete =0x??)
//  (3) Add a mapping in MakeOpCodeRenumberTable()
//  (4) Implement i486DX::FetchOperand
//  (5) Implement i486DX::Instruction::Disassemble
//  (6) Implement i486DX::RunOneInstruction
//  (7) Add disassembly test.


/*
0x80,0x81,0x83 can be:
ADD(REG=0)
OR(REG=1)
ADC(REG=2)
SBB(REG=3)
AND(REG=4)
SUB(REG=5)
XOR(REG=6)
CMP(REG=7)
	I486_OPCODE_BINARYOP_RM8_FROM_I8=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	I486_OPCODE_BINARYOP_R_FROM_I=     0x81,
	I486_OPCODE_BINARYOP_RM_FROM_SXI8= 0x83,

0xF6,0xF7 can be:
TEST(REG=0)

0xFF can be:
JMP(REG=4,5)
CALL(REG=2,3)
*/


enum 
{
//	I486_OPCODE_

	I486_OPCODE_MAX=0xFFF,

	I486_OPCODE_UNDEFINED_SHOOT_INT6=0x0FA6,  // In TownsOS V1.1 this instruction is used for testing(?) INT 6.

	I486_OPCODE_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8=0xC0,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8=0xC1, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1=0xD0, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1=0xD1,  // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL=0xD2,// ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL=0xD3, // ROL(REG=0),ROR(REG=1),RCL(REG=2),RCR(REG=3),SAL/SHL(REG=4),SHR(REG=5),SAR(REG=7)
	I486_OPCODE_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV=0xF6,
	I486_OPCODE_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV=0xF7,
	I486_OPCODE_INC_DEC_R_M8=                     0xFE, // INC(REG=0),DEC(REG=1)
	I486_OPCODE_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH =0xFF, // INC(REG=0),DEC(REG=1),CALL(REG=2),CALLF(REG=3),JMP(REG=4),JMPF(REG=5),PUSH(REG=6)
	I486_OPCODE_LGDT_LIDT_SGDT_SIDT=                        0x0F01, // LGDT(REG=2),LIDT(REG=3)

	I486_OPCODE_AAA=        0x37,
	I486_OPCODE_AAM=    0xD4,
	I486_OPCODE_AAD=    0xD5,
	I486_OPCODE_AAS=        0x3F,


	I486_OPCODE_ARPL=       0x63,

	I486_OPCODE_BOUND=           0x62,

	I486_OPCODE_BT_BTS_BTR_BTC_RM_I8=0xFBA, // BT(REG=4),BTS(REG=5),BTR(REG=6),BTC(REG=7)
	I486_OPCODE_BSF_R_RM=   0x0FBC,
	I486_OPCODE_BSR_R_RM=   0x0FBD,
	I486_OPCODE_BT_R_RM=    0x0FA3,
	I486_OPCODE_BTC_RM_R=   0x0FBB,
	I486_OPCODE_BTS_RM_R=   0x0FAB,
	I486_OPCODE_BTR_RM_R=   0x0FB3,


	I486_OPCODE_CALL_REL=   0xE8,
	I486_OPCODE_CALL_FAR=   0x9A,


	I486_OPCODE_CLC=        0xF8,
	I486_OPCODE_CLD=        0xFC,
	I486_OPCODE_CLI=        0xFA,

	I486_OPCODE_CLTS=       0x0F06,

	I486_OPCODE_CMC=        0xF5,


	I486_OPCODE_ADC_AL_FROM_I8=  0x14,
	I486_OPCODE_ADC_A_FROM_I=    0x15,
	I486_OPCODE_ADC_RM8_FROM_R8= 0x10,
	I486_OPCODE_ADC_RM_FROM_R=   0x11,
	I486_OPCODE_ADC_R8_FROM_RM8= 0x12,
	I486_OPCODE_ADC_R_FROM_RM=   0x13,


	I486_OPCODE_ADD_AL_FROM_I8=  0x04,
	I486_OPCODE_ADD_A_FROM_I=    0x05,
	I486_OPCODE_ADD_RM8_FROM_R8= 0x00,
	I486_OPCODE_ADD_RM_FROM_R=   0x01,
	I486_OPCODE_ADD_R8_FROM_RM8= 0x02,
	I486_OPCODE_ADD_R_FROM_RM=   0x03,


	I486_OPCODE_AND_AL_FROM_I8=  0x24,
	I486_OPCODE_AND_A_FROM_I=    0x25,
	I486_OPCODE_AND_RM8_FROM_R8= 0x20,
	I486_OPCODE_AND_RM_FROM_R=   0x21,
	I486_OPCODE_AND_R8_FROM_RM8= 0x22,
	I486_OPCODE_AND_R_FROM_RM=   0x23,


	I486_OPCODE_CBW_CWDE=        0x98,
	I486_OPCODE_CWD_CDQ=         0x99,


	I486_OPCODE_CMP_AL_FROM_I8=  0x3C,
	I486_OPCODE_CMP_A_FROM_I=    0x3D,
	I486_OPCODE_CMP_RM8_FROM_R8= 0x38,
	I486_OPCODE_CMP_RM_FROM_R=   0x39,
	I486_OPCODE_CMP_R8_FROM_RM8= 0x3A,
	I486_OPCODE_CMP_R_FROM_RM=   0x3B,


	I486_OPCODE_CMPSB=           0xA6,
	I486_OPCODE_CMPS=            0xA7,


	I486_OPCODE_DAA=             0x27,
	I486_OPCODE_DAS=             0x2F,


	I486_OPCODE_DEC_EAX=    0x48, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_ECX=    0x49, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_EDX=    0x4A, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_EBX=    0x4B, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_ESP=    0x4C, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_EBP=    0x4D, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_ESI=    0x4E, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_DEC_EDI=    0x4F, // 16/32 depends on OPSIZE_OVERRIDE


	I486_OPCODE_ENTER=      0xC8,


	I486_OPCODE_FWAIT=      0x9B,
	I486_OPCODE_FPU_D8_FADD=                   0xD8, // REG 0:FADD
	I486_OPCODE_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_= 0xD9,
	I486_OPCODE_FPU_DB_FNINIT_FRSTOR=     0xDB, 
	I486_OPCODE_FPU_DC_FADD=                   0xDC, // REG 0:FADD
	I486_OPCODE_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM= 0xDD,
	I486_OPCODE_FPU_DF_FNSTSW_AX=  0xDF,
	// [1] i486 Programmers Reference does not list DD /7 as FNSTSW.
	// However, [5] 80387 Programmers Reference example uses DD 3C as FNSTSW.
	// [4] also lists DD /7 as FNSTSW.  Most likely [1] pp.26-136 listing of DF /7 is wrong.
	// Linux objump interprets DD /7 as FNSTSW.  [1] lists DD /7 as FIST as well.


	I486_OPCODE_HLT=        0xF4,


	I486_OPCODE_IMUL_R_RM_I8=0x6B,
	I486_OPCODE_IMUL_R_RM_IMM=0x69,
	I486_OPCODE_IMUL_R_RM=  0x0FAF,


	I486_OPCODE_INSB=       0x6C,


	I486_OPCODE_IN_AL_I8=   0xE4,
	I486_OPCODE_IN_A_I8=    0xE5,
	I486_OPCODE_IN_AL_DX=   0xEC,
	I486_OPCODE_IN_A_DX=    0xED,


	I486_OPCODE_INC_EAX=    0x40, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_ECX=    0x41, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_EDX=    0x42, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_EBX=    0x43, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_ESP=    0x44, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_EBP=    0x45, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_ESI=    0x46, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_INC_EDI=    0x47, // 16/32 depends on OPSIZE_OVERRIDE


	I486_OPCODE_INT3=       0xCC,
	I486_OPCODE_INT=        0xCD,
	I486_OPCODE_INTO=       0xCE,


	I486_OPCODE_IRET=   0xCF,


	// I486_OPCODE_JCXZ_REL8= 0xE3, Same as JCXZ_REL8
	I486_OPCODE_JECXZ_REL8=0xE3,  // Depending on the operand size

	I486_OPCODE_JA_REL8=   0x77,
	I486_OPCODE_JAE_REL8=  0x73,
	I486_OPCODE_JB_REL8=   0x72,
	I486_OPCODE_JBE_REL8=  0x76,
	// I486_OPCODE_JC_REL8=   0x72, Same as JB_REL8
	I486_OPCODE_JE_REL8=   0x74,
	// I486_OPCODE_JZ_REL8=   0x74, Same as JE_REL8
	I486_OPCODE_JG_REL8=   0x7F,
	I486_OPCODE_JGE_REL8=  0x7D,
	I486_OPCODE_JL_REL8=   0x7C,
	I486_OPCODE_JLE_REL8=  0x7E,
	// I486_OPCODE_JNA_REL8=  0x76, Same as JBE_REL8
	// I486_OPCODE_JNAE_REL8= 0x72, Same as JB_REL8
	// I486_OPCODE_JNB_REL8=  0x73, Same as JNC_REL8
	// I486_OPCODE_JNBE_REL8= 0x77, Same as JA_REL8
	// I486_OPCODE_JNC_REL8=  0x73, Same as JAE_REL8
	I486_OPCODE_JNE_REL8=  0x75,
	// I486_OPCODE_JNG_REL8=  0x7E, Same as JLE_REL8
	// I486_OPCODE_JNGE_REL8= 0x7C, Same as JL_REL8
	// I486_OPCODE_JNL_REL8=  0x7D, Same as JGE_REL8
	// I486_OPCODE_JNLE_REL8= 0x7F, Same as JG_REL8
	I486_OPCODE_JNO_REL8=  0x71,
	I486_OPCODE_JNP_REL8=  0x7B,
	I486_OPCODE_JNS_REL8=  0x79,
	// I486_OPCODE_JNZ_REL8=  0x75, Same as JNE_REL8
	I486_OPCODE_JO_REL8=   0x70,
	I486_OPCODE_JP_REL8=   0x7A,
	// I486_OPCODE_JPE_REL8=  0x7A, Same as JP_REL8
	// I486_OPCODE_JPO_REL8=  0x7B, Same as JNP_REL8
	I486_OPCODE_JS_REL8=   0x78,
	// I486_OPCODE_JZ_REL8=   0x74, Same as JE_REL8

	// Relative offset 32/16 depends on operand size.  Not address size.
	I486_OPCODE_JA_REL=    0x0F87,
	I486_OPCODE_JAE_REL=   0x0F83,
	I486_OPCODE_JB_REL=    0x0F82,
	I486_OPCODE_JBE_REL=   0x0F86,
	// I486_OPCODE_JC_REL=    0x0F82, Same as JB_REL
	I486_OPCODE_JE_REL=    0x0F84,
	// I486_OPCODE_JZ_REL=    0x0F84, Same as JZ_REL
	I486_OPCODE_JG_REL=    0x0F8F,
	I486_OPCODE_JGE_REL=   0x0F8D,
	I486_OPCODE_JL_REL=    0x0F8C,
	I486_OPCODE_JLE_REL=   0x0F8E,
	// I486_OPCODE_JNA_REL=   0x0F86, Same as JBE_REL
	// I486_OPCODE_JNAE_REL=  0x0F82, Same as JB_REL
	// I486_OPCODE_JNB_REL=   0x0F83, Same as JAE_REL
	// I486_OPCODE_JNBE_REL=  0x0F87, Same as JA_REL
	// I486_OPCODE_JNC_REL=   0x0F83, Same as JAE_REL
	I486_OPCODE_JNE_REL=   0x0F85,
	// I486_OPCODE_JNG_REL=   0x0F8E, Same as JLE_REL
	// I486_OPCODE_JNGE_REL=  0x0F8C, Same as JL_REL
	// I486_OPCODE_JNL_REL=   0x0F8D, Same as JGE_REL
	// I486_OPCODE_JNLE_REL=  0x0F8F, Same as JG_REL
	I486_OPCODE_JNO_REL=   0x0F81,
	I486_OPCODE_JNP_REL=   0x0F8B,
	I486_OPCODE_JNS_REL=   0x0F89,
	// I486_OPCODE_JNZ_REL=   0x0F85, Same as JNE_REL
	I486_OPCODE_JO_REL=    0x0F80,
	I486_OPCODE_JP_REL=    0x0F8A,
	// I486_OPCODE_JPE_REL=   0x0F8A, Same as JP_REL
	// I486_OPCODE_JPO_REL=   0x0F8B, Same as JNP_REL
	I486_OPCODE_JS_REL=    0x0F88,
	// I486_OPCODE_JZ_REL=    0x0F84, Same as JE_REL


	I486_OPCODE_JMP_REL8=         0xEB,   // cb
	I486_OPCODE_JMP_REL=          0xE9,   // cw or cd
	I486_OPCODE_JMP_FAR=          0xEA,   // cd or cp


	// AND, OR, or XOR
	I486_OPCODE_BINARYOP_RM8_FROM_I8=  0x80, // AND(REG=4), OR(REG=1), or XOR(REG=6) depends on the REG field of MODR/M
	I486_OPCODE_BINARYOP_R_FROM_I=     0x81,
	I486_OPCODE_BINARYOP_RM8_FROM_I8_ALIAS=0x82, 
	// [4], https://github.com/aquynh/capstone/issues/238 imply 0x82 is alias for 0x80.
	// Also Linux objdump seems to interpret 0x82 and 0x80 same.
	I486_OPCODE_BINARYOP_RM_FROM_SXI8= 0x83,


	I486_OPCODE_LAHF=             0x9F,


	I486_OPCODE_LEA=              0x8D,


	I486_OPCODE_LDS=              0xC5,
	I486_OPCODE_LSS=              0x0FB2,
	I486_OPCODE_LES=              0xC4,
	I486_OPCODE_LFS=              0x0FB4,
	I486_OPCODE_LGS=              0x0FB5,


	I486_OPCODE_LEAVE=            0xC9,


	I486_OPCODE_LODSB=            0xAC,
	I486_OPCODE_LODS=             0xAD,


	I486_OPCODE_LOOP=             0xE2,
	I486_OPCODE_LOOPE=            0xE1,
	I486_OPCODE_LOOPNE=           0xE0,


	I486_OPCODE_LSL=              0x0F03,


	I486_OPCODE_MOV_FROM_R8=      0x88,
	I486_OPCODE_MOV_FROM_R=       0x89, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_TO_R8=        0x8A,
	I486_OPCODE_MOV_TO_R=         0x8B, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_FROM_SEG=     0x8C,
	I486_OPCODE_MOV_TO_SEG=       0x8E,
	I486_OPCODE_MOV_M_TO_AL=      0xA0, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	I486_OPCODE_MOV_M_TO_EAX=     0xA1, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	I486_OPCODE_MOV_M_FROM_AL=    0xA2, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	I486_OPCODE_MOV_M_FROM_EAX=   0xA3, // 16/32 depends on ADDRESSSIZE_OVERRIDE
	I486_OPCODE_MOV_I8_TO_AL=     0xB0,
	I486_OPCODE_MOV_I8_TO_CL=     0xB1,
	I486_OPCODE_MOV_I8_TO_DL=     0xB2,
	I486_OPCODE_MOV_I8_TO_BL=     0xB3,
	I486_OPCODE_MOV_I8_TO_AH=     0xB4,
	I486_OPCODE_MOV_I8_TO_CH=     0xB5,
	I486_OPCODE_MOV_I8_TO_DH=     0xB6,
	I486_OPCODE_MOV_I8_TO_BH=     0xB7,
	I486_OPCODE_MOV_I_TO_EAX=     0xB8, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_ECX=     0xB9, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_EDX=     0xBA, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_EBX=     0xBB, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_ESP=     0xBC, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_EBP=     0xBD, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_ESI=     0xBE, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I_TO_EDI=     0xBF, // 16/32 depends on OPSIZE_OVERRIDE
	I486_OPCODE_MOV_I8_TO_RM8=    0xC6,
	I486_OPCODE_MOV_I_TO_RM=      0xC7, // 16/32 depends on OPSIZE_OVERRIDE


	I486_OPCODE_MOV_TO_CR=        0x0F22,
	I486_OPCODE_MOV_FROM_CR=      0x0F20,
	I486_OPCODE_MOV_FROM_DR=      0x0F21,
	I486_OPCODE_MOV_TO_DR=        0x0F23,
	I486_OPCODE_MOV_FROM_TR=      0x0F24,
	I486_OPCODE_MOV_TO_TR=        0x0F26,


	I486_OPCODE_MOVSB=            0xA4,
	I486_OPCODE_MOVS=             0xA5,


	I486_OPCODE_MOVSX_R_RM8=      0x0FBE,
	I486_OPCODE_MOVSX_R32_RM16=   0x0FBF,


	I486_OPCODE_MOVZX_R_RM8=      0x0FB6,
	I486_OPCODE_MOVZX_R32_RM16=   0x0FB7,


	I486_OPCODE_NOP=              0x90,


	I486_OPCODE_OUT_I8_AL=        0xE6,
	I486_OPCODE_OUT_I8_A=         0xE7,
	I486_OPCODE_OUT_DX_AL=        0xEE,
	I486_OPCODE_OUT_DX_A=         0xEF,


	I486_OPCODE_OUTSB=            0x6E,
	I486_OPCODE_OUTS=             0x6F,


	I486_OPCODE_PUSHA=            0x60,
	I486_OPCODE_PUSHF=            0x9C,


	I486_OPCODE_PUSH_EAX=         0x50,
	I486_OPCODE_PUSH_ECX=         0x51,
	I486_OPCODE_PUSH_EDX=         0x52,
	I486_OPCODE_PUSH_EBX=         0x53,
	I486_OPCODE_PUSH_ESP=         0x54,
	I486_OPCODE_PUSH_EBP=         0x55,
	I486_OPCODE_PUSH_ESI=         0x56,
	I486_OPCODE_PUSH_EDI=         0x57,
	I486_OPCODE_PUSH_I8=          0x6A,
	I486_OPCODE_PUSH_I=           0x68,
	I486_OPCODE_PUSH_CS=          0x0E,
	I486_OPCODE_PUSH_SS=          0x16,
	I486_OPCODE_PUSH_DS=          0x1E,
	I486_OPCODE_PUSH_ES=          0x06,
	I486_OPCODE_PUSH_FS=          0x0FA0,
	I486_OPCODE_PUSH_GS=          0x0FA8,


	I486_OPCODE_POP_M=            0x8F,
	I486_OPCODE_POP_EAX=          0x58,
	I486_OPCODE_POP_ECX=          0x59,
	I486_OPCODE_POP_EDX=          0x5A,
	I486_OPCODE_POP_EBX=          0x5B,
	I486_OPCODE_POP_ESP=          0x5C,
	I486_OPCODE_POP_EBP=          0x5D,
	I486_OPCODE_POP_ESI=          0x5E,
	I486_OPCODE_POP_EDI=          0x5F,
	I486_OPCODE_POP_SS=           0x17,
	I486_OPCODE_POP_DS=           0x1F,
	I486_OPCODE_POP_ES=           0x07,
	I486_OPCODE_POP_FS=           0x0FA1,
	I486_OPCODE_POP_GS=           0x0FA9,

	I486_OPCODE_POPA=             0x61,
	I486_OPCODE_POPF=             0x9D,


	I486_OPCODE_OR_AL_FROM_I8=    0x0C,
	I486_OPCODE_OR_A_FROM_I=      0x0D,
	I486_OPCODE_OR_RM8_FROM_R8=   0x08,
	I486_OPCODE_OR_RM_FROM_R=     0x09,
	I486_OPCODE_OR_R8_FROM_RM8=   0x0A,
	I486_OPCODE_OR_R_FROM_RM=     0x0B,


	I486_OPCODE_RET=              0xC3,
	I486_OPCODE_RETF=             0xCB,
	I486_OPCODE_RET_I16=          0xC2,
	I486_OPCODE_RETF_I16=         0xCA,


	I486_OPCODE_SAHF=             0x9E,


	I486_OPCODE_SHLD_RM_I8=       0x0FA4,
	I486_OPCODE_SHLD_RM_CL=       0x0FA5,
	I486_OPCODE_SHRD_RM_I8=       0x0FAC,
	I486_OPCODE_SHRD_RM_CL=       0x0FAD,


	I486_OPCODE_SCASB=            0xAE,
	I486_OPCODE_SCAS=             0xAF,


	I486_OPCODE_SETA=             0x0F97,
	I486_OPCODE_SETAE=            0x0F93,
	I486_OPCODE_SETB=             0x0F92,
	I486_OPCODE_SETBE=            0x0F96,
	// I486_OPCODE_SETC=             0x0F92,
	I486_OPCODE_SETE=             0x0F94,
	I486_OPCODE_SETG=             0x0F9F,
	I486_OPCODE_SETGE=            0x0F9D,
	I486_OPCODE_SETL=             0x0F9C,
	I486_OPCODE_SETLE=            0x0F9E,
	//I486_OPCODE_SETNA=            0x0F96,
	//I486_OPCODE_SETNAE=           0x0F92,
	//I486_OPCODE_SETNB=            0x0F93,
	//I486_OPCODE_SETNBE=           0x0F97,
	//I486_OPCODE_SETNC=            0x0F93,
	I486_OPCODE_SETNE=            0x0F95,
	//I486_OPCODE_SETNG=            0x0F9E,
	//I486_OPCODE_SETNGE=           0x0F9C,
	//I486_OPCODE_SETNL=            0x0F9D,
	//I486_OPCODE_SETNLE=           0x0F9F,
	I486_OPCODE_SETNO=            0x0F91,
	I486_OPCODE_SETNP=            0x0F9B,
	I486_OPCODE_SETNS=            0x0F99,
	// I486_OPCODE_SETNZ=            0x0F95,
	I486_OPCODE_SETO=             0x0F90,
	I486_OPCODE_SETP=             0x0F9A,
	//I486_OPCODE_SETPE=            0x0F9A,
	//I486_OPCODE_SETPO=            0x0F9B,
	I486_OPCODE_SETS=             0x0F98,
	// I486_OPCODE_SETZ=             0x0F94,


	I486_OPCODE_SLDT_STR_LLDT_LTR_VERR_VERW=             0x0F00,


	I486_OPCODE_STC=              0xF9,
	I486_OPCODE_STD=              0xFD,
	I486_OPCODE_STI=              0xFB,


	I486_OPCODE_STOSB=            0xAA,
	I486_OPCODE_STOS=             0xAB,


	I486_OPCODE_SBB_AL_FROM_I8=  0x1C,
	I486_OPCODE_SBB_A_FROM_I=    0x1D,
	I486_OPCODE_SBB_RM8_FROM_R8= 0x18,
	I486_OPCODE_SBB_RM_FROM_R=   0x19,
	I486_OPCODE_SBB_R8_FROM_RM8= 0x1A,
	I486_OPCODE_SBB_R_FROM_RM=   0x1B,


	I486_OPCODE_SUB_AL_FROM_I8=  0x2C,
	I486_OPCODE_SUB_A_FROM_I=    0x2D,
	I486_OPCODE_SUB_RM8_FROM_R8= 0x28,
	I486_OPCODE_SUB_RM_FROM_R=   0x29,
	I486_OPCODE_SUB_R8_FROM_RM8= 0x2A,
	I486_OPCODE_SUB_R_FROM_RM=   0x2B,


	I486_OPCODE_TEST_AL_FROM_I8=  0xA8,
	I486_OPCODE_TEST_A_FROM_I=    0xA9,
	I486_OPCODE_TEST_RM8_FROM_R8= 0x84,
	I486_OPCODE_TEST_RM_FROM_R=   0x85,


	// XCHG EAX,EAX (0x90) is NOP
	I486_OPCODE_XCHG_EAX_ECX=     0x91,
	I486_OPCODE_XCHG_EAX_EDX=     0x92,
	I486_OPCODE_XCHG_EAX_EBX=     0x93,
	I486_OPCODE_XCHG_EAX_ESP=     0x94,
	I486_OPCODE_XCHG_EAX_EBP=     0x95,
	I486_OPCODE_XCHG_EAX_ESI=     0x96,
	I486_OPCODE_XCHG_EAX_EDI=     0x97,
	I486_OPCODE_XCHG_RM8_R8=      0x86,
	I486_OPCODE_XCHG_RM_R=        0x87,


	I486_OPCODE_XLAT=             0xD7,


	I486_OPCODE_XOR_AL_FROM_I8=   0x34,
	I486_OPCODE_XOR_A_FROM_I=     0x35,
	I486_OPCODE_XOR_RM8_FROM_R8=  0x30,
	I486_OPCODE_XOR_RM_FROM_R=    0x31,
	I486_OPCODE_XOR_R8_FROM_RM8=  0x32,
	I486_OPCODE_XOR_R_FROM_RM=    0x33,
};

// Visual C++ creates separate jump tables if the case numbers are sparse.
// I don't know about other compilers though.
// To avoid multiple jump-table checks, the instruction code needs to be renumbered to a sequential number.

enum 
{
//	I486_RENUMBER_

	I486_RENUMBER_UNDEFINED_SHOOT_INT6,

	I486_RENUMBER_C0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_I8,
	I486_RENUMBER_C1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_I8,
	I486_RENUMBER_D0_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_1,
	I486_RENUMBER_D1_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_1,
	I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM8_CL,
	I486_RENUMBER_D3_ROL_ROR_RCL_RCR_SAL_SAR_SHL_SHR_RM_CL,
	I486_RENUMBER_F6_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV,
	I486_RENUMBER_F7_TEST_NOT_NEG_MUL_IMUL_DIV_IDIV,
	I486_RENUMBER_INC_DEC_R_M8,
	I486_RENUMBER_INC_DEC_CALL_CALLF_JMP_JMPF_PUSH ,
	I486_RENUMBER_LGDT_LIDT_SGDT_SIDT,

	I486_RENUMBER_AAA,
	I486_RENUMBER_AAM,
	I486_RENUMBER_AAD,
	I486_RENUMBER_AAS,


	I486_RENUMBER_ARPL,

	I486_RENUMBER_BOUND,

	I486_RENUMBER_BT_BTS_BTR_BTC_RM_I8,
	I486_RENUMBER_BSF_R_RM,
	I486_RENUMBER_BSR_R_RM,
	I486_RENUMBER_BT_R_RM,
	I486_RENUMBER_BTC_RM_R,
	I486_RENUMBER_BTS_RM_R,
	I486_RENUMBER_BTR_RM_R,


	I486_RENUMBER_CALL_REL,
	I486_RENUMBER_CALL_FAR,


	I486_RENUMBER_CLC,
	I486_RENUMBER_CLD,
	I486_RENUMBER_CLI,

	I486_RENUMBER_CLTS,

	I486_RENUMBER_CMC,


	I486_RENUMBER_ADC_AL_FROM_I8,
	I486_RENUMBER_ADC_A_FROM_I,
	I486_RENUMBER_ADC_RM8_FROM_R8,
	I486_RENUMBER_ADC_RM_FROM_R,
	I486_RENUMBER_ADC_R8_FROM_RM8,
	I486_RENUMBER_ADC_R_FROM_RM,


	I486_RENUMBER_ADD_AL_FROM_I8,
	I486_RENUMBER_ADD_A_FROM_I,
	I486_RENUMBER_ADD_RM8_FROM_R8,
	I486_RENUMBER_ADD_RM_FROM_R,
	I486_RENUMBER_ADD_R8_FROM_RM8,
	I486_RENUMBER_ADD_R_FROM_RM,


	I486_RENUMBER_AND_AL_FROM_I8,
	I486_RENUMBER_AND_A_FROM_I,
	I486_RENUMBER_AND_RM8_FROM_R8,
	I486_RENUMBER_AND_RM_FROM_R,
	I486_RENUMBER_AND_R8_FROM_RM8,
	I486_RENUMBER_AND_R_FROM_RM,


	I486_RENUMBER_CBW_CWDE,
	I486_RENUMBER_CWD_CDQ,


	I486_RENUMBER_CMP_AL_FROM_I8,
	I486_RENUMBER_CMP_A_FROM_I,
	I486_RENUMBER_CMP_RM8_FROM_R8,
	I486_RENUMBER_CMP_RM_FROM_R,
	I486_RENUMBER_CMP_R8_FROM_RM8,
	I486_RENUMBER_CMP_R_FROM_RM,


	I486_RENUMBER_CMPSB,
	I486_RENUMBER_CMPS,


	I486_RENUMBER_DAA,
	I486_RENUMBER_DAS,


	I486_RENUMBER_DEC_EAX,
	I486_RENUMBER_DEC_ECX,
	I486_RENUMBER_DEC_EDX,
	I486_RENUMBER_DEC_EBX,
	I486_RENUMBER_DEC_ESP,
	I486_RENUMBER_DEC_EBP,
	I486_RENUMBER_DEC_ESI,
	I486_RENUMBER_DEC_EDI,


	I486_RENUMBER_ENTER,


	I486_RENUMBER_FWAIT,
	I486_RENUMBER_FPU_D8_FADD,
	I486_RENUMBER_FPU_D9_FNSTCW_M16_FNSTENV_F2XM1_FXAM_FXCH_FXTRACT_FYL2X_FYL2XP1_FABS_,
	I486_RENUMBER_FPU_DB_FNINIT_FRSTOR,
	I486_RENUMBER_FPU_DC_FADD,
	I486_RENUMBER_FPU_DD_FLD_FSAVE_FST_FNSTSW_M16_FFREE_FUCOM,
	I486_RENUMBER_FPU_DF_FNSTSW_AX,
	
	
	
	


	I486_RENUMBER_HLT,


	I486_RENUMBER_IMUL_R_RM_I8,
	I486_RENUMBER_IMUL_R_RM_IMM,
	I486_RENUMBER_IMUL_R_RM,


	I486_RENUMBER_INSB,


	I486_RENUMBER_IN_AL_I8,
	I486_RENUMBER_IN_A_I8,
	I486_RENUMBER_IN_AL_DX,
	I486_RENUMBER_IN_A_DX,


	I486_RENUMBER_INC_EAX,
	I486_RENUMBER_INC_ECX,
	I486_RENUMBER_INC_EDX,
	I486_RENUMBER_INC_EBX,
	I486_RENUMBER_INC_ESP,
	I486_RENUMBER_INC_EBP,
	I486_RENUMBER_INC_ESI,
	I486_RENUMBER_INC_EDI,


	I486_RENUMBER_INT3,
	I486_RENUMBER_INT,
	I486_RENUMBER_INTO,


	I486_RENUMBER_IRET,


	
	I486_RENUMBER_JECXZ_REL8,

	I486_RENUMBER_JA_REL8,
	I486_RENUMBER_JAE_REL8,
	I486_RENUMBER_JB_REL8,
	I486_RENUMBER_JBE_REL8,
	
	I486_RENUMBER_JE_REL8,
	
	I486_RENUMBER_JG_REL8,
	I486_RENUMBER_JGE_REL8,
	I486_RENUMBER_JL_REL8,
	I486_RENUMBER_JLE_REL8,
	
	
	
	
	
	I486_RENUMBER_JNE_REL8,
	
	
	
	
	I486_RENUMBER_JNO_REL8,
	I486_RENUMBER_JNP_REL8,
	I486_RENUMBER_JNS_REL8,
	
	I486_RENUMBER_JO_REL8,
	I486_RENUMBER_JP_REL8,
	
	
	I486_RENUMBER_JS_REL8,
	

	
	I486_RENUMBER_JA_REL,
	I486_RENUMBER_JAE_REL,
	I486_RENUMBER_JB_REL,
	I486_RENUMBER_JBE_REL,
	
	I486_RENUMBER_JE_REL,
	
	I486_RENUMBER_JG_REL,
	I486_RENUMBER_JGE_REL,
	I486_RENUMBER_JL_REL,
	I486_RENUMBER_JLE_REL,
	
	
	
	
	
	I486_RENUMBER_JNE_REL,
	
	
	
	
	I486_RENUMBER_JNO_REL,
	I486_RENUMBER_JNP_REL,
	I486_RENUMBER_JNS_REL,
	
	I486_RENUMBER_JO_REL,
	I486_RENUMBER_JP_REL,
	
	
	I486_RENUMBER_JS_REL,
	


	I486_RENUMBER_JMP_REL8,
	I486_RENUMBER_JMP_REL,
	I486_RENUMBER_JMP_FAR,


	
	I486_RENUMBER_BINARYOP_RM8_FROM_I8,
	I486_RENUMBER_BINARYOP_R_FROM_I,
	I486_RENUMBER_BINARYOP_RM8_FROM_I8_ALIAS,
	
	
	I486_RENUMBER_BINARYOP_RM_FROM_SXI8,


	I486_RENUMBER_LAHF,


	I486_RENUMBER_LEA,


	I486_RENUMBER_LDS,
	I486_RENUMBER_LSS,
	I486_RENUMBER_LES,
	I486_RENUMBER_LFS,
	I486_RENUMBER_LGS,


	I486_RENUMBER_LEAVE,


	I486_RENUMBER_LODSB,
	I486_RENUMBER_LODS,


	I486_RENUMBER_LOOP,
	I486_RENUMBER_LOOPE,
	I486_RENUMBER_LOOPNE,


	I486_RENUMBER_LSL,


	I486_RENUMBER_MOV_FROM_R8,
	I486_RENUMBER_MOV_FROM_R,
	I486_RENUMBER_MOV_TO_R8,
	I486_RENUMBER_MOV_TO_R,
	I486_RENUMBER_MOV_FROM_SEG,
	I486_RENUMBER_MOV_TO_SEG,
	I486_RENUMBER_MOV_M_TO_AL,
	I486_RENUMBER_MOV_M_TO_EAX,
	I486_RENUMBER_MOV_M_FROM_AL,
	I486_RENUMBER_MOV_M_FROM_EAX,
	I486_RENUMBER_MOV_I8_TO_AL,
	I486_RENUMBER_MOV_I8_TO_CL,
	I486_RENUMBER_MOV_I8_TO_DL,
	I486_RENUMBER_MOV_I8_TO_BL,
	I486_RENUMBER_MOV_I8_TO_AH,
	I486_RENUMBER_MOV_I8_TO_CH,
	I486_RENUMBER_MOV_I8_TO_DH,
	I486_RENUMBER_MOV_I8_TO_BH,
	I486_RENUMBER_MOV_I_TO_EAX,
	I486_RENUMBER_MOV_I_TO_ECX,
	I486_RENUMBER_MOV_I_TO_EDX,
	I486_RENUMBER_MOV_I_TO_EBX,
	I486_RENUMBER_MOV_I_TO_ESP,
	I486_RENUMBER_MOV_I_TO_EBP,
	I486_RENUMBER_MOV_I_TO_ESI,
	I486_RENUMBER_MOV_I_TO_EDI,
	I486_RENUMBER_MOV_I8_TO_RM8,
	I486_RENUMBER_MOV_I_TO_RM,


	I486_RENUMBER_MOV_TO_CR,
	I486_RENUMBER_MOV_FROM_CR,
	I486_RENUMBER_MOV_FROM_DR,
	I486_RENUMBER_MOV_TO_DR,
	I486_RENUMBER_MOV_FROM_TR,
	I486_RENUMBER_MOV_TO_TR,


	I486_RENUMBER_MOVSB,
	I486_RENUMBER_MOVS,


	I486_RENUMBER_MOVSX_R_RM8,
	I486_RENUMBER_MOVSX_R32_RM16,


	I486_RENUMBER_MOVZX_R_RM8,
	I486_RENUMBER_MOVZX_R32_RM16,


	I486_RENUMBER_NOP,


	I486_RENUMBER_OUT_I8_AL,
	I486_RENUMBER_OUT_I8_A,
	I486_RENUMBER_OUT_DX_AL,
	I486_RENUMBER_OUT_DX_A,


	I486_RENUMBER_OUTSB,
	I486_RENUMBER_OUTS,


	I486_RENUMBER_PUSHA,
	I486_RENUMBER_PUSHF,


	I486_RENUMBER_PUSH_EAX,
	I486_RENUMBER_PUSH_ECX,
	I486_RENUMBER_PUSH_EDX,
	I486_RENUMBER_PUSH_EBX,
	I486_RENUMBER_PUSH_ESP,
	I486_RENUMBER_PUSH_EBP,
	I486_RENUMBER_PUSH_ESI,
	I486_RENUMBER_PUSH_EDI,
	I486_RENUMBER_PUSH_I8,
	I486_RENUMBER_PUSH_I,
	I486_RENUMBER_PUSH_CS,
	I486_RENUMBER_PUSH_SS,
	I486_RENUMBER_PUSH_DS,
	I486_RENUMBER_PUSH_ES,
	I486_RENUMBER_PUSH_FS,
	I486_RENUMBER_PUSH_GS,


	I486_RENUMBER_POP_M,
	I486_RENUMBER_POP_EAX,
	I486_RENUMBER_POP_ECX,
	I486_RENUMBER_POP_EDX,
	I486_RENUMBER_POP_EBX,
	I486_RENUMBER_POP_ESP,
	I486_RENUMBER_POP_EBP,
	I486_RENUMBER_POP_ESI,
	I486_RENUMBER_POP_EDI,
	I486_RENUMBER_POP_SS,
	I486_RENUMBER_POP_DS,
	I486_RENUMBER_POP_ES,
	I486_RENUMBER_POP_FS,
	I486_RENUMBER_POP_GS,

	I486_RENUMBER_POPA,
	I486_RENUMBER_POPF,


	I486_RENUMBER_OR_AL_FROM_I8,
	I486_RENUMBER_OR_A_FROM_I,
	I486_RENUMBER_OR_RM8_FROM_R8,
	I486_RENUMBER_OR_RM_FROM_R,
	I486_RENUMBER_OR_R8_FROM_RM8,
	I486_RENUMBER_OR_R_FROM_RM,


	I486_RENUMBER_RET,
	I486_RENUMBER_RETF,
	I486_RENUMBER_RET_I16,
	I486_RENUMBER_RETF_I16,


	I486_RENUMBER_SAHF,


	I486_RENUMBER_SHLD_RM_I8,
	I486_RENUMBER_SHLD_RM_CL,
	I486_RENUMBER_SHRD_RM_I8,
	I486_RENUMBER_SHRD_RM_CL,


	I486_RENUMBER_SCASB,
	I486_RENUMBER_SCAS,


	I486_RENUMBER_SETA,
	I486_RENUMBER_SETAE,
	I486_RENUMBER_SETB,
	I486_RENUMBER_SETBE,
	
	I486_RENUMBER_SETE,
	I486_RENUMBER_SETG,
	I486_RENUMBER_SETGE,
	I486_RENUMBER_SETL,
	I486_RENUMBER_SETLE,
	
	
	
	
	
	I486_RENUMBER_SETNE,
	
	
	
	
	I486_RENUMBER_SETNO,
	I486_RENUMBER_SETNP,
	I486_RENUMBER_SETNS,
	
	I486_RENUMBER_SETO,
	I486_RENUMBER_SETP,
	
	
	I486_RENUMBER_SETS,
	


	I486_RENUMBER_SLDT_STR_LLDT_LTR_VERR_VERW,


	I486_RENUMBER_STC,
	I486_RENUMBER_STD,
	I486_RENUMBER_STI,


	I486_RENUMBER_STOSB,
	I486_RENUMBER_STOS,


	I486_RENUMBER_SBB_AL_FROM_I8,
	I486_RENUMBER_SBB_A_FROM_I,
	I486_RENUMBER_SBB_RM8_FROM_R8,
	I486_RENUMBER_SBB_RM_FROM_R,
	I486_RENUMBER_SBB_R8_FROM_RM8,
	I486_RENUMBER_SBB_R_FROM_RM,


	I486_RENUMBER_SUB_AL_FROM_I8,
	I486_RENUMBER_SUB_A_FROM_I,
	I486_RENUMBER_SUB_RM8_FROM_R8,
	I486_RENUMBER_SUB_RM_FROM_R,
	I486_RENUMBER_SUB_R8_FROM_RM8,
	I486_RENUMBER_SUB_R_FROM_RM,


	I486_RENUMBER_TEST_AL_FROM_I8,
	I486_RENUMBER_TEST_A_FROM_I,
	I486_RENUMBER_TEST_RM8_FROM_R8,
	I486_RENUMBER_TEST_RM_FROM_R,


	I486_RENUMBER_XCHG_EAX_ECX,
	I486_RENUMBER_XCHG_EAX_EDX,
	I486_RENUMBER_XCHG_EAX_EBX,
	I486_RENUMBER_XCHG_EAX_ESP,
	I486_RENUMBER_XCHG_EAX_EBP,
	I486_RENUMBER_XCHG_EAX_ESI,
	I486_RENUMBER_XCHG_EAX_EDI,
	I486_RENUMBER_XCHG_RM8_R8,
	I486_RENUMBER_XCHG_RM_R,


	I486_RENUMBER_XLAT,


	I486_RENUMBER_XOR_AL_FROM_I8,
	I486_RENUMBER_XOR_A_FROM_I,
	I486_RENUMBER_XOR_RM8_FROM_R8,
	I486_RENUMBER_XOR_RM_FROM_R,
	I486_RENUMBER_XOR_R8_FROM_RM8,
	I486_RENUMBER_XOR_R_FROM_RM,


I486_NUM_SUPPORTED_INSTRUCTIONS,
};


/* } */
#endif
