/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ADD.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		AND_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
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
		MCHEmul::UByte rst = a.values ()[0] & v;
		a.set ({ rst }); // and stored it back...

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, false);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (rst.numberBitsOn () % 2) == 0); // If it is an even number then it is set...
		st.setBitStatus (CZ80::_BIT3FLAG, rst.bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, true);
		st.setBitStatus (CZ80::_BIT5FLAG, rst.bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.bit (7));

		return (true);
	}

	// With A
	_INSTZ80_FROM (0xA7,	1, 4, { },	"AND A",					AND_A, AND_General);
	_INSTZ80_FROM (0xDDA7,	2, 8, { },	"U1AND A",					U1AND_A, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA7,	2, 8, { },	"U2AND A",					U2AND_A, AND_General);				// Undocumented
	// With B
	_INSTZ80_FROM (0xA0,	1, 4, { },	"AND B",					AND_B, AND_General);				
	_INSTZ80_FROM (0xDDA0,	2, 8, { },	"U1AND B",					U1AND_B, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA0,	2, 8, { },	"U2AND B",					U2AND_B, AND_General);				// Undocumented
	// With C
	_INSTZ80_FROM (0xA1,	1, 4, { },	"AND C",					AND_C, AND_General);
	_INSTZ80_FROM (0xDDA1,	2, 8, { },	"U1AND C",					U1AND_C, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA1,	2, 8, { },	"U2AND C",					U2AND_C, AND_General);				// Undocumented
	// With D
	_INSTZ80_FROM (0xA2,	1, 4, { },	"AND D",					AND_D, AND_General);
	_INSTZ80_FROM (0xDDA2,	2, 8, { },	"U1AND D",					U1AND_D, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA2,	2, 8, { },	"U2AND D",					U2AND_D, AND_General);				// Undocumented
	// With E
	_INSTZ80_FROM (0xA3,	1, 4, { },	"AND E",					AND_E, AND_General);
	_INSTZ80_FROM (0xDDA3,	2, 8, { },	"U1AND E",					U1AND_E, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA3,	2, 8, { },	"U2AND E",					U2AND_E, AND_General);				// Undocumented
	// With H
	_INSTZ80_FROM (0xA4,	1, 4, { },	"AND H",					AND_H, AND_General);
	// With L
	_INSTZ80_FROM (0xA5,	1, 4, { },	"AND L",					AND_L, AND_General);
	// With (HL)
	_INSTZ80_FROM (0xA6,	1, 7, { },	"AND (HL)",					AND_IndirectHL, AND_General);
	// With (IX + d)
	_INSTZ80_FROM (0xDDA6, 3, 19, { },	"AND (IX+[#1])",			AND_IndirectIndexIX, AND_General);
	// With (IX + d)
	_INSTZ80_FROM (0xFDA6,	3, 19, { }, "AND (IY+[#1])",			AND_IndirectIndexIY, AND_General);
	// With Value
	_INSTZ80_FROM (0xE6,	2, 7, { },	"AND [#1]",					AND, AND_General);
	// With IXH, IXL, IYH and IYL
	_INSTZ80_FROM (0xDDA4,	2, 8, { },	"AND IXH",					AND_IXH, AND_General);				// Undocumented
	_INSTZ80_FROM (0xDDA5,	2, 8, { },	"AND IXL",					AND_IXL, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA4,	2, 8, { },	"AND IYH",					AND_IYH, AND_General);				// Undocumented
	_INSTZ80_FROM (0xFDA5,	2, 8, { },	"AND IYL",					AND_IYL, AND_General);				// Undocumented
}

#endif
  
// End of the file
/*@}*/
