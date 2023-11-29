/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: OR.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of OR instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ORINSTRUCTIONS__
#define __FZX80_ORINSTRUCTIONS__

namespace FZ80
{
	/** OR is always over the value of A. \n
		The flags are affected according with the final result stored in A. */
	class OR_General : public Instruction
	{
		public:
		OR_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xB7,	1, 4, 4,	"OR A",					OR_A, OR_General);						// Also undocumented with codes: DDB7 & FDB7 
	// With B
	_INST_FROM (0xB0,	1, 4, 4,	"OR B",					OR_B, OR_General);						// Also undocumented with codes: DDB0 & FDB0
	// With C
	_INST_FROM (0xB1,	1, 4, 4,	"OR C",					OR_C, OR_General);						// Also undocumented with codes: DDB1 & FDB1
	// With D
	_INST_FROM (0xB2,	1, 4, 4,	"OR D",					OR_D, OR_General);						// Also undocumented with codes: DDB2 & FDB2
	// With E
	_INST_FROM (0xB3,	1, 4, 4,	"OR E",					OR_E, OR_General);						// Also undocumented with codes: DDB3 & FDB3
	// With H
	_INST_FROM (0xB4,	1, 4, 4,	"OR H",					OR_H, OR_General);
	// With L
	_INST_FROM (0xB5,	1, 4, 4,	"OR L",					OR_L, OR_General);
	// With (HL)
	_INST_FROM (0xB6,	1, 7, 7,	"OR (HL)",				OR_IndirectHL, OR_General);
	// With (IX + d)
	_INST_FROM (0xDDB6, 3, 19, 19,	"OR (IX+[#1])",			OR_IndirectIndexIX, OR_General);
	// With (IX + d)
	_INST_FROM (0xFDB6,	3, 19, 19,	"OR (IY+[#1])",			OR_IndirectIndexIY, OR_General);
	// With Value
	_INST_FROM (0xF6,	2, 7, 7,	"OR [#1]",				OR, OR_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDB4,	2, 8, 8,	"OR IXH",				OR_IXH, OR_General);					// Undocumented
	_INST_FROM (0xDDB5,	2, 8, 8,	"OR IXL",				OR_IXL, OR_General);					// Undocumented
	_INST_FROM (0xFDB4,	2, 8, 8,	"OR IYH",				OR_IYH, OR_General);					// Undocumented
	_INST_FROM (0xFDB5,	2, 8, 8,	"OR IYL",				OR_IYL, OR_General);					// Undocumented
}

#endif
  
// End of the file
/*@}*/
