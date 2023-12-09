/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: SHIFTRIGHT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of SHIFTRIGHT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_SHIFTRIGHTINSTRUCTIONS__
#define __FZX80_SHIFTRIGHTINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class SHIFTRight_General : public Instruction
	{
		public:
		SHIFTRight_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** A register. \n
			The behavior is a little bit different that in shift left. \n
			When crr = true		=>	bit 7 <- bit 7, CF <- bit 0.
			When crr = false	=>	bit 7 <- 0, CF <- bit 0. */
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
	inline bool SHIFTRight_General::executeWith (MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = r.values ()[0];
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		r.set ({ v });

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool SHIFTRight_General::executeWith (const MCHEmul::Address& a, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		memory () -> set (a, v); // Set back the value...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool SHIFTRight_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftRightC (crr ? v.bit (7) : false);
		memory () -> set (a, v); // Set back the value...
		r.set ({ v }); // And store it in the register...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline void SHIFTRight_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
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

	/** The SHIFTRIGHT instruction, when managing indexes, creates its code in another way. */
	class SHIFTRight_Index : public SHIFTRight_General
	{
		public:
		SHIFTRight_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: SHIFTRight_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Shift maintaining the bit 7!
	// Within A
	_INST_FROM (0xCB2F,		2, 8, 8,	"SRA A",			SRA_A, SHIFTRight_General);
	// Within B
	_INST_FROM (0xCB28,		2, 8, 8,	"SRA B",			SRA_B, SHIFTRight_General);
	// Within C
	_INST_FROM (0xCB29,		2, 8, 8,	"SRA C",			SRA_C, SHIFTRight_General);
	// Within D
	_INST_FROM (0xCB2A,		2, 8, 8,	"SRA D",			SRA_D, SHIFTRight_General);
	// Within E
	_INST_FROM (0xCB2B,		2, 8, 8,	"SRA E",			SRA_E, SHIFTRight_General);
	// Within F
	_INST_FROM (0xCB2C,		2, 8, 8,	"SRA H",			SRA_H, SHIFTRight_General);
	// Within L
	_INST_FROM (0xCB2D,		2, 8, 8,	"SRA L",			SRA_L, SHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB2E,		2, 15, 15,	"SRA (HL)",			SRA_IndirectHL, SHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB2E,	4, 23, 23,	"SRA (IX+[#1])",	SRA_IndirectIndexIX, SHIFTRight_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB2F,	4, 23, 23,	"SRA (IX+[#1]),A",	SRA_IndirectIndexIXCopyA, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB28,	4, 23, 23,	"SRA (IX+[#1]),B",	SRA_IndirectIndexIXCopyB, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB29,	4, 23, 23,	"SRA (IX+[#1]),C",	SRA_IndirectIndexIXCopyC, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB2A,	4, 23, 23,	"SRA (IX+[#1]),D",	SRA_IndirectIndexIXCopyD, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB2B,	4, 23, 23,	"SRA (IX+[#1]),E",	SRA_IndirectIndexIXCopyE, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB2C,	4, 23, 23,	"SRA (IX+[#1]),H",	SRA_IndirectIndexIXCopyH, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB2D,	4, 23, 23,	"SRA (IX+[#1]),L",	SRA_IndirectIndexIXCopyL, SHIFTRight_Index);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB2E,	4, 23, 23,	"SRA (IY+[#1])",	SRA_IndirectIndexIY, SHIFTRight_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB2F,	4, 23, 23,	"SRA (IY+[#1]),A",	SRA_IndirectIndexIYCopyA, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB28,	4, 23, 23,	"SRA (IY+[#1]),B",	SRA_IndirectIndexIYCopyB, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB29,	4, 23, 23,	"SRA (IY+[#1]),C",	SRA_IndirectIndexIYCopyC, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB2A,	4, 23, 23,	"SRA (IY+[#1]),D",	SRA_IndirectIndexIYCopyD, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB2B,	4, 23, 23,	"SRA (IY+[#1]),E",	SRA_IndirectIndexIYCopyE, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB2C,	4, 23, 23,	"SRA (IY+[#1]),H",	SRA_IndirectIndexIYCopyH, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB2D,	4, 23, 23,	"SRA (IY+[#1]),L",	SRA_IndirectIndexIYCopyL, SHIFTRight_Index);	// Undocumented

	// Shift introducing a 0 in the bit 7!
	// Within A
	_INST_FROM (0xCB3F,		2, 8, 8,	"SRL A",			SRL_A, SHIFTRight_General);
	// Within B
	_INST_FROM (0xCB38,		2, 8, 8,	"SRL B",			SRL_B, SHIFTRight_General);
	// Within C
	_INST_FROM (0xCB39,		2, 8, 8,	"SRL C",			SRL_C, SHIFTRight_General);
	// Within D
	_INST_FROM (0xCB3A,		2, 8, 8,	"SRL D",			SRL_D, SHIFTRight_General);
	// Within E
	_INST_FROM (0xCB3B,		2, 8, 8,	"SRL E",			SRL_E, SHIFTRight_General);
	// Within F
	_INST_FROM (0xCB3C,		2, 8, 8,	"SRL H",			SRL_H, SHIFTRight_General);
	// Within L
	_INST_FROM (0xCB3D,		2, 8, 8,	"SRL L",			SRL_L, SHIFTRight_General);
	// Within (HL)
	_INST_FROM (0xCB3E,		2, 15, 15,	"SRL (HL)",			SRL_IndirectHL, SHIFTRight_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INST_FROM (0xDDCB3E,	4, 23, 23,	"SRL (IX+[#1])",	SRL_IndirectIndexIX, SHIFTRight_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xDDCB3F,	4, 23, 23,	"SRL (IX+[#1]),A",	SRL_IndirectIndexIXCopyA, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB38,	4, 23, 23,	"SRL (IX+[#1]),B",	SRL_IndirectIndexIXCopyB, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB39,	4, 23, 23,	"SRL (IX+[#1]),C",	SRL_IndirectIndexIXCopyC, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB3A,	4, 23, 23,	"SRL (IX+[#1]),D",	SRL_IndirectIndexIXCopyD, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB3B,	4, 23, 23,	"SRL (IX+[#1]),E",	SRL_IndirectIndexIXCopyE, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB3C,	4, 23, 23,	"SRL (IX+[#1]),H",	SRL_IndirectIndexIXCopyH, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xDDCB3D,	4, 23, 23,	"SRL (IX+[#1]),L",	SRL_IndirectIndexIXCopyL, SHIFTRight_Index);	// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INST_FROM (0xFDCB3E,	4, 23, 23,	"SRL (IY+[#1])",	SRL_IndirectIndexIY, SHIFTRight_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INST_FROM (0xFDCB3F,	4, 23, 23,	"SRL (IY+[#1]),A",	SRL_IndirectIndexIYCopyA, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB38,	4, 23, 23,	"SRL (IY+[#1]),B",	SRL_IndirectIndexIYCopyB, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB39,	4, 23, 23,	"SRL (IY+[#1]),C",	SRL_IndirectIndexIYCopyC, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB3A,	4, 23, 23,	"SRL (IY+[#1]),D",	SRL_IndirectIndexIYCopyD, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB3A,	4, 23, 23,	"SRL (IY+[#1]),E",	SRL_IndirectIndexIYCopyE, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB3B,	4, 23, 23,	"SRL (IY+[#1]),H",	SRL_IndirectIndexIYCopyH, SHIFTRight_Index);	// Undocumented
	_INST_FROM (0xFDCB3C,	4, 23, 23,	"SRL (IY+[#1]),L",	SRL_IndirectIndexIYCopyL, SHIFTRight_Index);	// Undocumented
}

#endif
  
// End of the file
/*@}*/
