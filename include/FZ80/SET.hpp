/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: SET.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/11/2023 \n
 *	Description: The list of SET type instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_SETINSTRUCTIONS__
#define __FZX80_SETINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** To set the BIT. */
	class SET_General : public Instruction
	{
		public:
		SET_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With the tested bit and a register. */
		inline bool executeWith (size_t nb, MCHEmul::Register& r);
		/** With the tested bit and an address. */
		inline bool executeWith (size_t nb, const MCHEmul::Address& a);
		/** With the tested bit, an address, and a register!. */
		inline bool executeWith (size_t nb, const MCHEmul::Address& a, MCHEmul::Register& r);
	};

	// ---
	inline bool SET_General::executeWith (size_t nb, MCHEmul::Register& r)
	{
		MCHEmul::UByte v = r.values ()[0];
		v.setBit (nb, true);
		r.set ({ v });

		// No affection to flags...

		return (true);
	}

	// ---
	inline bool SET_General::executeWith (size_t nb, const MCHEmul::Address& a)
	{
		MCHEmul::UByte v = memory () -> value (a);
		v.setBit (nb, true);
		memory () -> set (a, v);

		// No affection to flags...

		return (true);
	}

	// ---
	inline bool SET_General::executeWith (size_t nb, const MCHEmul::Address& a, MCHEmul::Register& r)
	{
		MCHEmul::UByte v = memory () -> value (a);
		v.setBit (nb, true);
		r.set ({ v });
		memory () -> set (a, v);

		// No affection to flags...

		return (true);
	}

	/** The SET instruction, when managing indexes, creates its code in another way. */
	class SET_Index : public SET_General
	{
		public:
		SET_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: SET_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;

		protected:
		/** The parameter should start at position 3 but it is really at position 2 instead. */
		virtual size_t byteParameter (size_t nP) const override
							{ return ((nP == 3) ? 2 : nP); }
	};

	// Bit 0
	_INST_FROM (0xCBC7,		2, 8, 8,	"SET 0,A",				SET_0WithA, SET_General);
	_INST_FROM (0xCBC0,		2, 8, 8,	"SET 0,B",				SET_0WithB, SET_General);
	_INST_FROM (0xCBC1,		2, 8, 8,	"SET 0,C",				SET_0WithC, SET_General);
	_INST_FROM (0xCBC2,		2, 8, 8,	"SET 0,D",				SET_0WithD, SET_General);
	_INST_FROM (0xCBC3,		2, 8, 8,	"SET 0,E",				SET_0WithE, SET_General);
	_INST_FROM (0xCBC4,		2, 8, 8,	"SET 0,H",				SET_0WithH, SET_General);
	_INST_FROM (0xCBC5,		2, 8, 8,	"SET 0,L",				SET_0WithL, SET_General);
	_INST_FROM (0xCBC6,		2, 15, 15,	"SET 0,(HL)",			SET_0WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBC6,	4, 23, 23,	"SET 0,(IX+[#1])",		SET_0WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBC7,	4, 23, 23,	"SET 0,(IX+[#1]),A",	SET_0WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC0,	4, 23, 23,	"SET 0,(IX+[#1]),B",	SET_0WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC1,	4, 23, 23,	"SET 0,(IX+[#1]),C",	SET_0WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC2,	4, 23, 23,	"SET 0,(IX+[#1]),D",	SET_0WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC3,	4, 23, 23,	"SET 0,(IX+[#1]),E",	SET_0WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC4,	4, 23, 23,	"SET 0,(IX+[#1]),H",	SET_0WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC5,	4, 23, 23,	"SET 0,(IX+[#1]),L",	SET_0WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC6,	4, 23, 23,	"SET 0,(IY+[#1])",		SET_0WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBC7,	4, 23, 23,	"SET 0,(IY+[#1]),A",	SET_0WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC0,	4, 23, 23,	"SET 0,(IY+[#1]),B",	SET_0WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC1,	4, 23, 23,	"SET 0,(IY+[#1]),C",	SET_0WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC2,	4, 23, 23,	"SET 0,(IY+[#1]),D",	SET_0WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC3,	4, 23, 23,	"SET 0,(IY+[#1]),E",	SET_0WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC4,	4, 23, 23,	"SET 0,(IY+[#1]),H",	SET_0WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC5,	4, 23, 23,	"SET 0,(IY+[#1]),L",	SET_0WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 1
	_INST_FROM (0xCBCF,		2, 8, 8,	"SET 1,A",				SET_1WithA, SET_General);
	_INST_FROM (0xCBC8,		2, 8, 8,	"SET 1,B",				SET_1WithB, SET_General);
	_INST_FROM (0xCBC9,		2, 8, 8,	"SET 1,C",				SET_1WithC, SET_General);
	_INST_FROM (0xCBCA,		2, 8, 8,	"SET 1,D",				SET_1WithD, SET_General);
	_INST_FROM (0xCBCB,		2, 8, 8,	"SET 1,E",				SET_1WithE, SET_General);
	_INST_FROM (0xCBCC,		2, 8, 8,	"SET 1,H",				SET_1WithH, SET_General);
	_INST_FROM (0xCBCD,		2, 8, 8,	"SET 1,L",				SET_1WithL, SET_General);
	_INST_FROM (0xCBCE,		2, 15, 15,	"SET 1,(HL)",			SET_1WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBCE,	4, 23, 23,	"SET 1,(IX+[#1])",		SET_1WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBCF,	4, 23, 23,	"SET 1,(IX+[#1]),A",	SET_1WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC8,	4, 23, 23,	"SET 1,(IX+[#1]),B",	SET_1WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBC9,	4, 23, 23,	"SET 1,(IX+[#1]),C",	SET_1WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBCA,	4, 23, 23,	"SET 1,(IX+[#1]),D",	SET_1WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBCB,	4, 23, 23,	"SET 1,(IX+[#1]),E",	SET_1WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBCC,	4, 23, 23,	"SET 1,(IX+[#1]),H",	SET_1WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBCD,	4, 23, 23,	"SET 1,(IX+[#1]),L",	SET_1WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBCE,	4, 23, 23,	"SET 1,(IY+[#1])",		SET_1WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBCF,	4, 23, 23,	"SET 1,(IY+[#1]),A",	SET_1WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC8,	4, 23, 23,	"SET 1,(IY+[#1]),B",	SET_1WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBC9,	4, 23, 23,	"SET 1,(IY+[#1]),C",	SET_1WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBCA,	4, 23, 23,	"SET 1,(IY+[#1]),D",	SET_1WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBCB,	4, 23, 23,	"SET 1,(IY+[#1]),E",	SET_1WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBCC,	4, 23, 23,	"SET 1,(IY+[#1]),H",	SET_1WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBCD,	4, 23, 23,	"SET 1,(IY+[#1]),L",	SET_1WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 2
	_INST_FROM (0xCBD7,		2, 8, 8,	"SET 2,A",				SET_2WithA, SET_General);
	_INST_FROM (0xCBD0,		2, 8, 8,	"SET 2,B",				SET_2WithB, SET_General);
	_INST_FROM (0xCBD1,		2, 8, 8,	"SET 2,C",				SET_2WithC, SET_General);
	_INST_FROM (0xCBD2,		2, 8, 8,	"SET 2,D",				SET_2WithD, SET_General);
	_INST_FROM (0xCBD3,		2, 8, 8,	"SET 2,E",				SET_2WithE, SET_General);
	_INST_FROM (0xCBD4,		2, 8, 8,	"SET 2,H",				SET_2WithH, SET_General);
	_INST_FROM (0xCBD5,		2, 8, 8,	"SET 2,L",				SET_2WithL, SET_General);
	_INST_FROM (0xCBD6,		2, 15, 15,	"SET 2,(HL)",			SET_2WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBD6,	4, 23, 23,	"SET 2,(IX+[#1])",		SET_2WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBD7,	4, 23, 23,	"SET 2,(IX+[#1]),A",	SET_2WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD0,	4, 23, 23,	"SET 2,(IX+[#1]),B",	SET_2WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD1,	4, 23, 23,	"SET 2,(IX+[#1]),C",	SET_2WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD2,	4, 23, 23,	"SET 2,(IX+[#1]),D",	SET_2WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD3,	4, 23, 23,	"SET 2,(IX+[#1]),E",	SET_2WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD4,	4, 23, 23,	"SET 2,(IX+[#1]),H",	SET_2WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD5,	4, 23, 23,	"SET 2,(IX+[#1]),L",	SET_2WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD6,	4, 23, 23,	"SET 2,(IY+[#1])",		SET_2WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBD7,	4, 23, 23,	"SET 2,(IY+[#1]),A",	SET_2WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD0,	4, 23, 23,	"SET 2,(IY+[#1]),B",	SET_2WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD1,	4, 23, 23,	"SET 2,(IY+[#1]),C",	SET_2WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD2,	4, 23, 23,	"SET 2,(IY+[#1]),D",	SET_2WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD3,	4, 23, 23,	"SET 2,(IY+[#1]),E",	SET_2WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD4,	4, 23, 23,	"SET 2,(IY+[#1]),H",	SET_2WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD5,	4, 23, 23,	"SET 2,(IY+[#1]),L",	SET_2WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 3
	_INST_FROM (0xCBDF,		2, 8, 8,	"SET 3,A",				SET_3WithA, SET_General);
	_INST_FROM (0xCBD8,		2, 8, 8,	"SET 3,B",				SET_3WithB, SET_General);
	_INST_FROM (0xCBD9,		2, 8, 8,	"SET 3,C",				SET_3WithC, SET_General);
	_INST_FROM (0xCBDA,		2, 8, 8,	"SET 3,D",				SET_3WithD, SET_General);
	_INST_FROM (0xCBDB,		2, 8, 8,	"SET 3,E",				SET_3WithE, SET_General);
	_INST_FROM (0xCBDC,		2, 8, 8,	"SET 3,H",				SET_3WithH, SET_General);
	_INST_FROM (0xCBDD,		2, 8, 8,	"SET 3,L",				SET_3WithL, SET_General);
	_INST_FROM (0xCBDE,		2, 15, 15,	"SET 3,(HL)",			SET_3WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBDE,	4, 23, 23,	"SET 3,(IX+[#1])",		SET_3WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBDF,	4, 23, 23,	"SET 3,(IX+[#1]),A",	SET_3WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD8,	4, 23, 23,	"SET 3,(IX+[#1]),B",	SET_3WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBD9,	4, 23, 23,	"SET 3,(IX+[#1]),C",	SET_3WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBDA,	4, 23, 23,	"SET 3,(IX+[#1]),D",	SET_3WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBDB,	4, 23, 23,	"SET 3,(IX+[#1]),E",	SET_3WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBDC,	4, 23, 23,	"SET 3,(IX+[#1]),H",	SET_3WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBDD,	4, 23, 23,	"SET 3,(IX+[#1]),L",	SET_3WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBDE,	4, 23, 23,	"SET 3,(IY+[#1])",		SET_3WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBDF,	4, 23, 23,	"SET 3,(IY+[#1]),A",	SET_3WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD8,	4, 23, 23,	"SET 3,(IY+[#1]),B",	SET_3WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBD9,	4, 23, 23,	"SET 3,(IY+[#1]),C",	SET_3WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBDA,	4, 23, 23,	"SET 3,(IY+[#1]),D",	SET_3WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBDB,	4, 23, 23,	"SET 3,(IY+[#1]),E",	SET_3WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBDC,	4, 23, 23,	"SET 3,(IY+[#1]),H",	SET_3WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBDD,	4, 23, 23,	"SET 3,(IY+[#1]),L",	SET_3WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 4
	_INST_FROM (0xCBE7,		2, 8, 8,	"SET 4,A",				SET_4WithA, SET_General);
	_INST_FROM (0xCBE0,		2, 8, 8,	"SET 4,B",				SET_4WithB, SET_General);
	_INST_FROM (0xCBE1,		2, 8, 8,	"SET 4,C",				SET_4WithC, SET_General);
	_INST_FROM (0xCBE2,		2, 8, 8,	"SET 4,D",				SET_4WithD, SET_General);
	_INST_FROM (0xCBE3,		2, 8, 8,	"SET 4,E",				SET_4WithE, SET_General);
	_INST_FROM (0xCBE4,		2, 8, 8,	"SET 4,H",				SET_4WithH, SET_General);
	_INST_FROM (0xCBE5,		2, 8, 8,	"SET 4,L",				SET_4WithL, SET_General);
	_INST_FROM (0xCBE6,		2, 15, 15,	"SET 4,(HL)",			SET_4WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBE6,	4, 23, 23,	"SET 4,(IX+[#1])",		SET_4WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBE7,	4, 23, 23,	"SET 4,(IX+[#1]),A",	SET_4WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE0,	4, 23, 23,	"SET 4,(IX+[#1]),B",	SET_4WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE1,	4, 23, 23,	"SET 4,(IX+[#1]),C",	SET_4WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE2,	4, 23, 23,	"SET 4,(IX+[#1]),D",	SET_4WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE3,	4, 23, 23,	"SET 4,(IX+[#1]),E",	SET_4WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE4,	4, 23, 23,	"SET 4,(IX+[#1]),H",	SET_4WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE5,	4, 23, 23,	"SET 4,(IX+[#1]),L",	SET_4WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE6,	4, 23, 23,	"SET 4,(IY+[#1])",		SET_4WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBE7,	4, 23, 23,	"SET 4,(IY+[#1]),A",	SET_4WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE0,	4, 23, 23,	"SET 4,(IY+[#1]),B",	SET_4WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE1,	4, 23, 23,	"SET 4,(IY+[#1]),C",	SET_4WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE2,	4, 23, 23,	"SET 4,(IY+[#1]),D",	SET_4WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE3,	4, 23, 23,	"SET 4,(IY+[#1]),E",	SET_4WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE4,	4, 23, 23,	"SET 4,(IY+[#1]),H",	SET_4WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE5,	4, 23, 23,	"SET 4,(IY+[#1]),L",	SET_4WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 5
	_INST_FROM (0xCBEF,		2, 8, 8,	"SET 5,A",				SET_5WithA, SET_General);
	_INST_FROM (0xCBE8,		2, 8, 8,	"SET 5,B",				SET_5WithB, SET_General);
	_INST_FROM (0xCBE9,		2, 8, 8,	"SET 5,C",				SET_5WithC, SET_General);
	_INST_FROM (0xCBEA,		2, 8, 8,	"SET 5,D",				SET_5WithD, SET_General);
	_INST_FROM (0xCBEB,		2, 8, 8,	"SET 5,E",				SET_5WithE, SET_General);
	_INST_FROM (0xCBEC,		2, 8, 8,	"SET 5,H",				SET_5WithH, SET_General);
	_INST_FROM (0xCBED,		2, 8, 8,	"SET 5,L",				SET_5WithL, SET_General);
	_INST_FROM (0xCBEE,		2, 15, 15,	"SET 5,(HL)",			SET_5WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBEE,	4, 23, 23,	"SET 5,(IX+[#1])",		SET_5WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBEF,	4, 23, 23,	"SET 5,(IX+[#1]),A",	SET_5WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE8,	4, 23, 23,	"SET 5,(IX+[#1]),B",	SET_5WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBE9,	4, 23, 23,	"SET 5,(IX+[#1]),C",	SET_5WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBEA,	4, 23, 23,	"SET 5,(IX+[#1]),D",	SET_5WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBEB,	4, 23, 23,	"SET 5,(IX+[#1]),E",	SET_5WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBEC,	4, 23, 23,	"SET 5,(IX+[#1]),H",	SET_5WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBED,	4, 23, 23,	"SET 5,(IX+[#1]),L",	SET_5WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBEE,	4, 23, 23,	"SET 5,(IY+[#1])",		SET_5WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBEF,	4, 23, 23,	"SET 5,(IY+[#1]),A",	SET_5WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE8,	4, 23, 23,	"SET 5,(IY+[#1]),B",	SET_5WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBE9,	4, 23, 23,	"SET 5,(IY+[#1]),C",	SET_5WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBEA,	4, 23, 23,	"SET 5,(IY+[#1]),D",	SET_5WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBEB,	4, 23, 23,	"SET 5,(IY+[#1]),E",	SET_5WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBEC,	4, 23, 23,	"SET 5,(IY+[#1]),H",	SET_5WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBED,	4, 23, 23,	"SET 5,(IY+[#1]),L",	SET_5WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 6
	_INST_FROM (0xCBF7,		2, 8, 8,	"SET 6,A",				SET_6WithA, SET_General);
	_INST_FROM (0xCBF0,		2, 8, 8,	"SET 6,B",				SET_6WithB, SET_General);
	_INST_FROM (0xCBF1,		2, 8, 8,	"SET 6,C",				SET_6WithC, SET_General);
	_INST_FROM (0xCBF2,		2, 8, 8,	"SET 6,D",				SET_6WithD, SET_General);
	_INST_FROM (0xCBF3,		2, 8, 8,	"SET 6,E",				SET_6WithE, SET_General);
	_INST_FROM (0xCBF4,		2, 8, 8,	"SET 6,H",				SET_6WithH, SET_General);
	_INST_FROM (0xCBF5,		2, 8, 8,	"SET 6,L",				SET_6WithL, SET_General);
	_INST_FROM (0xCBF6,		2, 15, 15,	"SET 6,(HL)",			SET_6WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBF6,	4, 23, 23,	"SET 6,(IX+[#1])",		SET_6WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBF7,	4, 23, 23,	"SET 6,(IX+[#1]),A",	SET_6WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF0,	4, 23, 23,	"SET 6,(IX+[#1]),B",	SET_6WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF1,	4, 23, 23,	"SET 6,(IX+[#1]),C",	SET_6WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF2,	4, 23, 23,	"SET 6,(IX+[#1]),D",	SET_6WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF3,	4, 23, 23,	"SET 6,(IX+[#1]),E",	SET_6WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF4,	4, 23, 23,	"SET 6,(IX+[#1]),H",	SET_6WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF5,	4, 23, 23,	"SET 6,(IX+[#1]),L",	SET_6WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF6,	4, 23, 23,	"SET 6,(IY+[#1])",		SET_6WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBF7,	4, 23, 23,	"SET 6,(IY+[#1]),A",	SET_6WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF0,	4, 23, 23,	"SET 6,(IY+[#1]),B",	SET_6WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF1,	4, 23, 23,	"SET 6,(IY+[#1]),C",	SET_6WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF2,	4, 23, 23,	"SET 6,(IY+[#1]),D",	SET_6WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF3,	4, 23, 23,	"SET 6,(IY+[#1]),E",	SET_6WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF4,	4, 23, 23,	"SET 6,(IY+[#1]),H",	SET_6WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF5,	4, 23, 23,	"SET 6,(IY+[#1]),L",	SET_6WithIndirectIndexIYCopyL, SET_Index);		// Undocumented

	// Bit 7
	_INST_FROM (0xCBFF,		2, 8, 8,	"SET 7,A",				SET_7WithA, SET_General);
	_INST_FROM (0xCBF8,		2, 8, 8,	"SET 7,B",				SET_7WithB, SET_General);
	_INST_FROM (0xCBF9,		2, 8, 8,	"SET 7,C",				SET_7WithC, SET_General);
	_INST_FROM (0xCBFA,		2, 8, 8,	"SET 7,D",				SET_7WithD, SET_General);
	_INST_FROM (0xCBFB,		2, 8, 8,	"SET 7,E",				SET_7WithE, SET_General);
	_INST_FROM (0xCBFC,		2, 8, 8,	"SET 7,H",				SET_7WithH, SET_General);
	_INST_FROM (0xCBFD,		2, 8, 8,	"SET 7,L",				SET_7WithL, SET_General);
	_INST_FROM (0xCBFE,		2, 15, 15,	"SET 7,(HL)",			SET_7WithIndirectHL, SET_General);
	_INST_FROM (0xDDCBFE,	4, 23, 23,	"SET 7,(IX+[#1])",		SET_7WithIndirectIndexIX, SET_Index);
	_INST_FROM (0xDDCBFF,	4, 23, 23,	"SET 7,(IX+[#1]),A",	SET_7WithIndirectIndexIXCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF8,	4, 23, 23,	"SET 7,(IX+[#1]),B",	SET_7WithIndirectIndexIXCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBF9,	4, 23, 23,	"SET 7,(IX+[#1]),C",	SET_7WithIndirectIndexIXCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBFA,	4, 23, 23,	"SET 7,(IX+[#1]),D",	SET_7WithIndirectIndexIXCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBFB,	4, 23, 23,	"SET 7,(IX+[#1]),E",	SET_7WithIndirectIndexIXCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBFC,	4, 23, 23,	"SET 7,(IX+[#1]),H",	SET_7WithIndirectIndexIXCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xDDCBFD,	4, 23, 23,	"SET 7,(IX+[#1]),L",	SET_7WithIndirectIndexIXCopyL, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBFE,	4, 23, 23,	"SET 7,(IY+[#1])",		SET_7WithIndirectIndexIY, SET_Index);
	_INST_FROM (0xFDCBFF,	4, 23, 23,	"SET 7,(IY+[#1]),A",	SET_7WithIndirectIndexIYCopyA, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF8,	4, 23, 23,	"SET 7,(IY+[#1]),B",	SET_7WithIndirectIndexIYCopyB, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBF9,	4, 23, 23,	"SET 7,(IY+[#1]),C",	SET_7WithIndirectIndexIYCopyC, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBFA,	4, 23, 23,	"SET 7,(IY+[#1]),D",	SET_7WithIndirectIndexIYCopyD, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBFB,	4, 23, 23,	"SET 7,(IY+[#1]),E",	SET_7WithIndirectIndexIYCopyE, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBFC,	4, 23, 23,	"SET 7,(IY+[#1]),H",	SET_7WithIndirectIndexIYCopyH, SET_Index);		// Undocumented
	_INST_FROM (0xFDCBFD,	4, 23, 23,	"SET 7,(IY+[#1]),L",	SET_7WithIndirectIndexIYCopyL, SET_Index);		// Undocumented
}

#endif
  
// End of the file
/*@}*/
