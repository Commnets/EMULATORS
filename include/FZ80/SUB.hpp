/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: SUB.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of SUB instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_SUBINSTRUCTIONS__
#define __FZX80_SUBINSTRUCTIONS__

namespace FZ80
{
	/** SUB is always over the value of A. */
	class SUB_General : public Instruction
	{
		public:
		SUB_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a value. \n
			The 8 bit operations are always done take A register as destination. \n
			The value of the carry is or not taken into account according with the parameter c. \n
			No carry is taken into account by default. */
		bool executeWith (const MCHEmul::UByte& v, bool c = false);
		/** With a complex register and a value. 
			All 16 bit operations are SBC type, so carry has to be taken into account. \n
			All 16 bit operations have HL register as destination. */
		bool executeWith (const MCHEmul::UBytes& v);
	};

	// Without carry
	// With A
	_INST_FROM (0x97,	1, 4, 4,	"SUB A",				SUB_AWithA, SUB_General);				// Also undocumented with codes: DD97 & FD97
	// With B
	_INST_FROM (0x90,	1, 4, 4,	"SUB B",				SUB_AWithB, SUB_General);				// Also undocumented with codes: DD90 & FD90
	// Wit C
	_INST_FROM (0x91,	1, 4, 4,	"SUB C",				SUB_AWithC, SUB_General);				// Also undocumented with codes: DD91 & FD91
	// Wih D
	_INST_FROM (0x92,	1, 4, 4,	"SUB D",				SUB_AWithD, SUB_General);				// Also undocumented with codes: DD92 & FD92
	// Wth E
	_INST_FROM (0x93,	1, 4, 4,	"SUB E",				SUB_AWithE, SUB_General);				// Also undocumented with codes: DD93 & FD93
	// Wih H
	_INST_FROM (0x94,	1, 4, 4,	"SUB H",				SUB_AWithH, SUB_General);
	// Wih L
	_INST_FROM (0x95,	1, 4, 4,	"SUB L",				SUB_AWithL, SUB_General);
	// With (HL)
	_INST_FROM (0x96,	1, 7, 7,	"SUB (HL)",				SUB_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INST_FROM (0xDD96, 3, 19, 19,	"SUB (IX+[#1])",		SUB_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INST_FROM (0xFD96,	3, 19, 19,	"SUB (IY+[#1])",		SUB_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INST_FROM (0xD6,	2, 7, 7,	"SUB [#1]",				SUB_A, SUB_General);
	// Notice that there is no 16 bit SUB instructions
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD94,	2, 8, 8,	"SUB IXH",				SUB_AWithIXH, SUB_General);				// Undocumented
	_INST_FROM (0xDD95,	2, 8, 8,	"SUB IXL",				SUB_AWithIXL, SUB_General);				// Undocumented
	_INST_FROM (0xFD94,	2, 8, 8,	"SUB IYH",				SUB_AWithIYH, SUB_General);				// Undocumented
	_INST_FROM (0xFD95,	2, 8, 8,	"SUB IYL",				SUB_AWithIYL, SUB_General);				// Undocumented

	// With carry
	// With A
	_INST_FROM (0x9F,	1, 4, 4,	"SBC A,A",				SBC_AWithA, SUB_General);				// Also undocumented with codes: DD9F & FD9F
	// With B
	_INST_FROM (0x98,	1, 4, 4,	"SBC A,B",				SBC_AWithB, SUB_General);				// Also undocumented with codes: DD98 & FD98
	// Wit C
	_INST_FROM (0x99,	1, 4, 4,	"SBC A,C",				SBC_AWithC, SUB_General);				// Also undocumented with codes: DD99 & FD99
	// Wih D
	_INST_FROM (0x9A,	1, 4, 4,	"SBC A,D",				SBC_AWithD, SUB_General);				// Also undocumented with codes: DD9A & FD9A
	// Wth E
	_INST_FROM (0x9B,	1, 4, 4,	"SBC A,E",				SBC_AWithE, SUB_General);				// Also undocumented with codes: DD9B & FD9B
	// Wih H
	_INST_FROM (0x9C,	1, 4, 4,	"SBC A,H",				SBC_AWithH, SUB_General);
	// Wih L
	_INST_FROM (0x9D,	1, 4, 4,	"SBC A,L",				SBC_AWithL, SUB_General);
	// With (HL)
	_INST_FROM (0x9E,	1, 7, 7,	"SBC A,(HL)",			SBC_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INST_FROM (0xDD9E, 3, 19, 19,	"SBC A,(IX+[#1])",		SBC_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INST_FROM (0xFD9E,	3, 19, 19,	"SBC A,(IY+[#1])",		SBC_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INST_FROM (0xDE,	2, 7, 7,	"SBC A,[#1]",			SBC_A, SUB_General);
	// With HL and BC
	_INST_FROM (0xED42,	2, 15, 15,	"SBC HL,BC",			SBC_HLWithBC, SUB_General);
	// With HL and DE
	_INST_FROM (0xED52,	2, 15, 15,	"SBC HL,DE",			SBC_HLWithDE, SUB_General);
	// With HL and HL
	_INST_FROM (0xED62,	2, 15, 15,	"SBC HL,HL",			SBC_HLWithHL, SUB_General);
	// With HL and SP
	_INST_FROM (0xED72,	2, 15, 15,	"SBC HL,SP",			SBC_HLWithSP, SUB_General);
	// With A and IXH, IXL, IYH or IYL
	_INST_FROM (0xDD9C,	2, 8, 8,	"SBC A,IXH",			SBC_AWithIXH, SUB_General);				// Undocumented
	_INST_FROM (0xDD9D,	2, 8, 8,	"SBC A,IXL",			SBC_AWithIXL, SUB_General);				// Undocumented
	_INST_FROM (0xFD9C,	2, 8, 8,	"SBC A,IYH",			SBC_AWithIYH, SUB_General);				// Undocumented
	_INST_FROM (0xFD9D,	2, 8, 8,	"SBC A,IYL",			SBC_AWithIYL, SUB_General);				// Undocumented
}

#endif
  
// End of the file
/*@}*/
