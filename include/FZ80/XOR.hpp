/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: XOR.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of XOR instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_XORINSTRUCTIONS__
#define __FZX80_XORINSTRUCTIONS__

namespace FZ80
{
	/** XOR is always over the value of A. \n
		The flags are affected according with the final result stored in A. */
	class XOR_General : public Instruction
	{
		public:
		XOR_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xAF,	1, 4, 4,	"XOR A",				XOR_A, XOR_General);					// Also undocumented with codes: DDAF & FDAF
	// WithB
	_INST_FROM (0xA8,	1, 4, 4,	"XOR B",				XOR_B, XOR_General);					// Also undocumented with codes: DDA8 & FDA8
	// Wit C
	_INST_FROM (0xA9,	1, 4, 4,	"XOR C",				XOR_C, XOR_General);					// Also undocumented with codes: DDA9 & FDA9
	// Wih D
	_INST_FROM (0xAA,	1, 4, 4,	"XOR D",				XOR_D, XOR_General);					// Also undocumented with codes: DDAA & FDAA
	// Wth E
	_INST_FROM (0xAB,	1, 4, 4,	"XOR E",				XOR_E, XOR_General);					// Also undocumented with codes: DDAB & FDAB
	// With H
	_INST_FROM (0xAC,	1, 4, 4,	"XOR H",				XOR_H, XOR_General);
	// With L
	_INST_FROM (0xAD,	1, 4, 4,	"XOR L",				XOR_L, XOR_General);
	// With (HL)
	_INST_FROM (0xAE,	1, 7, 7,	"XOR (HL)",				XOR_IndirectHL, XOR_General);
	// With (IX + d)
	_INST_FROM (0xDDAE, 3, 19, 19,	"XOR (IX+[#1])",		XOR_IndirectIndexIX, XOR_General);
	// With (IX + d)
	_INST_FROM (0xFDAE,	3, 19, 19,	"XOR (IY+[#1])",		XOR_IndirectIndexIY, XOR_General);
	// With Value
	_INST_FROM (0xEE,	2, 7, 7,	"XOR [#1]",				XOR, XOR_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDAC,	2, 8, 8,	"XOR IXH",				XOR_IXH, XOR_General);					// Undocumented
	_INST_FROM (0xDDAD,	2, 8, 8,	"XOR IXL",				XOR_IXL, XOR_General);					// Undocumented
	_INST_FROM (0xFDAC,	2, 8, 8,	"XOR IYH",				XOR_IYH, XOR_General);					// Undocumented
	_INST_FROM (0xFDAD,	2, 8, 8,	"XOR IYL",				XOR_IYL, XOR_General);					// Undocumented
}

#endif
  
// End of the file
/*@}*/
