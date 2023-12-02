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

#include <FZ80/Instruction.hpp>

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
		inline bool executeRotateWith (MCHEmul::Register& r, bool crr);
		/** A memory address. crr like above. */
		inline bool executeRotateWith (const MCHEmul::Address& a, bool crr);
		/** A memory address and the result is left in a regiter. crr like above. */
		inline bool executeRotateWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr);

		// Shift
		/** A register. \n
			The behavior is a little bit different that in shift left. \n
			When crr = true		=>	bit 7 <- bit 7, CF <- bit 0.
			When crr = false	=>	bit 7 <- 0, CF <- bit 0. */
		inline bool executeShiftWith (MCHEmul::Register& r, bool crr);
		/** A memory address. crr like above. */
		inline bool executeShiftWith (const MCHEmul::Address& a, bool crr);
		/** A memory address and the result is left in a regiter. crr like above. */
		inline bool executeShiftWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr);

		private:
		/** A common way to affect the flags for all these instructions. */
		inline void affectFlagsWith (const MCHEmul::UByte& v, bool c);
	};

	// ---
	inline bool BITSHIFTRight_General::executeRotateWith (MCHEmul::Register& r, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = r.values ()[0];
		bool c = v.bit (7);
		v.shiftRightC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
		r.set ({ v });

		// When the register affect is A and is the documented version, 
		// the flags affected are different (some less) than in non documented versions...
		if (&r == &registerA () && parameters ().size () == 1)
		{
			// How the flags are accepted...
			st.setBitStatus (CZ80::_CARRYFLAG, c);
			st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always
			// Flag Parity/Overflow not affected...
			st.setBitStatus (CZ80::_BIT3FLAG, v.bit (3)); // Undocumented...
			st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
			st.setBitStatus (CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...
			// Flag Negative not affected...
			// Flag Sign not affected...
		}
		else
			affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool BITSHIFTRight_General::executeRotateWith (const MCHEmul::Address& a, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.bit (7);
		v.shiftRightC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
		memory () -> set (a, v); // Set back the value...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool BITSHIFTRight_General::executeRotateWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.bit (7);
		v.shiftLeftC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
		memory () -> set (a, v); // Set back the value...
		r.set ({ v }); // And store it in the register...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool BITSHIFTRight_General::executeShiftWith (MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = r.values ()[0];
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		r.set ({ v });

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool BITSHIFTRight_General::executeShiftWith (const MCHEmul::Address& a, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		memory () -> set (a, v); // Set back the value...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool BITSHIFTRight_General::executeShiftWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		memory () -> set (a, v); // Set back the value...
		r.set ({ v }); // And store it in the register...

		affectFlagsWith (v, c);

		return (true);
	}

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
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RRC (IX+[#1]),A",	RRC_IndirectIndexIXCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RRC (IX+[#1]),B",	RRC_IndirectIndexIXCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RRC (IX+[#1]),C",	RRC_IndirectIndexIXCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RRC (IX+[#1]),D",	RRC_IndirectIndexIXCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RRC (IX+[#1]),E",	RRC_IndirectIndexIXCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RRC (IX+[#1]),H",	RRC_IndirectIndexIXCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RRC (IX+[#1]),L",	RRC_IndirectIndexIXCopyL, BITSHIFTRight_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RRC (IY+[#1])",	RRC_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RRC (IX+[#1]),A",	RRC_IndirectIndexIYCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RRC (IX+[#1]),B",	RRC_IndirectIndexIYCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RRC (IX+[#1]),C",	RRC_IndirectIndexIYCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RRC (IX+[#1]),D",	RRC_IndirectIndexIYCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RRC (IX+[#1]),E",	RRC_IndirectIndexIYCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RRC (IX+[#1]),H",	RRC_IndirectIndexIYCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RRC (IX+[#1]),L",	RRC_IndirectIndexIYCopyL, BITSHIFTRight_General);	// Undocumented

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
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RR (IX+[#1]),A",	RR_IndirectIndexIXCopyA, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RR (IX+[#1]),B",	RR_IndirectIndexIXCopyB, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RR (IX+[#1]),C",	RR_IndirectIndexIXCopyC, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RR (IX+[#1]),D",	RR_IndirectIndexIXCopyD, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RR (IX+[#1]),E",	RR_IndirectIndexIXCopyE, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RR (IX+[#1]),H",	RR_IndirectIndexIXCopyH, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RR (IX+[#1]),L",	RR_IndirectIndexIXCopyL, BITSHIFTRight_General);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RR (IY+[#1])",		RR_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RR (IX+[#1]),A",	RR_IndirectIndexIYCopyA, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RR (IX+[#1]),B",	RR_IndirectIndexIYCopyB, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RR (IX+[#1]),C",	RR_IndirectIndexIYCopyC, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RR (IX+[#1]),D",	RR_IndirectIndexIYCopyD, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RR (IX+[#1]),E",	RR_IndirectIndexIYCopyE, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RR (IX+[#1]),H",	RR_IndirectIndexIYCopyH, BITSHIFTRight_General);		// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RR (IX+[#1]),L",	RR_IndirectIndexIYCopyL, BITSHIFTRight_General);		// Undocumented

	/** The content of low nibble A is copied into the high nibble (HL),
		the content of the high nibble (HL) is copied into the low nibble (HL), 
		and the low nibble (HL) is copied into the low nibble A. */
	_INST_FROM (0xED67,		2, 18, 18,	"RRD",				RRD, BITSHIFTRight_General);

	// Shift maintaining the bit 7!
	// Within A
	_INST_FROM (0xCB2F,		2, 8, 8,	"SRA A",			SRA_A, BITSHIFTRight_General);
	// Within B
	_INST_FROM (0xCB28,		2, 8, 8,	"SRA B",			SRA_B, BITSHIFTRight_General);
	// Within C
	_INST_FROM (0xCB29,		2, 8, 8,	"SRA C",			SRA_C, BITSHIFTRight_General);
	// Within D
	_INST_FROM (0xCB2A,		2, 8, 8,	"SRA D",			SRA_D, BITSHIFTRight_General);
	// Within E
	_INST_FROM (0xCB2B,		2, 8, 8,	"SRA E",			SRA_E, BITSHIFTRight_General);
	// Within F
	_INST_FROM (0xCB2C,		2, 8, 8,	"SRA H",			SRA_H, BITSHIFTRight_General);
	// Within L
	_INST_FROM (0xCB2D,		2, 8, 8,	"SRA L",			SRA_L, BITSHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB2E,		2, 15, 15,	"SRA (HL)",			SRA_IndirectHL, BITSHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB2E,	4, 23, 23,	"SRA (IX+[#1])",	SRA_IndirectIndexIX, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB2F,	4, 23, 23,	"SRA (IX+[#1]),A",	SRA_IndirectIndexIXCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB28,	4, 23, 23,	"SRA (IX+[#1]),B",	SRA_IndirectIndexIXCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB29,	4, 23, 23,	"SRA (IX+[#1]),C",	SRA_IndirectIndexIXCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB2A,	4, 23, 23,	"SRA (IX+[#1]),D",	SRA_IndirectIndexIXCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB2B,	4, 23, 23,	"SRA (IX+[#1]),E",	SRA_IndirectIndexIXCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB2C,	4, 23, 23,	"SRA (IX+[#1]),H",	SRA_IndirectIndexIXCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB2D,	4, 23, 23,	"SRA (IX+[#1]),L",	SRA_IndirectIndexIXCopyL, BITSHIFTRight_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB2E,	4, 23, 23,	"SRA (IY+[#1])",	SRA_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB2F,	4, 23, 23,	"SRA (IY+[#1]),A",	SRA_IndirectIndexIYCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB28,	4, 23, 23,	"SRA (IY+[#1]),B",	SRA_IndirectIndexIYCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB29,	4, 23, 23,	"SRA (IY+[#1]),C",	SRA_IndirectIndexIYCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB2A,	4, 23, 23,	"SRA (IY+[#1]),D",	SRA_IndirectIndexIYCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB2B,	4, 23, 23,	"SRA (IY+[#1]),E",	SRA_IndirectIndexIYCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB2C,	4, 23, 23,	"SRA (IY+[#1]),H",	SRA_IndirectIndexIYCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB2D,	4, 23, 23,	"SRA (IY+[#1]),L",	SRA_IndirectIndexIYCopyL, BITSHIFTRight_General);	// Undocumented

	// Shift introducing a 0 in the bit 7!
	// Within A
	_INST_FROM (0xCB3F,		2, 8, 8,	"SRL A",			SRL_A, BITSHIFTRight_General);
	// Within B
	_INST_FROM (0xCB38,		2, 8, 8,	"SRL B",			SRL_B, BITSHIFTRight_General);
	// Within C
	_INST_FROM (0xCB39,		2, 8, 8,	"SRL C",			SRL_C, BITSHIFTRight_General);
	// Within D
	_INST_FROM (0xCB3A,		2, 8, 8,	"SRL D",			SRL_D, BITSHIFTRight_General);
	// Within E
	_INST_FROM (0xCB3B,		2, 8, 8,	"SRL E",			SRL_E, BITSHIFTRight_General);
	// Within F
	_INST_FROM (0xCB3C,		2, 8, 8,	"SRL H",			SRL_H, BITSHIFTRight_General);
	// Within L
	_INST_FROM (0xCB3D,		2, 8, 8,	"SRL L",			SRL_L, BITSHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB3E,		2, 15, 15,	"SRL (HL)",			SRL_IndirectHL, BITSHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB3E,	4, 23, 23,	"SRL (IX+[#1])",	SRL_IndirectIndexIX, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB3F,	4, 23, 23,	"SRL (IX+[#1]),A",	SRL_IndirectIndexIXCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB38,	4, 23, 23,	"SRL (IX+[#1]),B",	SRL_IndirectIndexIXCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB39,	4, 23, 23,	"SRL (IX+[#1]),C",	SRL_IndirectIndexIXCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB3A,	4, 23, 23,	"SRL (IX+[#1]),D",	SRL_IndirectIndexIXCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB3B,	4, 23, 23,	"SRL (IX+[#1]),E",	SRL_IndirectIndexIXCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB3C,	4, 23, 23,	"SRL (IX+[#1]),H",	SRL_IndirectIndexIXCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xDDCB3D,	4, 23, 23,	"SRL (IX+[#1]),L",	SRL_IndirectIndexIXCopyL, BITSHIFTRight_General);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB3E,	4, 23, 23,	"SRL (IY+[#1])",	SRL_IndirectIndexIY, BITSHIFTRight_General);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB3F,	4, 23, 23,	"SRL (IY+[#1]),A",	SRL_IndirectIndexIYCopyA, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB38,	4, 23, 23,	"SRL (IY+[#1]),B",	SRL_IndirectIndexIYCopyB, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB39,	4, 23, 23,	"SRL (IY+[#1]),C",	SRL_IndirectIndexIYCopyC, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB3A,	4, 23, 23,	"SRL (IY+[#1]),D",	SRL_IndirectIndexIYCopyD, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB3A,	4, 23, 23,	"SRL (IY+[#1]),E",	SRL_IndirectIndexIYCopyE, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB3B,	4, 23, 23,	"SRL (IY+[#1]),H",	SRL_IndirectIndexIYCopyH, BITSHIFTRight_General);	// Undocumented
	_INST_FROM (0xFDCB3C,	4, 23, 23,	"SRL (IY+[#1]),L",	SRL_IndirectIndexIYCopyL, BITSHIFTRight_General);	// Undocumented
}

#endif
  
// End of the file
/*@}*/
