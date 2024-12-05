/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: SHIFTLEFT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of SHIFTLEFT related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_SHIFTLEFTINSTRUCTIONS__
#define __FZX80_SHIFTLEFTINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Shift the bits to the left. \n
		Can be useful to implement either shift or rotate movements. */
	class SHIFTLeft_General : public Instruction
	{
		public:
		SHIFTLeft_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		/** A register. \n
			When crr = true		=>	bit 0 <- 1, CF <- bit 7.
			When crr = false	=>	bit 0 <- 0, CF <- bit 7. */
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
	inline bool SHIFTLeft_General::executeWith (MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = r.values ()[0];
		bool c = v.shiftLeftC (crr);
		r.set ({ v });

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool SHIFTLeft_General::executeWith (const MCHEmul::Address& a, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftLeftC (crr);
		memory () -> set (a, v); // Set back the value...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline bool SHIFTLeft_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
	{
		MCHEmul::UByte v = memory () -> value (a);
		bool c = v.shiftLeftC (crr);
		memory () -> set (a, v); // Set back the value...
		r.set ({ v }); // And store it in the register...

		affectFlagsWith (v, c);

		return (true);
	}

	// ---
	inline void SHIFTLeft_General::affectFlagsWith (const MCHEmul::UByte& v, bool c)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		st.setBitStatus (CZ80::_CARRYFLAG, c);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
		st.setBitStatus (CZ80::_BIT3FLAG, v.bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, v.bit (7));
	}

	/** The SHIFTLEFT instruction, when managing indexes, creates its code in another way. */
	class SHIFTLeft_Index : public SHIFTLeft_General
	{
		public:
		SHIFTLeft_Index (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: SHIFTLeft_General (c, mp, cc, cS, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Shift introducing a 0 in the first bit!
	// Within A
	_INSTZ80_FROM (0xCB27,		2, 8, { },	"SLA A",			SLA_A, SHIFTLeft_General);
	// Within B
	_INSTZ80_FROM (0xCB20,		2, 8, { },	"SLA B",			SLA_B, SHIFTLeft_General);
	// Within C
	_INSTZ80_FROM (0xCB21,		2, 8, { },	"SLA C",			SLA_C, SHIFTLeft_General);
	// Within D
	_INSTZ80_FROM (0xCB22,		2, 8, { },	"SLA D",			SLA_D, SHIFTLeft_General);
	// Within E
	_INSTZ80_FROM (0xCB23,		2, 8, { },	"SLA E",			SLA_E, SHIFTLeft_General);
	// Within F
	_INSTZ80_FROM (0xCB24,		2, 8, { },	"SLA H",			SLA_H, SHIFTLeft_General);
	// Within L
	_INSTZ80_FROM (0xCB25,		2, 8, { },	"SLA L",			SLA_L, SHIFTLeft_General);
	// Within (HL)
	_INSTZ80_FROM (0xCB26,		2, 15, { },	"SLA (HL)",			SLA_IndirectHL, SHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INSTZ80_FROM (0xDDCB26,	4, 23, { },	"SLA (IX+[#1])",	SLA_IndirectIndexIX, SHIFTLeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INSTZ80_FROM (0xDDCB27,	4, 23, { },	"SLA (IX+[#1]),A",	SLA_IndirectIndexIXCopyA, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB20,	4, 23, { },	"SLA (IX+[#1]),B",	SLA_IndirectIndexIXCopyB, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB21,	4, 23, { },	"SLA (IX+[#1]),C",	SLA_IndirectIndexIXCopyC, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB22,	4, 23, { },	"SLA (IX+[#1]),D",	SLA_IndirectIndexIXCopyD, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB23,	4, 23, { },	"SLA (IX+[#1]),E",	SLA_IndirectIndexIXCopyE, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB24,	4, 23, { },	"SLA (IX+[#1]),H",	SLA_IndirectIndexIXCopyH, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB25,	4, 23, { },	"SLA (IX+[#1]),L",	SLA_IndirectIndexIXCopyL, SHIFTLeft_Index);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INSTZ80_FROM (0xFDCB26,	4, 23, { },	"SLA (IY+[#1])",	SLA_IndirectIndexIY, SHIFTLeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INSTZ80_FROM (0xFDCB27,	4, 23, { },	"SLA (IY+[#1]),A",	SLA_IndirectIndexIYCopyA, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB20,	4, 23, { },	"SLA (IY+[#1]),B",	SLA_IndirectIndexIYCopyB, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB21,	4, 23, { },	"SLA (IY+[#1]),C",	SLA_IndirectIndexIYCopyC, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB22,	4, 23, { },	"SLA (IY+[#1]),D",	SLA_IndirectIndexIYCopyD, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB23,	4, 23, { },	"SLA (IY+[#1]),E",	SLA_IndirectIndexIYCopyE, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB24,	4, 23, { },	"SLA (IY+[#1]),H",	SLA_IndirectIndexIYCopyH, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB25,	4, 23, { },	"SLA (IY+[#1]),L",	SLA_IndirectIndexIYCopyL, SHIFTLeft_Index);		// Undocumented

	// Shift introducing a 1 in the first bit!
	// Within A
	_INSTZ80_FROM (0xCB37,		2, 8, { },	"SLL A",			SLL_A, SHIFTLeft_General);
	// Within B
	_INSTZ80_FROM (0xCB30,		2, 8, { },	"SLL B",			SLL_B, SHIFTLeft_General);
	// Within C
	_INSTZ80_FROM (0xCB31,		2, 8, { },	"SLL C",			SLL_C, SHIFTLeft_General);
	// Within D
	_INSTZ80_FROM (0xCB32,		2, 8, { },	"SLL D",			SLL_D, SHIFTLeft_General);
	// Within E
	_INSTZ80_FROM (0xCB33,		2, 8, { },	"SLL E",			SLL_E, SHIFTLeft_General);
	// Within F
	_INSTZ80_FROM (0xCB34,		2, 8, { },	"SLL H",			SLL_H, SHIFTLeft_General);
	// Within L
	_INSTZ80_FROM (0xCB35,		2, 8, { },	"SLL L",			SLL_L, SHIFTLeft_General);
	// Within (HL)
	_INSTZ80_FROM (0xCB36,		2, 15, { },	"SLL (HL)",			SLL_IndirectHL, SHIFTLeft_General);
	// Within (IX + d)...4 block but it really uses only 3!
	// The instruction code is really DDCD(opCode)06, but is done in this way to simplify the way it is used...
	// This applicable to all instructions...
	_INSTZ80_FROM (0xDDCB36,	4, 23, { },	"SLL (IX+[#1])",	SLL_IndirectIndexIX, SHIFTLeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INSTZ80_FROM (0xDDCB37,	4, 23, { },	"SLL (IX+[#1]),A",	SLL_IndirectIndexIXCopyA, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB30,	4, 23, { },	"SLL (IX+[#1]),B",	SLL_IndirectIndexIXCopyB, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB31,	4, 23, { },	"SLL (IX+[#1]),C",	SLL_IndirectIndexIXCopyC, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB32,	4, 23, { },	"SLL (IX+[#1]),D",	SLL_IndirectIndexIXCopyD, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB33,	4, 23, { },	"SLL (IX+[#1]),E",	SLL_IndirectIndexIXCopyE, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB34,	4, 23, { },	"SLL (IX+[#1]),H",	SLL_IndirectIndexIXCopyH, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xDDCB35,	4, 23, { },	"SLL (IX+[#1]),L",	SLL_IndirectIndexIXCopyL, SHIFTLeft_Index);		// Undocumented
	// Within (IY + d)...4 block instruction but it really uses only 3!
	_INSTZ80_FROM (0xFDCB36,	4, 23, { },	"SLL (IY+[#1])",	SLL_IndirectIndexIY, SHIFTLeft_Index);
	// Within (IX + d) and the result copied into a register. 
	// All of them are non documented
	_INSTZ80_FROM (0xFDCB37,	4, 23, { },	"SLL (IY+[#1]),A",	SLL_IndirectIndexIYCopyA, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB30,	4, 23, { },	"SLL (IY+[#1]),B",	SLL_IndirectIndexIYCopyB, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB31,	4, 23, { },	"SLL (IY+[#1]),C",	SLL_IndirectIndexIYCopyC, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB32,	4, 23, { },	"SLL (IY+[#1]),D",	SLL_IndirectIndexIYCopyD, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB33,	4, 23, { },	"SLL (IY+[#1]),E",	SLL_IndirectIndexIYCopyE, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB34,	4, 23, { },	"SLL (IY+[#1]),H",	SLL_IndirectIndexIYCopyH, SHIFTLeft_Index);		// Undocumented
	_INSTZ80_FROM (0xFDCB35,	4, 23, { },	"SLL (IY+[#1]),L",	SLL_IndirectIndexIYCopyL, SHIFTLeft_Index);		// Undocumented
}

#endif
  
// End of the file
/*@}*/
