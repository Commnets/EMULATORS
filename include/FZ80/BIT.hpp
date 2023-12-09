/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: BIT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/11/2023 \n
 *	Description: The list of BIT type instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_BITINSTRUCTIONS__
#define __FZX80_BITINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** To test the BIT. */
	class BIT_General : public Instruction
	{
		public:
		BIT_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With the tested bit. */
		inline bool executeWith (size_t nb, const MCHEmul::UByte& d);
	};

	// ---
	inline bool BIT_General::executeWith (size_t nb, const MCHEmul::UByte& d)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// How the flags are affected...
		// Carry flag doesn't change...
		bool v = d.bit (nb);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);		// Always
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, !v);	// Undocumented...
		st.setBitStatus (CZ80::_BIT3FLAG, nb == 3 && v);	// Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, true);		// Always...
		st.setBitStatus (CZ80::_BIT5FLAG, nb == 5 && v);	// Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, !v);
		st.setBitStatus (CZ80::_SIGNFLAG, nb == 7 && v);	// Undocumented...

		return (true);
	}

	/** The BIT instruction, when managing indexes, creates its code in another way. */
	class BIT_Index : public BIT_General
	{
		public:
		BIT_Index (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: BIT_General (c, mp, cc, rcc, t)
							{ }

		/** The last part of the code is at the end of the byte codes. */
		virtual std::vector <MCHEmul::UByte> shapeCodeWithData 
			(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const override;
	};

	// Bit 0
	_INST_FROM (0xCB47,		2, 8, 8,	"BIT 0,A",				BIT_0WithA, BIT_General);
	_INST_FROM (0xCB40,		2, 8, 8,	"BIT 0,B",				BIT_0WithB, BIT_General);
	_INST_FROM (0xCB41,		2, 8, 8,	"BIT 0,C",				BIT_0WithC, BIT_General);
	_INST_FROM (0xCB42,		2, 8, 8,	"BIT 0,D",				BIT_0WithD, BIT_General);
	_INST_FROM (0xCB43,		2, 8, 8,	"BIT 0,E",				BIT_0WithE, BIT_General);
	_INST_FROM (0xCB44,		2, 8, 8,	"BIT 0,H",				BIT_0WithH, BIT_General);
	_INST_FROM (0xCB45,		2, 8, 8,	"BIT 0,L",				BIT_0WithL, BIT_General);
	_INST_FROM (0xCB46,		2, 12, 12,	"BIT 0,(HL)",			BIT_0WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB46,	4, 20, 20,	"BIT 0,(IX+[#1])",		BIT_0WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB40,	4, 20, 20,	"U1BIT 0,(IX+[#1])",	U1BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB41,	4, 20, 20,	"U2BIT 0,(IX+[#1])",	U2BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB42,	4, 20, 20,	"U3BIT 0,(IX+[#1])",	U3BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB43,	4, 20, 20,	"U4BIT 0,(IX+[#1])",	U4BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB44,	4, 20, 20,	"U5BIT 0,(IX+[#1])",	U5BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB45,	4, 20, 20,	"U6BIT 0,(IX+[#1])",	U6BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB47,	4, 20, 20,	"U7BIT 0,(IX+[#1])",	U7BIT_0WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB46,	4, 20, 20,	"BIT 0,(IY+[#1])",		BIT_0WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB40,	4, 20, 20,	"U1BIT 0,(IY+[#1])",	U1BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB41,	4, 20, 20,	"U2BIT 0,(IY+[#1])",	U2BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB42,	4, 20, 20,	"U3BIT 0,(IY+[#1])",	U3BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB43,	4, 20, 20,	"U4BIT 0,(IY+[#1])",	U4BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB44,	4, 20, 20,	"U5BIT 0,(IY+[#1])",	U5BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB45,	4, 20, 20,	"U6BIT 0,(IY+[#1])",	U6BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB47,	4, 20, 20,	"U7BIT 0,(IY+[#1])",	U7BIT_0WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 1
	_INST_FROM (0xCB4F,		2, 8, 8,	"BIT 1,A",				BIT_1WithA, BIT_General);
	_INST_FROM (0xCB48,		2, 8, 8,	"BIT 1,B",				BIT_1WithB, BIT_General);
	_INST_FROM (0xCB49,		2, 8, 8,	"BIT 1,C",				BIT_1WithC, BIT_General);
	_INST_FROM (0xCB4A,		2, 8, 8,	"BIT 1,D",				BIT_1WithD, BIT_General);
	_INST_FROM (0xCB4B,		2, 8, 8,	"BIT 1,E",				BIT_1WithE, BIT_General);
	_INST_FROM (0xCB4C,		2, 8, 8,	"BIT 1,H",				BIT_1WithH, BIT_General);
	_INST_FROM (0xCB4D,		2, 8, 8,	"BIT 1,L",				BIT_1WithL, BIT_General);
	_INST_FROM (0xCB4E,		2, 12, 12,	"BIT 1,(HL)",			BIT_1WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB4E,	4, 20, 20,	"BIT 1,(IX+[#1])",		BIT_1WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB48,	4, 20, 20,	"U1BIT 1,(IX+[#1])",	U1BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB49,	4, 20, 20,	"U2BIT 1,(IX+[#1])",	U2BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB4A,	4, 20, 20,	"U3BIT 1,(IX+[#1])",	U3BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB4B,	4, 20, 20,	"U4BIT 1,(IX+[#1])",	U4BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB4C,	4, 20, 20,	"U5BIT 1,(IX+[#1])",	U5BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB4D,	4, 20, 20,	"U6BIT 1,(IX+[#1])",	U6BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB4F,	4, 20, 20,	"U7BIT 1,(IX+[#1])",	U7BIT_1WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4E,	4, 20, 20,	"BIT 1,(IY+[#1])",		BIT_1WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB48,	4, 20, 20,	"U1BIT 1,(IY+[#1])",	U1BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB49,	4, 20, 20,	"U2BIT 1,(IY+[#1])",	U2BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4A,	4, 20, 20,	"U3BIT 1,(IY+[#1])",	U3BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4B,	4, 20, 20,	"U4BIT 1,(IY+[#1])",	U4BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4C,	4, 20, 20,	"U5BIT 1,(IY+[#1])",	U5BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4D,	4, 20, 20,	"U6BIT 1,(IY+[#1])",	U6BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB4F,	4, 20, 20,	"U7BIT 1,(IY+[#1])",	U7BIT_1WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 2
	_INST_FROM (0xCB57,		2, 8, 8,	"BIT 2,A",				BIT_2WithA, BIT_General);
	_INST_FROM (0xCB50,		2, 8, 8,	"BIT 2,B",				BIT_2WithB, BIT_General);
	_INST_FROM (0xCB51,		2, 8, 8,	"BIT 2,C",				BIT_2WithC, BIT_General);
	_INST_FROM (0xCB52,		2, 8, 8,	"BIT 2,D",				BIT_2WithD, BIT_General);
	_INST_FROM (0xCB53,		2, 8, 8,	"BIT 2,E",				BIT_2WithE, BIT_General);
	_INST_FROM (0xCB54,		2, 8, 8,	"BIT 2,H",				BIT_2WithH, BIT_General);
	_INST_FROM (0xCB55,		2, 8, 8,	"BIT 2,L",				BIT_2WithL, BIT_General);
	_INST_FROM (0xCB56,		2, 12, 12,	"BIT 2,(HL)",			BIT_2WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB56,	4, 20, 20,	"BIT 2,(IX+[#1])",		BIT_2WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB50,	4, 20, 20,	"U1BIT 2,(IX+[#1])",	U1BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB51,	4, 20, 20,	"U2BIT 2,(IX+[#1])",	U2BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB52,	4, 20, 20,	"U3BIT 2,(IX+[#1])",	U3BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB53,	4, 20, 20,	"U4BIT 2,(IX+[#1])",	U4BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB54,	4, 20, 20,	"U5BIT 2,(IX+[#1])",	U5BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB55,	4, 20, 20,	"U6BIT 2,(IX+[#1])",	U6BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB57,	4, 20, 20,	"U7BIT 2,(IX+[#1])",	U7BIT_2WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB56,	4, 20, 20,	"BIT 2,(IY+[#1])",		BIT_2WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB50,	4, 20, 20,	"U1BIT 2,(IY+[#1])",	U1BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB51,	4, 20, 20,	"U2BIT 2,(IY+[#1])",	U2BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB52,	4, 20, 20,	"U3BIT 2,(IY+[#1])",	U3BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB53,	4, 20, 20,	"U4BIT 2,(IY+[#1])",	U4BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB54,	4, 20, 20,	"U5BIT 2,(IY+[#1])",	U5BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB55,	4, 20, 20,	"U6BIT 2,(IY+[#1])",	U6BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB57,	4, 20, 20,	"U7BIT 2,(IY+[#1])",	U7BIT_2WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 3
	_INST_FROM (0xCB5F,		2, 8, 8,	"BIT 3,A",				BIT_3WithA, BIT_General);
	_INST_FROM (0xCB58,		2, 8, 8,	"BIT 3,B",				BIT_3WithB, BIT_General);
	_INST_FROM (0xCB59,		2, 8, 8,	"BIT 3,C",				BIT_3WithC, BIT_General);
	_INST_FROM (0xCB5A,		2, 8, 8,	"BIT 3,D",				BIT_3WithD, BIT_General);
	_INST_FROM (0xCB5B,		2, 8, 8,	"BIT 3,E",				BIT_3WithE, BIT_General);
	_INST_FROM (0xCB5C,		2, 8, 8,	"BIT 3,H",				BIT_3WithH, BIT_General);
	_INST_FROM (0xCB5D,		2, 8, 8,	"BIT 3,L",				BIT_3WithL, BIT_General);
	_INST_FROM (0xCB5E,		2, 12, 12,	"BIT 3,(HL)",			BIT_3WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB5E,	4, 20, 20,	"BIT 3,(IX+[#1])",		BIT_3WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB58,	4, 20, 20,	"U1BIT 3,(IX+[#1])",	U1BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB59,	4, 20, 20,	"U2BIT 3,(IX+[#1])",	U2BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB5A,	4, 20, 20,	"U3BIT 3,(IX+[#1])",	U3BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB5B,	4, 20, 20,	"U4BIT 3,(IX+[#1])",	U4BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB5C,	4, 20, 20,	"U5BIT 3,(IX+[#1])",	U5BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB5D,	4, 20, 20,	"U6BIT 3,(IX+[#1])",	U6BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB5F,	4, 20, 20,	"U7BIT 3,(IX+[#1])",	U7BIT_3WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5E,	4, 20, 20,	"BIT 3,(IY+[#1])",		BIT_3WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB58,	4, 20, 20,	"U1BIT 3,(IY+[#1])",	U1BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB59,	4, 20, 20,	"U2BIT 3,(IY+[#1])",	U2BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5A,	4, 20, 20,	"U3BIT 3,(IY+[#1])",	U3BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5B,	4, 20, 20,	"U4BIT 3,(IY+[#1])",	U4BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5C,	4, 20, 20,	"U5BIT 3,(IY+[#1])",	U5BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5D,	4, 20, 20,	"U6BIT 3,(IY+[#1])",	U6BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB5F,	4, 20, 20,	"U7BIT 3,(IY+[#1])",	U7BIT_3WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 4
	_INST_FROM (0xCB67,		2, 8, 8,	"BIT 4,A",				BIT_4WithA, BIT_General);
	_INST_FROM (0xCB60,		2, 8, 8,	"BIT 4,B",				BIT_4WithB, BIT_General);
	_INST_FROM (0xCB61,		2, 8, 8,	"BIT 4,C",				BIT_4WithC, BIT_General);
	_INST_FROM (0xCB62,		2, 8, 8,	"BIT 4,D",				BIT_4WithD, BIT_General);
	_INST_FROM (0xCB63,		2, 8, 8,	"BIT 4,E",				BIT_4WithE, BIT_General);
	_INST_FROM (0xCB64,		2, 8, 8,	"BIT 4,H",				BIT_4WithH, BIT_General);
	_INST_FROM (0xCB65,		2, 8, 8,	"BIT 4,L",				BIT_4WithL, BIT_General);
	_INST_FROM (0xCB66,		2, 12, 12,	"BIT 4,(HL)",			BIT_4WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB66,	4, 20, 20,	"BIT 4,(IX+[#1])",		BIT_4WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB60,	4, 20, 20,	"U1BIT 4,(IX+[#1])",	U1BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB61,	4, 20, 20,	"U2BIT 4,(IX+[#1])",	U2BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB62,	4, 20, 20,	"U3BIT 4,(IX+[#1])",	U3BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB63,	4, 20, 20,	"U4BIT 4,(IX+[#1])",	U4BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB64,	4, 20, 20,	"U5BIT 4,(IX+[#1])",	U5BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB65,	4, 20, 20,	"U6BIT 4,(IX+[#1])",	U6BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB67,	4, 20, 20,	"U7BIT 4,(IX+[#1])",	U7BIT_4WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB66,	4, 20, 20,	"BIT 4,(IY+[#1])",		BIT_4WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB60,	4, 20, 20,	"U1BIT 4,(IY+[#1])",	U1BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB61,	4, 20, 20,	"U2BIT 4,(IY+[#1])",	U2BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB62,	4, 20, 20,	"U3BIT 4,(IY+[#1])",	U3BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB63,	4, 20, 20,	"U4BIT 4,(IY+[#1])",	U4BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB64,	4, 20, 20,	"U5BIT 4,(IY+[#1])",	U5BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB65,	4, 20, 20,	"U6BIT 4,(IY+[#1])",	U6BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB67,	4, 20, 20,	"U7BIT 4,(IY+[#1])",	U7BIT_4WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 5
	_INST_FROM (0xCB6F,		2, 8, 8,	"BIT 5,A",				BIT_5WithA, BIT_General);
	_INST_FROM (0xCB68,		2, 8, 8,	"BIT 5,B",				BIT_5WithB, BIT_General);
	_INST_FROM (0xCB69,		2, 8, 8,	"BIT 5,C",				BIT_5WithC, BIT_General);
	_INST_FROM (0xCB6A,		2, 8, 8,	"BIT 5,D",				BIT_5WithD, BIT_General);
	_INST_FROM (0xCB6B,		2, 8, 8,	"BIT 5,E",				BIT_5WithE, BIT_General);
	_INST_FROM (0xCB6C,		2, 8, 8,	"BIT 5,H",				BIT_5WithH, BIT_General);
	_INST_FROM (0xCB6D,		2, 8, 8,	"BIT 5,L",				BIT_5WithL, BIT_General);
	_INST_FROM (0xCB6E,		2, 12, 12,	"BIT 5,(HL)",			BIT_5WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB6E,	4, 20, 20,	"BIT 5,(IX+[#1])",		BIT_5WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB68,	4, 20, 20,	"U1BIT 5,(IX+[#1])",	U1BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB69,	4, 20, 20,	"U2BIT 5,(IX+[#1])",	U2BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB6A,	4, 20, 20,	"U3BIT 5,(IX+[#1])",	U3BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB6B,	4, 20, 20,	"U4BIT 5,(IX+[#1])",	U4BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB6C,	4, 20, 20,	"U5BIT 5,(IX+[#1])",	U5BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB6D,	4, 20, 20,	"U6BIT 5,(IX+[#1])",	U6BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB6F,	4, 20, 20,	"U7BIT 5,(IX+[#1])",	U7BIT_5WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6E,	4, 20, 20,	"BIT 5,(IY+[#1])",		BIT_5WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB68,	4, 20, 20,	"U1BIT 5,(IY+[#1])",	U1BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB69,	4, 20, 20,	"U2BIT 5,(IY+[#1])",	U2BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6A,	4, 20, 20,	"U3BIT 5,(IY+[#1])",	U3BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6B,	4, 20, 20,	"U4BIT 5,(IY+[#1])",	U4BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6C,	4, 20, 20,	"U5BIT 5,(IY+[#1])",	U5BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6D,	4, 20, 20,	"U6BIT 5,(IY+[#1])",	U6BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB6F,	4, 20, 20,	"U7BIT 5,(IY+[#1])",	U7BIT_5WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 6
	_INST_FROM (0xCB77,		2, 8, 8,	"BIT 6,A",				BIT_6WithA, BIT_General);
	_INST_FROM (0xCB70,		2, 8, 8,	"BIT 6,B",				BIT_6WithB, BIT_General);
	_INST_FROM (0xCB71,		2, 8, 8,	"BIT 6,C",				BIT_6WithC, BIT_General);
	_INST_FROM (0xCB72,		2, 8, 8,	"BIT 6,D",				BIT_6WithD, BIT_General);
	_INST_FROM (0xCB73,		2, 8, 8,	"BIT 6,E",				BIT_6WithE, BIT_General);
	_INST_FROM (0xCB74,		2, 8, 8,	"BIT 6,H",				BIT_6WithH, BIT_General);
	_INST_FROM (0xCB75,		2, 8, 8,	"BIT 6,L",				BIT_6WithL, BIT_General);
	_INST_FROM (0xCB76,		2, 12, 12,	"BIT 6,(HL)",			BIT_6WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB76,	4, 20, 20,	"BIT 6,(IX+[#1])",		BIT_6WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB70,	4, 20, 20,	"U1BIT 6,(IX+[#1])",	U1BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB71,	4, 20, 20,	"U2BIT 6,(IX+[#1])",	U2BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB72,	4, 20, 20,	"U3BIT 6,(IX+[#1])",	U3BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB73,	4, 20, 20,	"U4BIT 6,(IX+[#1])",	U4BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB74,	4, 20, 20,	"U5BIT 6,(IX+[#1])",	U5BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB75,	4, 20, 20,	"U6BIT 6,(IX+[#1])",	U6BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB77,	4, 20, 20,	"U7BIT 6,(IX+[#1])",	U7BIT_6WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB76,	4, 20, 20,	"BIT 6,(IY+[#1])",		BIT_6WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB70,	4, 20, 20,	"U1BIT 6,(IY+[#1])",	U1BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB71,	4, 20, 20,	"U2BIT 6,(IY+[#1])",	U2BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB72,	4, 20, 20,	"U3BIT 6,(IY+[#1])",	U3BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB73,	4, 20, 20,	"U4BIT 6,(IY+[#1])",	U4BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB74,	4, 20, 20,	"U5BIT 6,(IY+[#1])",	U5BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB75,	4, 20, 20,	"U6BIT 6,(IY+[#1])",	U6BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB77,	4, 20, 20,	"U7BIT 6,(IY+[#1])",	U7BIT_6WithIndirectIndexIY, BIT_Index);			// Undocumented

	// Bit 7
	_INST_FROM (0xCB7F,		2, 8, 8,	"BIT 7,A",				BIT_7WithA, BIT_General);
	_INST_FROM (0xCB78,		2, 8, 8,	"BIT 7,B",				BIT_7WithB, BIT_General);
	_INST_FROM (0xCB79,		2, 8, 8,	"BIT 7,C",				BIT_7WithC, BIT_General);
	_INST_FROM (0xCB7A,		2, 8, 8,	"BIT 7,D",				BIT_7WithD, BIT_General);
	_INST_FROM (0xCB7B,		2, 8, 8,	"BIT 7,E",				BIT_7WithE, BIT_General);
	_INST_FROM (0xCB7C,		2, 8, 8,	"BIT 7,H",				BIT_7WithH, BIT_General);
	_INST_FROM (0xCB7D,		2, 8, 8,	"BIT 7,L",				BIT_7WithL, BIT_General);
	_INST_FROM (0xCB7E,		2, 12, 12,	"BIT 7,(HL)",			BIT_7WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB7E,	4, 20, 20,	"BIT 7,(IX+[#1])",		BIT_7WithIndirectIndexIX, BIT_Index);
	_INST_FROM (0xDDCB78,	4, 20, 20,	"U1BIT 7,(IX+[#1])",	U1BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB79,	4, 20, 20,	"U2BIT 7,(IX+[#1])",	U2BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB7A,	4, 20, 20,	"U3BIT 7,(IX+[#1])",	U3BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB7B,	4, 20, 20,	"U4BIT 7,(IX+[#1])",	U4BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB7C,	4, 20, 20,	"U5BIT 7,(IX+[#1])",	U5BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB7D,	4, 20, 20,	"U6BIT 7,(IX+[#1])",	U6BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xDDCB7F,	4, 20, 20,	"U7BIT 7,(IX+[#1])",	U7BIT_7WithIndirectIndexIX, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7E,	4, 20, 20,	"BIT 7,(IY+[#1])",		BIT_7WithIndirectIndexIY, BIT_Index);
	_INST_FROM (0xFDCB78,	4, 20, 20,	"U1BIT 7,(IY+[#1])",	U1BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB79,	4, 20, 20,	"U2BIT 7,(IY+[#1])",	U2BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7A,	4, 20, 20,	"U3BIT 7,(IY+[#1])",	U3BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7B,	4, 20, 20,	"U4BIT 7,(IY+[#1])",	U4BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7C,	4, 20, 20,	"U5BIT 7,(IY+[#1])",	U5BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7D,	4, 20, 20,	"U6BIT 7,(IY+[#1])",	U6BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
	_INST_FROM (0xFDCB7F,	4, 20, 20,	"U7BIT 7,(IY+[#1])",	U7BIT_7WithIndirectIndexIY, BIT_Index);			// Undocumented
}

#endif
  
// End of the file
/*@}*/
