/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: BITSHIFTLEFT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of BITSHIFTLEFT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_BITSHIFTLEFTINSTRUCTIONS__
#define __FZX80_BITSHIFTLEFTINSTRUCTIONS__

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class BITSHIFTLeft_General : public Instruction
	{
		public:
		BITSHIFTLeft_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// Rotate
		/** A register. \n
			When crr = true		=>	bit 0 <- CF, CF <- bit 7.
			When crr = false	=>	CF <- bit 7, bit 0 <- bit 7. */
		bool executeRotateWith (MCHEmul::Register& r, bool crr);
		/** A memory address. crr like above. */
		bool executeRotateWith (const MCHEmul::Address& a, bool crr);
		/** A memory address and the result is left in a regiter. crr like above. */
		bool executeRotateWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr);

		// Shift
		/** A register. \n
			When crr = true		=>	bit 0 <- 1, CF <- bit 7.
			When crr = false	=>	bit 0 <- 0, CF <- bit 7. */
		bool executeShiftWith (MCHEmul::Register& r, bool crr);
		/** A memory address. crr like above. */
		bool executeShiftWith (const MCHEmul::Address& a, bool crr);
		/** A memory address and the result is left in a regiter. crr like above. */
		bool executeShiftWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr);

		private:
		/** A common way to affect the flags for all these instructions. */
		inline void affectFlagsWith (const MCHEmul::UByte& v, bool c);
	};

	// ---
	inline void BITSHIFTLeft_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		st.setBitStatus (CZ80::_CARRYFLAG, c);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
		st.setBitStatus (CZ80::_BIT3FLAG, v.bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, v.bit (7));
	}

	// Rotate with bit 7...
	// Within A
	_INST_FROM (0x07,		1, 4, 4,	"RLCA",				RLC_A, BITSHIFTLeft_General);
	// Within A, but slower...
	_INST_FROM (0xCB07,		2, 8, 8,	"RLC A",			RLC_ASlower, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB00,		2, 8, 8,	"RLC B",			RLC_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB01,		2, 8, 8,	"RLC C",			RLC_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB02,		2, 8, 8,	"RLC D",			RLC_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB03,		2, 8, 8,	"RLC E",			RLC_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB04,		2, 8, 8,	"RLC H",			RLC_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB05,		2, 8, 8,	"RLC L",			RLC_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB06,		2, 15, 15,	"RLC (HL)",			RLC_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB06,	4, 23, 23,	"RLC (IX+[#1])",	RLC_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB07,	4, 23, 23,	"RLC (IX+[#1]), A",	RLC_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB00,	4, 23, 23,	"RLC (IX+[#1]), B",	RLC_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB01,	4, 23, 23,	"RLC (IX+[#1]), C",	RLC_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB02,	4, 23, 23,	"RLC (IX+[#1]), D",	RLC_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB03,	4, 23, 23,	"RLC (IX+[#1]), E",	RLC_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB04,	4, 23, 23,	"RLC (IX+[#1]), H",	RLC_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB05,	4, 23, 23,	"RLC (IX+[#1]), L",	RLC_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB06,	4, 23, 23,	"RLC (IY+[#1])",	RLC_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB07,	4, 23, 23,	"RLC (IY+[#1]), A",	RLC_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB00,	4, 23, 23,	"RLC (IY+[#1]), B",	RLC_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB01,	4, 23, 23,	"RLC (IY+[#1]), C",	RLC_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB02,	4, 23, 23,	"RLC (IY+[#1]), D",	RLC_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB03,	4, 23, 23,	"RLC (IY+[#1]), E",	RLC_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB04,	4, 23, 23,	"RLC (IY+[#1]), H",	RLC_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB05,	4, 23, 23,	"RLC (IY+[#1]), L",	RLC_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented

	// Rotate with carry!
	// Within A
	_INST_FROM (0x17,		1, 4, 4,	"RLA",				RL_A, BITSHIFTLeft_General);
	// Within A, but slower...
	_INST_FROM (0xCB17,		2, 8, 8,	"RL A",				RL_ASlower, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB10,		2, 8, 8,	"RL B",				RL_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB11,		2, 8, 8,	"RL C",				RL_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB12,		2, 8, 8,	"RL D",				RL_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB13,		2, 8, 8,	"RL E",				RL_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB14,		2, 8, 8,	"RL H",				RL_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB15,		2, 8, 8,	"RL L",				RL_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB16,		2, 15, 15,	"RL (HL)",			RL_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB16,	4, 23, 23,	"RL (IX+[#1])",		RL_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB17,	4, 23, 23,	"RL (IX+[#1]), A",	RL_IndirectIndexIXCopyA, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB10,	4, 23, 23,	"RL (IX+[#1]), B",	RL_IndirectIndexIXCopyB, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB11,	4, 23, 23,	"RL (IX+[#1]), C",	RL_IndirectIndexIXCopyC, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB12,	4, 23, 23,	"RL (IX+[#1]), D",	RL_IndirectIndexIXCopyD, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB13,	4, 23, 23,	"RL (IX+[#1]), E",	RL_IndirectIndexIXCopyE, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB14,	4, 23, 23,	"RL (IX+[#1]), H",	RL_IndirectIndexIXCopyH, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB15,	4, 23, 23,	"RL (IX+[#1]), L",	RL_IndirectIndexIXCopyL, BITSHIFTLeft_General);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB16,	4, 23, 23,	"RL (IY+[#1])",		RL_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB17,	4, 23, 23,	"RL (IY+[#1]), A",	RL_IndirectIndexIYCopyA, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB10,	4, 23, 23,	"RL (IY+[#1]), B",	RL_IndirectIndexIYCopyB, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB11,	4, 23, 23,	"RL (IY+[#1]), C",	RL_IndirectIndexIYCopyC, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB12,	4, 23, 23,	"RL (IY+[#1]), D",	RL_IndirectIndexIYCopyD, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB13,	4, 23, 23,	"RL (IY+[#1]), E",	RL_IndirectIndexIYCopyE, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB14,	4, 23, 23,	"RL (IY+[#1]), H",	RL_IndirectIndexIYCopyH, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB15,	4, 23, 23,	"RL (IY+[#1]), L",	RL_IndirectIndexIYCopyL, BITSHIFTLeft_General);		// Undocumented

	/** The content of low nibble (HL) is copied into the high one,
		the content of the high nibble (HL) is copied into the low nibble A, 
		and the low nibble A is copied into the low nibble (HL). */
	_INST_FROM (0xED6F,		2, 18, 18,	"RLD",				RLD, BITSHIFTLeft_General);

	// Shift introducing a 0 in the first bit!
	// Within A
	_INST_FROM (0xCB27,		2, 8, 8,	"SLA A",			SLA_A, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB20,		2, 8, 8,	"SLA B",			SLA_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB21,		2, 8, 8,	"SLA C",			SLA_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB22,		2, 8, 8,	"SLA D",			SLA_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB23,		2, 8, 8,	"SLA E",			SLA_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB24,		2, 8, 8,	"SLA H",			SLA_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB25,		2, 8, 8,	"SLA L",			SLA_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB26,		2, 15, 15,	"SLA (HL)",			SLA_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB26,	4, 23, 23,	"SLA (IX+[#1])",	SLA_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB27,	4, 23, 23,	"SLA (IX+[#1]), A",	SLA_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB20,	4, 23, 23,	"SLA (IX+[#1]), B",	SLA_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB21,	4, 23, 23,	"SLA (IX+[#1]), C",	SLA_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB22,	4, 23, 23,	"SLA (IX+[#1]), D",	SLA_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB23,	4, 23, 23,	"SLA (IX+[#1]), E",	SLA_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB24,	4, 23, 23,	"SLA (IX+[#1]), H",	SLA_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB25,	4, 23, 23,	"SLA (IX+[#1]), L",	SLA_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB26,	4, 23, 23,	"SLA (IY+[#1])",	SLA_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB27,	4, 23, 23,	"SLA (IY+[#1]), A",	SLA_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB20,	4, 23, 23,	"SLA (IY+[#1]), B",	SLA_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB21,	4, 23, 23,	"SLA (IY+[#1]), C",	SLA_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB22,	4, 23, 23,	"SLA (IY+[#1]), D",	SLA_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB23,	4, 23, 23,	"SLA (IY+[#1]), E",	SLA_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB24,	4, 23, 23,	"SLA (IY+[#1]), H",	SLA_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB25,	4, 23, 23,	"SLA (IY+[#1]), L",	SLA_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented

	// Shift introducing a 1 in the first bit!
	// Within A
	_INST_FROM (0xCB37,		2, 8, 8,	"SLL A",			SLL_A, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB30,		2, 8, 8,	"SLL B",			SLL_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB31,		2, 8, 8,	"SLL C",			SLL_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB32,		2, 8, 8,	"SLL D",			SLL_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB33,		2, 8, 8,	"SLL E",			SLL_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB34,		2, 8, 8,	"SLL H",			SLL_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB35,		2, 8, 8,	"SLL L",			SLL_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB36,		2, 15, 15,	"SLL (HL)",			SLL_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB36,	4, 23, 23,	"SLL (IX+[#1])",	SLL_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB37,	4, 23, 23,	"SLL (IX+[#1]), A",	SLL_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB30,	4, 23, 23,	"SLL (IX+[#1]), B",	SLL_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB31,	4, 23, 23,	"SLL (IX+[#1]), C",	SLL_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB32,	4, 23, 23,	"SLL (IX+[#1]), D",	SLL_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB33,	4, 23, 23,	"SLL (IX+[#1]), E",	SLL_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB34,	4, 23, 23,	"SLL (IX+[#1]), H",	SLL_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB35,	4, 23, 23,	"SLL (IX+[#1]), L",	SLL_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB36,	4, 23, 23,	"SLL (IY+[#1])",	SLL_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB37,	4, 23, 23,	"SLL (IY+[#1]), A",	SLL_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB30,	4, 23, 23,	"SLL (IY+[#1]), B",	SLL_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB31,	4, 23, 23,	"SLL (IY+[#1]), C",	SLL_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB32,	4, 23, 23,	"SLL (IY+[#1]), D",	SLL_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB33,	4, 23, 23,	"SLL (IY+[#1]), E",	SLL_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB34,	4, 23, 23,	"SLL (IY+[#1]), H",	SLL_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB35,	4, 23, 23,	"SLL (IY+[#1]), L",	SLL_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented
}

#endif
  
// End of the file
/*@}*/
