/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ROTATELEFT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of ROTATELEFT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ROTATELEFTINSTRUCTIONS__
#define __FZX80_ROTATELEFTINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class ROTATELeft_General : public Instruction
	{
		public:
		ROTATELeft_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** A register. \n
			When crr = true		=>	bit 0 <- CF, CF <- bit 7.
			When crr = false	=>	CF <- bit 7, bit 0 <- bit 7. */
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
	inline bool ROTATELeft_General::executeWith (MCHEmul::Register& r, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = r.values ()[0];
		bool c = v.bit (7);
		v.shiftLeftC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
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
	inline bool ROTATELeft_General::executeWith (const MCHEmul::Address& a, bool crr)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.bit (7);
		v.shiftLeftC (crr ? st.bitStatus (CZ80::_CARRYFLAG) : c);
		memory () -> set (a, v); // Set back the value...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool ROTATELeft_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
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
	inline void ROTATELeft_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
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

	/** The ROTATELEFT instruction, when managing indexes, creates its code in another way. */
	class ROTATELeft_Index : public ROTATELeft_General
	{
		public:
		ROTATELeft_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: ROTATELeft_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Rotate with bit 7...
	// Within A
	_INST_FROM (0x07,		1, 4, 4,	"RLCA",				RLC_A, ROTATELeft_General);
	// Within A, but slower...
	_INST_FROM (0xCB07,		2, 8, 8,	"RLC A",			RLC_ASlower, ROTATELeft_General);
	// Within B
	_INST_FROM (0xCB00,		2, 8, 8,	"RLC B",			RLC_B, ROTATELeft_General);
	// Within C
	_INST_FROM (0xCB01,		2, 8, 8,	"RLC C",			RLC_C, ROTATELeft_General);
	// Within D
	_INST_FROM (0xCB02,		2, 8, 8,	"RLC D",			RLC_D, ROTATELeft_General);
	// Within E
	_INST_FROM (0xCB03,		2, 8, 8,	"RLC E",			RLC_E, ROTATELeft_General);
	// Within F
	_INST_FROM (0xCB04,		2, 8, 8,	"RLC H",			RLC_H, ROTATELeft_General);
	// Within L
	_INST_FROM (0xCB05,		2, 8, 8,	"RLC L",			RLC_L, ROTATELeft_General);
	// Within (HL)
	_INST_FROM (0xCB06,		2, 15, 15,	"RLC (HL)",			RLC_IndirectHL, ROTATELeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB06,	4, 23, 23,	"RLC (IX+[#1])",	RLC_IndirectIndexIX, ROTATELeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB07,	4, 23, 23,	"RLC (IX+[#1]),A",	RLC_IndirectIndexIXCopyA, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB00,	4, 23, 23,	"RLC (IX+[#1]),B",	RLC_IndirectIndexIXCopyB, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB01,	4, 23, 23,	"RLC (IX+[#1]),C",	RLC_IndirectIndexIXCopyC, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB02,	4, 23, 23,	"RLC (IX+[#1]),D",	RLC_IndirectIndexIXCopyD, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB03,	4, 23, 23,	"RLC (IX+[#1]),E",	RLC_IndirectIndexIXCopyE, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB04,	4, 23, 23,	"RLC (IX+[#1]),H",	RLC_IndirectIndexIXCopyH, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xDDCB05,	4, 23, 23,	"RLC (IX+[#1]),L",	RLC_IndirectIndexIXCopyL, ROTATELeft_Index);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB06,	4, 23, 23,	"RLC (IY+[#1])",	RLC_IndirectIndexIY, ROTATELeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB07,	4, 23, 23,	"RLC (IY+[#1]),A",	RLC_IndirectIndexIYCopyA, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB00,	4, 23, 23,	"RLC (IY+[#1]),B",	RLC_IndirectIndexIYCopyB, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB01,	4, 23, 23,	"RLC (IY+[#1]),C",	RLC_IndirectIndexIYCopyC, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB02,	4, 23, 23,	"RLC (IY+[#1]),D",	RLC_IndirectIndexIYCopyD, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB03,	4, 23, 23,	"RLC (IY+[#1]),E",	RLC_IndirectIndexIYCopyE, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB04,	4, 23, 23,	"RLC (IY+[#1]),H",	RLC_IndirectIndexIYCopyH, ROTATELeft_Index);	// Undocumented
	_INST_FROM (0xFDCB05,	4, 23, 23,	"RLC (IY+[#1]),L",	RLC_IndirectIndexIYCopyL, ROTATELeft_Index);	// Undocumented

	// Rotate with carry!
	// Within A
	_INST_FROM (0x17,		1, 4, 4,	"RLA",				RL_A, ROTATELeft_General);
	// Within A, but slower...
	_INST_FROM (0xCB17,		2, 8, 8,	"RL A",				RL_ASlower, ROTATELeft_General);
	// Within B
	_INST_FROM (0xCB10,		2, 8, 8,	"RL B",				RL_B, ROTATELeft_General);
	// Within C
	_INST_FROM (0xCB11,		2, 8, 8,	"RL C",				RL_C, ROTATELeft_General);
	// Within D
	_INST_FROM (0xCB12,		2, 8, 8,	"RL D",				RL_D, ROTATELeft_General);
	// Within E
	_INST_FROM (0xCB13,		2, 8, 8,	"RL E",				RL_E, ROTATELeft_General);
	// Within F
	_INST_FROM (0xCB14,		2, 8, 8,	"RL H",				RL_H, ROTATELeft_General);
	// Within L
	_INST_FROM (0xCB15,		2, 8, 8,	"RL L",				RL_L, ROTATELeft_General);
	// Within (HL)
	_INST_FROM (0xCB16,		2, 15, 15,	"RL (HL)",			RL_IndirectHL, ROTATELeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB16,	4, 23, 23,	"RL (IX+[#1])",		RL_IndirectIndexIX, ROTATELeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB17,	4, 23, 23,	"RL (IX+[#1]),A",	RL_IndirectIndexIXCopyA, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB10,	4, 23, 23,	"RL (IX+[#1]),B",	RL_IndirectIndexIXCopyB, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB11,	4, 23, 23,	"RL (IX+[#1]),C",	RL_IndirectIndexIXCopyC, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB12,	4, 23, 23,	"RL (IX+[#1]),D",	RL_IndirectIndexIXCopyD, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB13,	4, 23, 23,	"RL (IX+[#1]),E",	RL_IndirectIndexIXCopyE, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB14,	4, 23, 23,	"RL (IX+[#1]),H",	RL_IndirectIndexIXCopyH, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xDDCB15,	4, 23, 23,	"RL (IX+[#1]),L",	RL_IndirectIndexIXCopyL, ROTATELeft_Index);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB16,	4, 23, 23,	"RL (IY+[#1])",		RL_IndirectIndexIY, ROTATELeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB17,	4, 23, 23,	"RL (IY+[#1]),A",	RL_IndirectIndexIYCopyA, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB10,	4, 23, 23,	"RL (IY+[#1]),B",	RL_IndirectIndexIYCopyB, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB11,	4, 23, 23,	"RL (IY+[#1]),C",	RL_IndirectIndexIYCopyC, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB12,	4, 23, 23,	"RL (IY+[#1]),D",	RL_IndirectIndexIYCopyD, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB13,	4, 23, 23,	"RL (IY+[#1]),E",	RL_IndirectIndexIYCopyE, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB14,	4, 23, 23,	"RL (IY+[#1]),H",	RL_IndirectIndexIYCopyH, ROTATELeft_Index);		// Undocumented
	_INST_FROM (0xFDCB15,	4, 23, 23,	"RL (IY+[#1]),L",	RL_IndirectIndexIYCopyL, ROTATELeft_Index);		// Undocumented

	/** The content of low nibble (HL) is copied into the high one,
		the content of the high nibble (HL) is copied into the low nibble A, 
		and the low nibble A is copied into the low nibble (HL). */
	_INST_FROM (0xED6F,		2, 18, 18,	"RLD",				RLD, ROTATELeft_General);
}

#endif
  
// End of the file
/*@}*/
