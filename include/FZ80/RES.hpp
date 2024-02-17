/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: RES.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/11/2023 \n
 *	Description: The list of RES type instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_RESINSTRUCTIONS__
#define __FZX80_RESINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** To reset the BIT. */
	class RES_General : public Instruction
	{
		public:
		RES_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
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
	inline bool RES_General::executeWith (size_t nb, MCHEmul::Register& r)
	{
		MCHEmul::UByte v = r.values ()[0];
		v.setBit (nb, false);
		r.set ({ v });

		// No affection to flags...

		return (true);
	}

	// ---
	inline bool RES_General::executeWith (size_t nb, const MCHEmul::Address& a)
	{
		MCHEmul::UByte v = memory () -> value (a);
		v.setBit (nb, false);
		memory () -> set (a, v);

		// No affection to flags...

		return (true);
	}

	// ---
	inline bool RES_General::executeWith (size_t nb, const MCHEmul::Address& a, MCHEmul::Register& r)
	{
		MCHEmul::UByte v = memory () -> value (a);
		v.setBit (nb, false);
		r.set ({ v });
		memory () -> set (a, v);

		// No affection to flags...

		return (true);
	}

	/** The RES instruction, when managing indexes, creates its code in another way. */
	class RES_Index : public RES_General
	{
		public:
		RES_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: RES_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Bit 0
	_INST_FROM (0xCB87,		2, 8, 8,	"RES 0,A",				RES_0WithA, RES_General);
	_INST_FROM (0xCB80,		2, 8, 8,	"RES 0,B",				RES_0WithB, RES_General);
	_INST_FROM (0xCB81,		2, 8, 8,	"RES 0,C",				RES_0WithC, RES_General);
	_INST_FROM (0xCB82,		2, 8, 8,	"RES 0,D",				RES_0WithD, RES_General);
	_INST_FROM (0xCB83,		2, 8, 8,	"RES 0,E",				RES_0WithE, RES_General);
	_INST_FROM (0xCB84,		2, 8, 8,	"RES 0,H",				RES_0WithH, RES_General);
	_INST_FROM (0xCB85,		2, 8, 8,	"RES 0,L",				RES_0WithL, RES_General);
	_INST_FROM (0xCB86,		2, 15, 15,	"RES 0,(HL)",			RES_0WithIndirectHL, RES_General);
	_INST_FROM (0xDDCB86,	4, 23, 23,	"RES 0,(IX+[#1])",		RES_0WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCB87,	4, 23, 23,	"RES 0,(IX+[#1]),A",	RES_0WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB80,	4, 23, 23,	"RES 0,(IX+[#1]),B",	RES_0WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB81,	4, 23, 23,	"RES 0,(IX+[#1]),C",	RES_0WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB82,	4, 23, 23,	"RES 0,(IX+[#1]),D",	RES_0WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB83,	4, 23, 23,	"RES 0,(IX+[#1]),E",	RES_0WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB84,	4, 23, 23,	"RES 0,(IX+[#1]),H",	RES_0WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB85,	4, 23, 23,	"RES 0,(IX+[#1]),L",	RES_0WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB86,	4, 23, 23,	"RES 0,(IY+[#1])",		RES_0WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCB87,	4, 23, 23,	"RES 0,(IY+[#1]),A",	RES_0WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB80,	4, 23, 23,	"RES 0,(IY+[#1]),B",	RES_0WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB81,	4, 23, 23,	"RES 0,(IY+[#1]),C",	RES_0WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB82,	4, 23, 23,	"RES 0,(IY+[#1]),D",	RES_0WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB83,	4, 23, 23,	"RES 0,(IY+[#1]),E",	RES_0WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB84,	4, 23, 23,	"RES 0,(IY+[#1]),H",	RES_0WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB85,	4, 23, 23,	"RES 0,(IY+[#1]),L",	RES_0WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 1
	_INST_FROM (0xCB8F,		2, 8, 8,	"RES 1,A",				RES_1WithA, RES_General);
	_INST_FROM (0xCB88,		2, 8, 8,	"RES 1,B",				RES_1WithB, RES_General);
	_INST_FROM (0xCB89,		2, 8, 8,	"RES 1,C",				RES_1WithC, RES_General);
	_INST_FROM (0xCB8A,		2, 8, 8,	"RES 1,D",				RES_1WithD, RES_General);
	_INST_FROM (0xCB8B,		2, 8, 8,	"RES 1,E",				RES_1WithE, RES_General);
	_INST_FROM (0xCB8C,		2, 8, 8,	"RES 1,H",				RES_1WithH, RES_General);
	_INST_FROM (0xCB8D,		2, 8, 8,	"RES 1,L",				RES_1WithL, RES_General);
	_INST_FROM (0xCB8E,		2, 12, 12,	"RES 1,(HL)",			RES_1WithIndirectHL, RES_General);
	_INST_FROM (0xDDCB8E,	4, 20, 20,	"RES 1,(IX+[#1])",		RES_1WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCB8F,	4, 23, 23,	"RES 1,(IX+[#1]),A",	RES_1WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB88,	4, 23, 23,	"RES 1,(IX+[#1]),B",	RES_1WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB89,	4, 23, 23,	"RES 1,(IX+[#1]),C",	RES_1WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB8A,	4, 23, 23,	"RES 1,(IX+[#1]),D",	RES_1WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB8B,	4, 23, 23,	"RES 1,(IX+[#1]),E",	RES_1WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB8C,	4, 23, 23,	"RES 1,(IX+[#1]),H",	RES_1WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB8D,	4, 23, 23,	"RES 1,(IX+[#1]),L",	RES_1WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB8E,	4, 20, 20,	"RES 1,(IY+[#1])",		RES_1WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCB8F,	4, 23, 23,	"RES 1,(IY+[#1]),A",	RES_1WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB88,	4, 23, 23,	"RES 1,(IY+[#1]),B",	RES_1WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB89,	4, 23, 23,	"RES 1,(IY+[#1]),C",	RES_1WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB8A,	4, 23, 23,	"RES 1,(IY+[#1]),D",	RES_1WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB8B,	4, 23, 23,	"RES 1,(IY+[#1]),E",	RES_1WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB8C,	4, 23, 23,	"RES 1,(IY+[#1]),H",	RES_1WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB8D,	4, 23, 23,	"RES 1,(IY+[#1]),L",	RES_1WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 2
	_INST_FROM (0xCB97,		2, 8, 8,	"RES 2,A",				RES_2WithA, RES_General);
	_INST_FROM (0xCB90,		2, 8, 8,	"RES 2,B",				RES_2WithB, RES_General);
	_INST_FROM (0xCB91,		2, 8, 8,	"RES 2,C",				RES_2WithC, RES_General);
	_INST_FROM (0xCB92,		2, 8, 8,	"RES 2,D",				RES_2WithD, RES_General);
	_INST_FROM (0xCB93,		2, 8, 8,	"RES 2,E",				RES_2WithE, RES_General);
	_INST_FROM (0xCB94,		2, 8, 8,	"RES 2,H",				RES_2WithH, RES_General);
	_INST_FROM (0xCB95,		2, 8, 8,	"RES 2,L",				RES_2WithL, RES_General);
	_INST_FROM (0xCB96,		2, 12, 12,	"RES 2,(HL)",			RES_2WithIndirectHL, RES_General);
	_INST_FROM (0xDDCB96,	4, 20, 20,	"RES 2,(IX+[#1])",		RES_2WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCB97,	4, 23, 23,	"RES 2,(IX+[#1]),A",	RES_2WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB90,	4, 23, 23,	"RES 2,(IX+[#1]),B",	RES_2WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB91,	4, 23, 23,	"RES 2,(IX+[#1]),C",	RES_2WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB92,	4, 23, 23,	"RES 2,(IX+[#1]),D",	RES_2WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB93,	4, 23, 23,	"RES 2,(IX+[#1]),E",	RES_2WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB94,	4, 23, 23,	"RES 2,(IX+[#1]),H",	RES_2WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB95,	4, 23, 23,	"RES 2,(IX+[#1]),L",	RES_2WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB96,	4, 20, 20,	"RES 2,(IY+[#1])",		RES_2WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCB97,	4, 23, 23,	"RES 2,(IY+[#1]),A",	RES_2WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB90,	4, 23, 23,	"RES 2,(IY+[#1]),B",	RES_2WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB91,	4, 23, 23,	"RES 2,(IY+[#1]),C",	RES_2WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB92,	4, 23, 23,	"RES 2,(IY+[#1]),D",	RES_2WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB93,	4, 23, 23,	"RES 2,(IY+[#1]),E",	RES_2WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB94,	4, 23, 23,	"RES 2,(IY+[#1]),H",	RES_2WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB95,	4, 23, 23,	"RES 2,(IY+[#1]),L",	RES_2WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 3
	_INST_FROM (0xCB9F,		2, 8, 8,	"RES 3,A",				RES_3WithA, RES_General);
	_INST_FROM (0xCB98,		2, 8, 8,	"RES 3,B",				RES_3WithB, RES_General);
	_INST_FROM (0xCB99,		2, 8, 8,	"RES 3,C",				RES_3WithC, RES_General);
	_INST_FROM (0xCB9A,		2, 8, 8,	"RES 3,D",				RES_3WithD, RES_General);
	_INST_FROM (0xCB9B,		2, 8, 8,	"RES 3,E",				RES_3WithE, RES_General);
	_INST_FROM (0xCB9C,		2, 8, 8,	"RES 3,H",				RES_3WithH, RES_General);
	_INST_FROM (0xCB9D,		2, 8, 8,	"RES 3,L",				RES_3WithL, RES_General);
	_INST_FROM (0xCB9E,		2, 12, 12,	"RES 3,(HL)",			RES_3WithIndirectHL, RES_General);
	_INST_FROM (0xDDCB9E,	4, 20, 20,	"RES 3,(IX+[#1])",		RES_3WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCB9F,	4, 23, 23,	"RES 3,(IX+[#1]),A",	RES_3WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB98,	4, 23, 23,	"RES 3,(IX+[#1]),B",	RES_3WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB99,	4, 23, 23,	"RES 3,(IX+[#1]),C",	RES_3WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB9A,	4, 23, 23,	"RES 3,(IX+[#1]),D",	RES_3WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB9B,	4, 23, 23,	"RES 3,(IX+[#1]),E",	RES_3WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB9C,	4, 23, 23,	"RES 3,(IX+[#1]),H",	RES_3WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCB9D,	4, 23, 23,	"RES 3,(IX+[#1]),L",	RES_3WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB9E,	4, 20, 20,	"RES 3,(IY+[#1])",		RES_3WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCB9F,	4, 23, 23,	"RES 3,(IY+[#1]),A",	RES_3WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB98,	4, 23, 23,	"RES 3,(IY+[#1]),B",	RES_3WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB99,	4, 23, 23,	"RES 3,(IY+[#1]),C",	RES_3WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB9A,	4, 23, 23,	"RES 3,(IY+[#1]),D",	RES_3WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB9B,	4, 23, 23,	"RES 3,(IY+[#1]),E",	RES_3WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB9C,	4, 23, 23,	"RES 3,(IY+[#1]),H",	RES_3WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCB9D,	4, 23, 23,	"RES 3,(IY+[#1]),L",	RES_3WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 4
	_INST_FROM (0xCBA7,		2, 8, 8,	"RES 4,A",				RES_4WithA, RES_General);
	_INST_FROM (0xCBA0,		2, 8, 8,	"RES 4,B",				RES_4WithB, RES_General);
	_INST_FROM (0xCBA1,		2, 8, 8,	"RES 4,C",				RES_4WithC, RES_General);
	_INST_FROM (0xCBA2,		2, 8, 8,	"RES 4,D",				RES_4WithD, RES_General);
	_INST_FROM (0xCBA3,		2, 8, 8,	"RES 4,E",				RES_4WithE, RES_General);
	_INST_FROM (0xCBA4,		2, 8, 8,	"RES 4,H",				RES_4WithH, RES_General);
	_INST_FROM (0xCBA5,		2, 8, 8,	"RES 4,L",				RES_4WithL, RES_General);
	_INST_FROM (0xCBA6,		2, 12, 12,	"RES 4,(HL)",			RES_4WithIndirectHL, RES_General);
	_INST_FROM (0xDDCBA6,	4, 20, 20,	"RES 4,(IX+[#1])",		RES_4WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCBA7,	4, 23, 23,	"RES 4,(IX+[#1]),A",	RES_4WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA0,	4, 23, 23,	"RES 4,(IX+[#1]),B",	RES_4WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA1,	4, 23, 23,	"RES 4,(IX+[#1]),C",	RES_4WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA2,	4, 23, 23,	"RES 4,(IX+[#1]),D",	RES_4WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA3,	4, 23, 23,	"RES 4,(IX+[#1]),E",	RES_4WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA4,	4, 23, 23,	"RES 4,(IX+[#1]),H",	RES_4WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA5,	4, 23, 23,	"RES 4,(IX+[#1]),L",	RES_4WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA6,	4, 20, 20,	"RES 4,(IY+[#1])",		RES_4WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCBA7,	4, 23, 23,	"RES 4,(IY+[#1]),A",	RES_4WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA0,	4, 23, 23,	"RES 4,(IY+[#1]),B",	RES_4WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA1,	4, 23, 23,	"RES 4,(IY+[#1]),C",	RES_4WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA2,	4, 23, 23,	"RES 4,(IY+[#1]),D",	RES_4WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA3,	4, 23, 23,	"RES 4,(IY+[#1]),E",	RES_4WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA4,	4, 23, 23,	"RES 4,(IY+[#1]),H",	RES_4WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA5,	4, 23, 23,	"RES 4,(IY+[#1]),L",	RES_4WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 5
	_INST_FROM (0xCBAF,		2, 8, 8,	"RES 5,A",				RES_5WithA, RES_General);
	_INST_FROM (0xCBA8,		2, 8, 8,	"RES 5,B",				RES_5WithB, RES_General);
	_INST_FROM (0xCBA9,		2, 8, 8,	"RES 5,C",				RES_5WithC, RES_General);
	_INST_FROM (0xCBAA,		2, 8, 8,	"RES 5,D",				RES_5WithD, RES_General);
	_INST_FROM (0xCBAB,		2, 8, 8,	"RES 5,E",				RES_5WithE, RES_General);
	_INST_FROM (0xCBAC,		2, 8, 8,	"RES 5,H",				RES_5WithH, RES_General);
	_INST_FROM (0xCBAD,		2, 8, 8,	"RES 5,L",				RES_5WithL, RES_General);
	_INST_FROM (0xCBAE,		2, 12, 12,	"RES 5,(HL)",			RES_5WithIndirectHL, RES_General);
	_INST_FROM (0xDDCBAE,	4, 20, 20,	"RES 5,(IX+[#1])",		RES_5WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCBAF,	4, 23, 23,	"RES 5,(IX+[#1]),A",	RES_5WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA8,	4, 23, 23,	"RES 5,(IX+[#1]),B",	RES_5WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBA9,	4, 23, 23,	"RES 5,(IX+[#1]),C",	RES_5WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBAA,	4, 23, 23,	"RES 5,(IX+[#1]),D",	RES_5WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBAB,	4, 23, 23,	"RES 5,(IX+[#1]),E",	RES_5WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBAC,	4, 23, 23,	"RES 5,(IX+[#1]),H",	RES_5WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBAD,	4, 23, 23,	"RES 5,(IX+[#1]),L",	RES_5WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBAE,	4, 20, 20,	"RES 5,(IY+[#1])",		RES_5WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCBAF,	4, 23, 23,	"RES 5,(IY+[#1]),A",	RES_5WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA8,	4, 23, 23,	"RES 5,(IY+[#1]),B",	RES_5WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBA9,	4, 23, 23,	"RES 5,(IY+[#1]),C",	RES_5WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBAA,	4, 23, 23,	"RES 5,(IY+[#1]),D",	RES_5WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBAB,	4, 23, 23,	"RES 5,(IY+[#1]),E",	RES_5WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBAC,	4, 23, 23,	"RES 5,(IY+[#1]),H",	RES_5WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBAD,	4, 23, 23,	"RES 5,(IY+[#1]),L",	RES_5WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 6
	_INST_FROM (0xCBB7,		2, 8, 8,	"RES 6,A",				RES_6WithA, RES_General);
	_INST_FROM (0xCBB0,		2, 8, 8,	"RES 6,B",				RES_6WithB, RES_General);
	_INST_FROM (0xCBB1,		2, 8, 8,	"RES 6,C",				RES_6WithC, RES_General);
	_INST_FROM (0xCBB2,		2, 8, 8,	"RES 6,D",				RES_6WithD, RES_General);
	_INST_FROM (0xCBB3,		2, 8, 8,	"RES 6,E",				RES_6WithE, RES_General);
	_INST_FROM (0xCBB4,		2, 8, 8,	"RES 6,H",				RES_6WithH, RES_General);
	_INST_FROM (0xCBB5,		2, 8, 8,	"RES 6,L",				RES_6WithL, RES_General);
	_INST_FROM (0xCBB6,		2, 12, 12,	"RES 6,(HL)",			RES_6WithIndirectHL, RES_General);
	_INST_FROM (0xDDCBB6,	4, 20, 20,	"RES 6,(IX+[#1])",		RES_6WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCBB7,	4, 23, 23,	"RES 6,(IX+[#1]),A",	RES_6WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB0,	4, 23, 23,	"RES 6,(IX+[#1]),B",	RES_6WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB1,	4, 23, 23,	"RES 6,(IX+[#1]),C",	RES_6WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB2,	4, 23, 23,	"RES 6,(IX+[#1]),D",	RES_6WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB3,	4, 23, 23,	"RES 6,(IX+[#1]),E",	RES_6WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB4,	4, 23, 23,	"RES 6,(IX+[#1]),H",	RES_6WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB5,	4, 23, 23,	"RES 6,(IX+[#1]),L",	RES_6WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB6,	4, 20, 20,	"RES 6,(IY+[#1])",		RES_6WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCBB7,	4, 23, 23,	"RES 6,(IY+[#1]),A",	RES_6WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB0,	4, 23, 23,	"RES 6,(IY+[#1]),B",	RES_6WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB1,	4, 23, 23,	"RES 6,(IY+[#1]),C",	RES_6WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB2,	4, 23, 23,	"RES 6,(IY+[#1]),D",	RES_6WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB3,	4, 23, 23,	"RES 6,(IY+[#1]),E",	RES_6WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB4,	4, 23, 23,	"RES 6,(IY+[#1]),H",	RES_6WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB5,	4, 23, 23,	"RES 6,(IY+[#1]),L",	RES_6WithIndirectIndexIYCopyL, RES_Index);		// Undocumented

	// Bit 7
	_INST_FROM (0xCBBF,		2, 8, 8,	"RES 7,A",				RES_7WithA, RES_General);
	_INST_FROM (0xCBB8,		2, 8, 8,	"RES 7,B",				RES_7WithB, RES_General);
	_INST_FROM (0xCBB9,		2, 8, 8,	"RES 7,C",				RES_7WithC, RES_General);
	_INST_FROM (0xCBBA,		2, 8, 8,	"RES 7,D",				RES_7WithD, RES_General);
	_INST_FROM (0xCBBB,		2, 8, 8,	"RES 7,E",				RES_7WithE, RES_General);
	_INST_FROM (0xCBBC,		2, 8, 8,	"RES 7,H",				RES_7WithH, RES_General);
	_INST_FROM (0xCBBD,		2, 8, 8,	"RES 7,L",				RES_7WithL, RES_General);
	_INST_FROM (0xCBBE,		2, 15, 15,	"RES 7,(HL)",			RES_7WithIndirectHL, RES_General);
	_INST_FROM (0xDDCBBE,	4, 23, 23,	"RES 7,(IX+[#1])",		RES_7WithIndirectIndexIX, RES_Index);
	_INST_FROM (0xDDCBBF,	4, 23, 23,	"RES 7,(IX+[#1]),A",	RES_7WithIndirectIndexIXCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB8,	4, 23, 23,	"RES 7,(IX+[#1]),B",	RES_7WithIndirectIndexIXCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBB9,	4, 23, 23,	"RES 7,(IX+[#1]),C",	RES_7WithIndirectIndexIXCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBBA,	4, 23, 23,	"RES 7,(IX+[#1]),D",	RES_7WithIndirectIndexIXCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBBB,	4, 23, 23,	"RES 7,(IX+[#1]),E",	RES_7WithIndirectIndexIXCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBBC,	4, 23, 23,	"RES 7,(IX+[#1]),H",	RES_7WithIndirectIndexIXCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xDDCBBD,	4, 23, 23,	"RES 7,(IX+[#1]),L",	RES_7WithIndirectIndexIXCopyL, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBBE,	4, 23, 23,	"RES 7,(IY+[#1])",		RES_7WithIndirectIndexIY, RES_Index);
	_INST_FROM (0xFDCBBF,	4, 23, 23,	"RES 7,(IY+[#1]),A",	RES_7WithIndirectIndexIYCopyA, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB8,	4, 23, 23,	"RES 7,(IY+[#1]),B",	RES_7WithIndirectIndexIYCopyB, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBB9,	4, 23, 23,	"RES 7,(IY+[#1]),C",	RES_7WithIndirectIndexIYCopyC, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBBA,	4, 23, 23,	"RES 7,(IY+[#1]),D",	RES_7WithIndirectIndexIYCopyD, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBBB,	4, 23, 23,	"RES 7,(IY+[#1]),E",	RES_7WithIndirectIndexIYCopyE, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBBC,	4, 23, 23,	"RES 7,(IY+[#1]),H",	RES_7WithIndirectIndexIYCopyH, RES_Index);		// Undocumented
	_INST_FROM (0xFDCBBD,	4, 23, 23,	"RES 7,(IY+[#1]),L",	RES_7WithIndirectIndexIYCopyL, RES_Index);		// Undocumented
}

#endif
  
// End of the file
/*@}*/
