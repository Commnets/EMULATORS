/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ROTATERIGHT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of ROTATERIGHT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ROTATERIGHTINSTRUCTIONS__
#define __FZX80_ROTATERIGHTINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class ROTATERight_General : public Instruction
	{
		public:
		ROTATERight_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** A register. \n
			When crr = true		=>	bit 7 <- CF, CF <- bit 0.
			When crr = false	=>	CF <- bit 0, bit 7 <- bit 0. */
		inline bool executeWith (MCHEmul::Register& r, bool crr);
		/** A memory address. crr like above. */
		inline bool executeWith (const MCHEmul::Address& a, bool crr);
		/** A memory address and the result is left in a regiter. crr like above. */
		inline bool executeWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr);

		private:
		/** A common way to affect the flags for all these instructions. */
		inline void affectFlagsWith (const MCHEmul::UByte& v, bool c);
	};

	// ---
	inline bool ROTATERight_General::executeWith (MCHEmul::Register& r, bool crr)
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
	inline bool ROTATERight_General::executeWith (const MCHEmul::Address& a, bool crr)
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
	inline bool ROTATERight_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.bit (7);
		v.shiftRightC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
		memory () -> set (a, v); // Set back the value...
		r.set ({ v }); // And store it in the register...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline void ROTATERight_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
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

	/** The ROTATERIGHT instruction, when managing indexes, creates its code in another way. */
	class ROTATERight_Index : public ROTATERight_General
	{
		public:
		ROTATERight_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: ROTATERight_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Rotate with bit 7!
	// Within A
	_INST_FROM (0x0F,		1, 4, 4,	"RRCA",				RRC_A, ROTATERight_General);
	// Within A, but slower...
	_INST_FROM (0xCB0F,		2, 8, 8,	"RRC A",			RRC_ASlower, ROTATERight_General);
	// Within B
	_INST_FROM (0xCB08,		2, 8, 8,	"RRC B",			RRC_B, ROTATERight_General);
	// Within C
	_INST_FROM (0xCB09,		2, 8, 8,	"RRC C",			RRC_C, ROTATERight_General);
	// Within D
	_INST_FROM (0xCB0A,		2, 8, 8,	"RRC D",			RRC_D, ROTATERight_General);
	// Within E
	_INST_FROM (0xCB0B,		2, 8, 8,	"RRC E",			RRC_E, ROTATERight_General);
	// Within F
	_INST_FROM (0xCB0C,		2, 8, 8,	"RRC F",			RRC_F, ROTATERight_General);
	// Within L
	_INST_FROM (0xCB0D,		2, 8, 8,	"RRC L",			RRC_L, ROTATERight_General);
	// Within (HL)
	_INST_FROM (0xCB0E,		2, 15, 16,	"RRC (HL)",			RRC_IndirectHL, ROTATERight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	_INST_FROM (0xDDCB0E,	4, 23, 23,	"RRC (IX+[#1])",	RRC_IndirectIndexIX, ROTATERight_Index);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xDDCB0F,	4, 23, 23,	"RRC (IX+[#1]),A",	RRC_IndirectIndexIXCopyA, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB08,	4, 23, 23,	"RRC (IX+[#1]),B",	RRC_IndirectIndexIXCopyB, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB09,	4, 23, 23,	"RRC (IX+[#1]),C",	RRC_IndirectIndexIXCopyC, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB0A,	4, 23, 23,	"RRC (IX+[#1]),D",	RRC_IndirectIndexIXCopyD, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB0B,	4, 23, 23,	"RRC (IX+[#1]),E",	RRC_IndirectIndexIXCopyE, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB0C,	4, 23, 23,	"RRC (IX+[#1]),H",	RRC_IndirectIndexIXCopyH, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB0D,	4, 23, 23,	"RRC (IX+[#1]),L",	RRC_IndirectIndexIXCopyL, ROTATERight_Index);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB0E,	4, 23, 23,	"RRC (IY+[#1])",	RRC_IndirectIndexIY, ROTATERight_Index);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB0F,	4, 23, 23,	"RRC (IY+[#1]),A",	RRC_IndirectIndexIYCopyA, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB08,	4, 23, 23,	"RRC (IY+[#1]),B",	RRC_IndirectIndexIYCopyB, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB09,	4, 23, 23,	"RRC (IY+[#1]),C",	RRC_IndirectIndexIYCopyC, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB0A,	4, 23, 23,	"RRC (IY+[#1]),D",	RRC_IndirectIndexIYCopyD, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB0B,	4, 23, 23,	"RRC (IY+[#1]),E",	RRC_IndirectIndexIYCopyE, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB0C,	4, 23, 23,	"RRC (IY+[#1]),H",	RRC_IndirectIndexIYCopyH, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB0D,	4, 23, 23,	"RRC (IY+[#1]),L",	RRC_IndirectIndexIYCopyL, ROTATERight_Index);	// Undocumented

	// Rotate with the carry!
	// Within A
	_INST_FROM (0x1F,		1, 4, 4,	"RRA",				RR_A, ROTATERight_General);
	// Within A, but slower...
	_INST_FROM (0xCB1F,		2, 8, 8,	"RR A",				RR_ASlower, ROTATERight_General);
	// Within B
	_INST_FROM (0xCB18,		2, 8, 8,	"RR B",				RR_B, ROTATERight_General);
	// Within C
	_INST_FROM (0xCB19,		2, 8, 8,	"RR C",				RR_C, ROTATERight_General);
	// Within D
	_INST_FROM (0xCB1A,		2, 8, 8,	"RR D",				RR_D, ROTATERight_General);
	// Within E
	_INST_FROM (0xCB1B,		2, 8, 8,	"RR E",				RR_E, ROTATERight_General);
	// Within F
	_INST_FROM (0xCB1C,		2, 8, 8,	"RR F",				RR_F, ROTATERight_General);
	// Within L
	_INST_FROM (0xCB1D,		2, 8, 8,	"RR L",				RR_L, ROTATERight_General);
	// Within (HL)
	_INST_FROM (0xCB1E,		2, 15, 16,	"RR (HL)",			RR_IndirectHL, ROTATERight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	_INST_FROM (0xDDCB1E,	4, 23, 23,	"RR (IX+[#1])",		RR_IndirectIndexIX, ROTATERight_Index);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xDDCB1F,	4, 23, 23,	"RR (IX+[#1]),A",	RR_IndirectIndexIXCopyA, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB18,	4, 23, 23,	"RR (IX+[#1]),B",	RR_IndirectIndexIXCopyB, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB19,	4, 23, 23,	"RR (IX+[#1]),C",	RR_IndirectIndexIXCopyC, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB1A,	4, 23, 23,	"RR (IX+[#1]),D",	RR_IndirectIndexIXCopyD, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB1B,	4, 23, 23,	"RR (IX+[#1]),E",	RR_IndirectIndexIXCopyE, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB1C,	4, 23, 23,	"RR (IX+[#1]),H",	RR_IndirectIndexIXCopyH, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xDDCB1D,	4, 23, 23,	"RR (IX+[#1]),L",	RR_IndirectIndexIXCopyL, ROTATERight_Index);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB1E,	4, 23, 23,	"RR (IY+[#1])",		RR_IndirectIndexIY, ROTATERight_Index);
	// Within (IX + d) and the result copied into a register
	// All of them are non documented
	_INST_FROM (0xFDCB1F,	4, 23, 23,	"RR (IY+[#1]),A",	RR_IndirectIndexIYCopyA, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB18,	4, 23, 23,	"RR (IY+[#1]),B",	RR_IndirectIndexIYCopyB, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB19,	4, 23, 23,	"RR (IY+[#1]),C",	RR_IndirectIndexIYCopyC, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB1A,	4, 23, 23,	"RR (IY+[#1]),D",	RR_IndirectIndexIYCopyD, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB1B,	4, 23, 23,	"RR (IY+[#1]),E",	RR_IndirectIndexIYCopyE, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB1C,	4, 23, 23,	"RR (IY+[#1]),H",	RR_IndirectIndexIYCopyH, ROTATERight_Index);	// Undocumented
	_INST_FROM (0xFDCB1D,	4, 23, 23,	"RR (IY+[#1]),L",	RR_IndirectIndexIYCopyL, ROTATERight_Index);	// Undocumented

	/** The content of low nibble A is copied into the high nibble (HL),
		the content of the high nibble (HL) is copied into the low nibble (HL), 
		and the low nibble (HL) is copied into the low nibble A. */
	_INST_FROM (0xED67,		2, 18, 18,	"RRD",				RRD, ROTATERight_General);
}

#endif
  
// End of the file
/*@}*/
