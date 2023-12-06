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

	// Bit 0
	_INST_FROM (0xCB47,		2, 8, 8,	"BIT 0,A",				BIT_0WithA, BIT_General);
	_INST_FROM (0xCB40,		2, 8, 8,	"BIT 0,B",				BIT_0WithB, BIT_General);
	_INST_FROM (0xCB41,		2, 8, 8,	"BIT 0,C",				BIT_0WithC, BIT_General);
	_INST_FROM (0xCB42,		2, 8, 8,	"BIT 0,D",				BIT_0WithD, BIT_General);
	_INST_FROM (0xCB43,		2, 8, 8,	"BIT 0,E",				BIT_0WithE, BIT_General);
	_INST_FROM (0xCB44,		2, 8, 8,	"BIT 0,H",				BIT_0WithH, BIT_General);
	_INST_FROM (0xCB45,		2, 8, 8,	"BIT 0,L",				BIT_0WithL, BIT_General);
	_INST_FROM (0xCB46,		2, 12, 12,	"BIT 0,(HL)",			BIT_0WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB46,	4, 20, 20,	"BIT 0,(IX+[#1])",		BIT_0WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB40..DDCB47
	_INST_FROM (0xFDCB46,	4, 20, 20,	"BIT 0,(IY+[#1])",		BIT_0WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB40..DDCB47

	// Bit 1
	_INST_FROM (0xCB4F,		2, 8, 8,	"BIT 1,A",				BIT_1WithA, BIT_General);
	_INST_FROM (0xCB48,		2, 8, 8,	"BIT 1,B",				BIT_1WithB, BIT_General);
	_INST_FROM (0xCB49,		2, 8, 8,	"BIT 1,C",				BIT_1WithC, BIT_General);
	_INST_FROM (0xCB4A,		2, 8, 8,	"BIT 1,D",				BIT_1WithD, BIT_General);
	_INST_FROM (0xCB4B,		2, 8, 8,	"BIT 1,E",				BIT_1WithE, BIT_General);
	_INST_FROM (0xCB4C,		2, 8, 8,	"BIT 1,H",				BIT_1WithH, BIT_General);
	_INST_FROM (0xCB4D,		2, 8, 8,	"BIT 1,L",				BIT_1WithL, BIT_General);
	_INST_FROM (0xCB4E,		2, 12, 12,	"BIT 1,(HL)",			BIT_1WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB4E,	4, 20, 20,	"BIT 1,(IX+[#1])",		BIT_1WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB48..DDCB4F
	_INST_FROM (0xFDCB4E,	4, 20, 20,	"BIT 1,(IY+[#1])",		BIT_1WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB48..DDCB4F

	// Bit 2
	_INST_FROM (0xCB57,		2, 8, 8,	"BIT 2,A",				BIT_2WithA, BIT_General);
	_INST_FROM (0xCB50,		2, 8, 8,	"BIT 2,B",				BIT_2WithB, BIT_General);
	_INST_FROM (0xCB51,		2, 8, 8,	"BIT 2,C",				BIT_2WithC, BIT_General);
	_INST_FROM (0xCB52,		2, 8, 8,	"BIT 2,D",				BIT_2WithD, BIT_General);
	_INST_FROM (0xCB53,		2, 8, 8,	"BIT 2,E",				BIT_2WithE, BIT_General);
	_INST_FROM (0xCB54,		2, 8, 8,	"BIT 2,H",				BIT_2WithH, BIT_General);
	_INST_FROM (0xCB55,		2, 8, 8,	"BIT 2,L",				BIT_2WithL, BIT_General);
	_INST_FROM (0xCB56,		2, 12, 12,	"BIT 2,(HL)",			BIT_2WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB56,	4, 20, 20,	"BIT 2,(IX+[#1])",		BIT_2WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB50..DDCB57
	_INST_FROM (0xFDCB56,	4, 20, 20,	"BIT 2,(IY+[#1])",		BIT_2WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB50..DDCB57

	// Bit 3
	_INST_FROM (0xCB5F,		2, 8, 8,	"BIT 3,A",				BIT_3WithA, BIT_General);
	_INST_FROM (0xCB58,		2, 8, 8,	"BIT 3,B",				BIT_3WithB, BIT_General);
	_INST_FROM (0xCB59,		2, 8, 8,	"BIT 3,C",				BIT_3WithC, BIT_General);
	_INST_FROM (0xCB5A,		2, 8, 8,	"BIT 3,D",				BIT_3WithD, BIT_General);
	_INST_FROM (0xCB5B,		2, 8, 8,	"BIT 3,E",				BIT_3WithE, BIT_General);
	_INST_FROM (0xCB5C,		2, 8, 8,	"BIT 3,H",				BIT_3WithH, BIT_General);
	_INST_FROM (0xCB5D,		2, 8, 8,	"BIT 3,L",				BIT_3WithL, BIT_General);
	_INST_FROM (0xCB5E,		2, 12, 12,	"BIT 3,(HL)",			BIT_3WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB5E,	4, 20, 20,	"BIT 3,(IX+[#1])",		BIT_3WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB58..DDCB5F
	_INST_FROM (0xFDCB5E,	4, 20, 20,	"BIT 3,(IY+[#1])",		BIT_3WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB58..DDCB5F

	// Bit 4
	_INST_FROM (0xCB67,		2, 8, 8,	"BIT 4,A",				BIT_4WithA, BIT_General);
	_INST_FROM (0xCB60,		2, 8, 8,	"BIT 4,B",				BIT_4WithB, BIT_General);
	_INST_FROM (0xCB61,		2, 8, 8,	"BIT 4,C",				BIT_4WithC, BIT_General);
	_INST_FROM (0xCB62,		2, 8, 8,	"BIT 4,D",				BIT_4WithD, BIT_General);
	_INST_FROM (0xCB63,		2, 8, 8,	"BIT 4,E",				BIT_4WithE, BIT_General);
	_INST_FROM (0xCB64,		2, 8, 8,	"BIT 4,H",				BIT_4WithH, BIT_General);
	_INST_FROM (0xCB65,		2, 8, 8,	"BIT 4,L",				BIT_4WithL, BIT_General);
	_INST_FROM (0xCB66,		2, 12, 12,	"BIT 4,(HL)",			BIT_4WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB66,	4, 20, 20,	"BIT 4,(IX+[#1])",		BIT_4WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB60..DDCB67
	_INST_FROM (0xFDCB66,	4, 20, 20,	"BIT 4,(IY+[#1])",		BIT_4WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB60..DDCB67

	// Bit 5
	_INST_FROM (0xCB6F,		2, 8, 8,	"BIT 5,A",				BIT_5WithA, BIT_General);
	_INST_FROM (0xCB68,		2, 8, 8,	"BIT 5,B",				BIT_5WithB, BIT_General);
	_INST_FROM (0xCB69,		2, 8, 8,	"BIT 5,C",				BIT_5WithC, BIT_General);
	_INST_FROM (0xCB6A,		2, 8, 8,	"BIT 5,D",				BIT_5WithD, BIT_General);
	_INST_FROM (0xCB6B,		2, 8, 8,	"BIT 5,E",				BIT_5WithE, BIT_General);
	_INST_FROM (0xCB6C,		2, 8, 8,	"BIT 5,H",				BIT_5WithH, BIT_General);
	_INST_FROM (0xCB6D,		2, 8, 8,	"BIT 5,L",				BIT_5WithL, BIT_General);
	_INST_FROM (0xCB6E,		2, 12, 12,	"BIT 5,(HL)",			BIT_5WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB6E,	4, 20, 20,	"BIT 5,(IX+[#1])",		BIT_5WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB68..DDCB6F
	_INST_FROM (0xFDCB6E,	4, 20, 20,	"BIT 5,(IY+[#1])",		BIT_5WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB68..DDCB6F

	// Bit 6
	_INST_FROM (0xCB77,		2, 8, 8,	"BIT 6,A",				BIT_6WithA, BIT_General);
	_INST_FROM (0xCB70,		2, 8, 8,	"BIT 6,B",				BIT_6WithB, BIT_General);
	_INST_FROM (0xCB71,		2, 8, 8,	"BIT 6,C",				BIT_6WithC, BIT_General);
	_INST_FROM (0xCB72,		2, 8, 8,	"BIT 6,D",				BIT_6WithD, BIT_General);
	_INST_FROM (0xCB73,		2, 8, 8,	"BIT 6,E",				BIT_6WithE, BIT_General);
	_INST_FROM (0xCB74,		2, 8, 8,	"BIT 6,H",				BIT_6WithH, BIT_General);
	_INST_FROM (0xCB75,		2, 8, 8,	"BIT 6,L",				BIT_6WithL, BIT_General);
	_INST_FROM (0xCB76,		2, 12, 12,	"BIT 6,(HL)",			BIT_6WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB76,	4, 20, 20,	"BIT 6,(IX+[#1])",		BIT_6WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB70..DDCB77
	_INST_FROM (0xFDCB76,	4, 20, 20,	"BIT 6,(IY+[#1])",		BIT_6WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB70..DDCB77

	// Bit 7
	_INST_FROM (0xCB7F,		2, 8, 8,	"BIT 7,A",				BIT_7WithA, BIT_General);
	_INST_FROM (0xCB78,		2, 8, 8,	"BIT 7,B",				BIT_7WithB, BIT_General);
	_INST_FROM (0xCB79,		2, 8, 8,	"BIT 7,C",				BIT_7WithC, BIT_General);
	_INST_FROM (0xCB7A,		2, 8, 8,	"BIT 7,D",				BIT_7WithD, BIT_General);
	_INST_FROM (0xCB7B,		2, 8, 8,	"BIT 7,E",				BIT_7WithE, BIT_General);
	_INST_FROM (0xCB7C,		2, 8, 8,	"BIT 7,H",				BIT_7WithH, BIT_General);
	_INST_FROM (0xCB7D,		2, 8, 8,	"BIT 7,L",				BIT_7WithL, BIT_General);
	_INST_FROM (0xCB7E,		2, 12, 12,	"BIT 7,(HL)",			BIT_7WithIndirectHL, BIT_General);
	_INST_FROM (0xDDCB7E,	4, 20, 20,	"BIT 7,(IX+[#1])",		BIT_7WithIndirectIndexIX, BIT_General);			// Also undocumented with codes: DDCB78..DDCB7F
	_INST_FROM (0xFDCB7E,	4, 20, 20,	"BIT 7,(IY+[#1])",		BIT_7WithIndirectIndexIY, BIT_General);			// Also undocumented with codes: FDCB78..DDCB7F
}

#endif
  
// End of the file
/*@}*/
