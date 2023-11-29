/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: BITSHIFTRIGHT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of BITSHIFTRIGHT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_BITSHIFTRIGHTINSTRUCTIONS__
#define __FZX80_BITSHIFTRIGHTINSTRUCTIONS__

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class BITSHIFTRight_General : public Instruction
	{
		public:
		BITSHIFTRight_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// Rotate
		/** A register. \n
			When crr = true		=>	bit 7 <- CF, CF <- bit 0.
			When crr = false	=>	CF <- bit 0, bit 7 <- bit 0. */
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
	inline void BITSHIFTRight_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
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

	// Rotate with bit 7!
	// Within A
	_INST_FROM (0x0F,		1, 4, 4,	"RRCA",				RRC_A, BITSHIFTRight_General);
	// Within A, but slower...
	_INST_FROM (0xCB0F,		2, 8, 8,	"RRC A",			RRC_ASlower, BITSHIFTRight_General);
	// Within B
	_INST_FROM (0xCB08,		2, 8, 8,	"RRC B",			RRC_B, BITSHIFTRight_General);
	// Within C
	_INST_FROM (0xCB09,		2, 8, 8,	"RRC C",			RRC_C, BITSHIFTRight_General);
	// Within D
	_INST_FROM (0xCB0A,		2, 8, 8,	"RRC D",			RRC_D, BITSHIFTRight_General);
	// Within E
	_INST_FROM (0xCB0B,		2, 8, 8,	"RRC E",			RRC_E, BITSHIFTRight_General);
	// Within F
	_INST_FROM (0xCB0C,		2, 8, 8,	"RRC F",			RRC_F, BITSHIFTRight_General);
	// Within L
	_INST_FROM (0xCB0D,		2, 8, 8,	"RRC L",			RRC_L, BITSHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB0E,		2, 15, 16,	"RRC (HL)",			RRC_IndirectHL, BITSHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	_INST_FROM (0xDDCB0E,	4, 23, 23,	"RRC (IX+[#1])",	RRC_IndirectIndexIX, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RRC (IX+[#1]), A",	RRC_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RRC (IX+[#1]), B",	RRC_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RRC (IX+[#1]), C",	RRC_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RRC (IX+[#1]), D",	RRC_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RRC (IX+[#1]), E",	RRC_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RRC (IX+[#1]), H",	RRC_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RRC (IX+[#1]), L",	RRC_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RRC (IY+[#1])",	RRC_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RRC (IX+[#1]), A",	RRC_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RRC (IX+[#1]), B",	RRC_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RRC (IX+[#1]), C",	RRC_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RRC (IX+[#1]), D",	RRC_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RRC (IX+[#1]), E",	RRC_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RRC (IX+[#1]), H",	RRC_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RRC (IX+[#1]), L",	RRC_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented

	// Rotate with the carry!
	// Within A
	_INST_FROM (0x0F,		1, 4, 4,	"RRA",				RR_A, BITSHIFTRight_General);
	// Within A, but slower...
	_INST_FROM (0xCB0F,		2, 8, 8,	"RR A",				RR_ASlower, BITSHIFTRight_General);
	// Within B
	_INST_FROM (0xCB08,		2, 8, 8,	"RR B",				RR_B, BITSHIFTRight_General);
	// Within C
	_INST_FROM (0xCB09,		2, 8, 8,	"RR C",				RR_C, BITSHIFTRight_General);
	// Within D
	_INST_FROM (0xCB0A,		2, 8, 8,	"RR D",				RR_D, BITSHIFTRight_General);
	// Within E
	_INST_FROM (0xCB0B,		2, 8, 8,	"RR E",				RR_E, BITSHIFTRight_General);
	// Within F
	_INST_FROM (0xCB0C,		2, 8, 8,	"RR F",				RR_F, BITSHIFTRight_General);
	// Within L
	_INST_FROM (0xCB0D,		2, 8, 8,	"RR L",				RR_L, BITSHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB0E,		2, 15, 16,	"RR (HL)",			RR_IndirectHL, BITSHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	_INST_FROM (0xDDCB0E,	4, 23, 23,	"RR (IX+[#1])",		RR_IndirectIndexIX, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RR (IX+[#1]), A",	RR_IndirectIndexIXCopyA, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RR (IX+[#1]), B",	RR_IndirectIndexIXCopyB, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RR (IX+[#1]), C",	RR_IndirectIndexIXCopyC, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RR (IX+[#1]), D",	RR_IndirectIndexIXCopyD, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RR (IX+[#1]), E",	RR_IndirectIndexIXCopyE, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RR (IX+[#1]), H",	RR_IndirectIndexIXCopyH, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RR (IX+[#1]), L",	RR_IndirectIndexIXCopyL, BITSHIFTLeft_General);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RR (IY+[#1])",		RR_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RR (IX+[#1]), A",	RR_IndirectIndexIYCopyA, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RR (IX+[#1]), B",	RR_IndirectIndexIYCopyB, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RR (IX+[#1]), C",	RR_IndirectIndexIYCopyC, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RR (IX+[#1]), D",	RR_IndirectIndexIYCopyD, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RR (IX+[#1]), E",	RR_IndirectIndexIYCopyE, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RR (IX+[#1]), H",	RR_IndirectIndexIYCopyH, BITSHIFTLeft_General);		// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RR (IX+[#1]), L",	RR_IndirectIndexIYCopyL, BITSHIFTLeft_General);		// Undocumented

	/** The content of low nibble A is copied into the high nibble (HL),
		the content of the high nibble (HL) is copied into the low nibble (HL), 
		and the low nibble (HL) is copied into the low nibble A. */
	_INST_FROM (0xED67,		2, 18, 18,	"RRD",				RRD, BITSHIFTLeft_General);

	// Shift maintaining the bit 7!
	// Within A
	_INST_FROM (0xCB2F,		2, 8, 8,	"SRA A",			SRA_A, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB28,		2, 8, 8,	"SRA B",			SRA_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB29,		2, 8, 8,	"SRA C",			SRA_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB2A,		2, 8, 8,	"SRA D",			SRA_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB2B,		2, 8, 8,	"SRA E",			SRA_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB2C,		2, 8, 8,	"SRA H",			SRA_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB2D,		2, 8, 8,	"SRA L",			SRA_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB2E,		2, 15, 15,	"SRA (HL)",			SRA_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB2E,	4, 23, 23,	"SRA (IX+[#1])",	SRA_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB2F,	4, 23, 23,	"SRA (IX+[#1]), A",	SRA_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB28,	4, 23, 23,	"SRA (IX+[#1]), B",	SRA_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB29,	4, 23, 23,	"SRA (IX+[#1]), C",	SRA_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB2A,	4, 23, 23,	"SRA (IX+[#1]), D",	SRA_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB2B,	4, 23, 23,	"SRA (IX+[#1]), E",	SRA_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB2C,	4, 23, 23,	"SRA (IX+[#1]), H",	SRA_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB2D,	4, 23, 23,	"SRA (IX+[#1]), L",	SRA_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB2E,	4, 23, 23,	"SRA (IY+[#1])",	SRA_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB2F,	4, 23, 23,	"SRA (IY+[#1]), A",	SRA_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB28,	4, 23, 23,	"SRA (IY+[#1]), B",	SRA_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB29,	4, 23, 23,	"SRA (IY+[#1]), C",	SRA_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB2A,	4, 23, 23,	"SRA (IY+[#1]), D",	SRA_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB2B,	4, 23, 23,	"SRA (IY+[#1]), E",	SRA_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB2C,	4, 23, 23,	"SRA (IY+[#1]), H",	SRA_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB2D,	4, 23, 23,	"SRA (IY+[#1]), L",	SRA_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented

	// Shift introducing a 0 in the bit 7!
	// Within A
	_INST_FROM (0xCB37,		2, 8, 8,	"SRL A",			SRL_A, BITSHIFTLeft_General);
	// Within B
	_INST_FROM (0xCB30,		2, 8, 8,	"SRL B",			SRL_B, BITSHIFTLeft_General);
	// Within C
	_INST_FROM (0xCB31,		2, 8, 8,	"SRL C",			SRL_C, BITSHIFTLeft_General);
	// Within D
	_INST_FROM (0xCB32,		2, 8, 8,	"SRL D",			SRL_D, BITSHIFTLeft_General);
	// Within E
	_INST_FROM (0xCB33,		2, 8, 8,	"SRL E",			SRL_E, BITSHIFTLeft_General);
	// Within F
	_INST_FROM (0xCB34,		2, 8, 8,	"SRL H",			SRL_H, BITSHIFTLeft_General);
	// Within L
	_INST_FROM (0xCB35,		2, 8, 8,	"SRL L",			SRL_L, BITSHIFTLeft_General);
	// Within (HL)
	_INST_FROM (0xCB36,		2, 15, 15,	"SRL (HL)",			SRL_IndirectHL, BITSHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB36,	4, 23, 23,	"SRL (IX+[#1])",	SRL_IndirectIndexIX, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB37,	4, 23, 23,	"SRL (IX+[#1]), A",	SRL_IndirectIndexIXCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB30,	4, 23, 23,	"SRL (IX+[#1]), B",	SRL_IndirectIndexIXCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB31,	4, 23, 23,	"SRL (IX+[#1]), C",	SRL_IndirectIndexIXCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB32,	4, 23, 23,	"SRL (IX+[#1]), D",	SRL_IndirectIndexIXCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB33,	4, 23, 23,	"SRL (IX+[#1]), E",	SRL_IndirectIndexIXCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB34,	4, 23, 23,	"SRL (IX+[#1]), H",	SRL_IndirectIndexIXCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xDDCB35,	4, 23, 23,	"SRL (IX+[#1]), L",	SRL_IndirectIndexIXCopyL, BITSHIFTLeft_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB36,	4, 23, 23,	"SRL (IY+[#1])",	SRL_IndirectIndexIY, BITSHIFTLeft_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB37,	4, 23, 23,	"SRL (IY+[#1]), A",	SRL_IndirectIndexIYCopyA, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB30,	4, 23, 23,	"SRL (IY+[#1]), B",	SRL_IndirectIndexIYCopyB, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB31,	4, 23, 23,	"SRL (IY+[#1]), C",	SRL_IndirectIndexIYCopyC, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB32,	4, 23, 23,	"SRL (IY+[#1]), D",	SRL_IndirectIndexIYCopyD, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB33,	4, 23, 23,	"SRL (IY+[#1]), E",	SRL_IndirectIndexIYCopyE, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB34,	4, 23, 23,	"SRL (IY+[#1]), H",	SRL_IndirectIndexIYCopyH, BITSHIFTLeft_General);	// Undocumented
	_INST_FROM (0xFDCB35,	4, 23, 23,	"SRL (IY+[#1]), L",	SRL_IndirectIndexIYCopyL, BITSHIFTLeft_General);	// Undocumented
}

#endif
  
// End of the file
/*@}*/
