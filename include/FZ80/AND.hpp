/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ADD.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of AND instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ANDINSTRUCTIONS__
#define __FZX80_ANDINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** AND is always over the value of A. \n
		The flags are affected according with the final result stored in A. */
	class AND_General : public Instruction
	{
		public:
		AND_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		inline bool executeWith (const MCHEmul::UByte& v);
	};

	// ---
	inline bool AND_General::executeWith (const MCHEmul::UByte& v)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte r = a.values ()[0] & v;
		a.set ({ r }); // and stored it back...

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, false);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (r.numberBitsOn () % 2) == 0); // If it is an even number then it is set...
		st.setBitStatus (CZ80::_BIT3FLAG, r.bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, true);
		st.setBitStatus (CZ80::_BIT5FLAG, r.bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, r == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, r.bit (7));

		return (true);
	}

	// With A
	_INST_FROM (0xA7,	1, 4, 4,	"AND A",					AND_A, AND_General);				// Also undocumented with codes: DDA7 & FDA7 
	// With B
	_INST_FROM (0xA0,	1, 4, 4,	"AND B",					AND_B, AND_General);				// Also undocumented with codes: DDA0 & FDA0
	// With C
	_INST_FROM (0xA1,	1, 4, 4,	"AND C",					AND_C, AND_General);				// Also undocumented with codes: DDA1 & FDA1
	// With D
	_INST_FROM (0xA2,	1, 4, 4,	"AND D",					AND_D, AND_General);				// Also undocumented with codes: DDA2 & FDA2
	// With E
	_INST_FROM (0xA3,	1, 4, 4,	"AND E",					AND_E, AND_General);				// Also undocumented with codes: DDA3 & FDA3
	// With H
	_INST_FROM (0xA4,	1, 4, 4,	"AND H",					AND_H, AND_General);
	// With L
	_INST_FROM (0xA5,	1, 4, 4,	"AND L",					AND_L, AND_General);
	// With (HL)
	_INST_FROM (0xA6,	1, 7, 7,	"AND (HL)",					AND_IndirectHL, AND_General);
	// With (IX + d)
	_INST_FROM (0xDDA6, 3, 19, 19,	"AND (IX+[#1])",			AND_IndirectIndexIX, AND_General);
	// With (IX + d)
	_INST_FROM (0xFDA6,	3, 19, 19,	"AND (IY+[#1])",			AND_IndirectIndexIY, AND_General);
	// With Value
	_INST_FROM (0xE6,	2, 7, 7,	"AND [#1]",					AND, AND_General);
	// With IXH, IXL, IYH and IYL
	_INST_FROM (0xDDA4,	2, 8, 8,	"AND IXH",					AND_IXH, AND_General);				// Undocumented
	_INST_FROM (0xDDA5,	2, 8, 8,	"AND IXL",					AND_IXL, AND_General);				// Undocumented
	_INST_FROM (0xFDA4,	2, 8, 8,	"AND IYH",					AND_IYH, AND_General);				// Undocumented
	_INST_FROM (0xFDA5,	2, 8, 8,	"AND IYL",					AND_IYL, AND_General);				// Undocumented
}

#endif
  
// End of the file
/*@}*/
