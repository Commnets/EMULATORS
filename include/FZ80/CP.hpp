/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: CP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of CP instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_CPINSTRUCTIONS__
#define __FZX80_CPINSTRUCTIONS__

namespace FZ80
{
	/** CP, to compare always A register with a value. */
	class CP_General : public Instruction
	{
		public:
		CP_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** With a a value. */
		bool executeWith (const MCHEmul::UByte& v);
	};

	// With A
	_INST_FROM (0xBF,	1, 4, 4,	"CP A",					CP_WithA, CP_General);					// Also undocumented with codes: DDBF & FDBF
	// With B
	_INST_FROM (0xB8,	1, 4, 4,	"CP B",					CP_WithB, CP_General);					// Also undocumented with codes: DDB8 & FDB8
	// With C
	_INST_FROM (0xB9,	1, 4, 4,	"CP C",					CP_WithC, CP_General);					// Also undocumented with codes: DDB9 & FDB9
	// With D
	_INST_FROM (0xBA,	1, 4, 4,	"CP D",					CP_WithD, CP_General);					// Also undocumented with codes: DDBA & FDBA
	// With E
	_INST_FROM (0xBB,	1, 4, 4,	"CP E",					CP_WithE, CP_General);					// Also undocumented with codes: DDBB & FDBB
	// With F
	_INST_FROM (0xBC,	1, 4, 4,	"CP H",					CP_WithH, CP_General);
	// With L
	_INST_FROM (0xBD,	1, 4, 4,	"CP L",					CP_WithL, CP_General);
	// With (HL)
	_INST_FROM (0xBE,	1, 7, 7,	"CP (HL)",				CP_WithIndirectHL, CP_General);
	// With(IX + d)
	_INST_FROM (0xDDBE, 3, 19, 19,	"CP (IX+[#1])",			CP_WithIndirectIndexIX, CP_General);
	// With (IX + d)
	_INST_FROM (0xFDBE,	3, 19, 19,	"CP (IY+[#1])",			CP_WithIndirectIndexIY, CP_General);
	// With Value
	_INST_FROM (0xFE,	2, 7, 7,	"CP [#1]",				CP, CP_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDDBC,	2, 8, 8,	"CP IXH",				CP_WithIXH, CP_General);				// Undocumented
	_INST_FROM (0xDDBD,	2, 8, 8,	"CP IXL",				CP_WithIXL, CP_General);				// Undocumented
	_INST_FROM (0xFDBC,	2, 8, 8,	"CP IYH",				CP_WithIYH, CP_General);				// Undocumented
	_INST_FROM (0xFDBD,	2, 8, 8,	"CP IYL",				CP_WithIYL, CP_General);				// Undocumented
}

#endif
  
// End of the file
/*@}*/
