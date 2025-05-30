/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: XOR.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of XOR instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_XORINSTRUCTIONS__
#define __FZX80_XORINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** XOR is always over the value of A. \n
		The flags are affected according with the final result stored in A. */
	class XOR_General : public Instruction
	{
		public:
		XOR_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		/** And is also done with the value of the accumulator. */
		inline bool executeWith (const MCHEmul::UByte& v);
	};

	// ---
	bool XOR_General::executeWith (const MCHEmul::UByte& v)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte rst = a.values ()[0] ^ v;
		a.set ({ rst }); // and stored it back...

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, false);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (rst.numberBitsOn () % 2) == 0); // If it isn even number then it is set...
		st.setBitStatus (CZ80::_BIT3FLAG, rst.bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (CZ80::_BIT5FLAG, rst.bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.bit (7));

		return (true);
	}

	// With A
	_INSTZ80_FROM (0xAF,	1, 4, { },	"XOR A",				XOR_A, XOR_General);
	_INSTZ80_FROM (0xDDAF,	2, 8, { },	"U1XOR A",				U1XOR_A, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDAF,	2, 8, { },	"U2XOR A",				U2XOR_A, XOR_General);					// Undocumented
	// WithB
	_INSTZ80_FROM (0xA8,	1, 4, { },	"XOR B",				XOR_B, XOR_General);
	_INSTZ80_FROM (0xDDA8,	2, 8, { },	"U1XOR B",				U1XOR_B, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDA8,	2, 8, { },	"U2XOR B",				U2XOR_B, XOR_General);					// Undocumented
	// Wit C
	_INSTZ80_FROM (0xA9,	1, 4, { },	"XOR C",				XOR_C, XOR_General);
	_INSTZ80_FROM (0xDDA9,	2, 8, { },	"U1XOR C",				U1XOR_C, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDA9,	2, 8, { },	"U2XOR C",				U2XOR_C, XOR_General);					// Undocumented
	// Wih D
	_INSTZ80_FROM (0xAA,	1, 4, { },	"XOR D",				XOR_D, XOR_General);
	_INSTZ80_FROM (0xDDAA,	2, 8, { },	"U1XOR D",				U1XOR_D, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDAA,	2, 8, { },	"U2XOR D",				U2XOR_D, XOR_General);					// Undocumented
	// Wth E
	_INSTZ80_FROM (0xAB,	1, 4, { },	"XOR E",				XOR_E, XOR_General);
	_INSTZ80_FROM (0xDDAB,	2, 8, { },	"U1XOR E",				U1XOR_E, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDAB,	2, 8, { },	"U2XOR E",				U2XOR_E, XOR_General);					// Undocumented
	// With H
	_INSTZ80_FROM (0xAC,	1, 4, { },	"XOR H",				XOR_H, XOR_General);
	// With L
	_INSTZ80_FROM (0xAD,	1, 4, { },	"XOR L",				XOR_L, XOR_General);
	// With (HL)
	_INSTZ80_FROM (0xAE,	1, 7, { },	"XOR (HL)",				XOR_IndirectHL, XOR_General);
	// With (IX + d)
	_INSTZ80_FROM (0xDDAE, 3, 19, { },	"XOR (IX+[#1])",		XOR_IndirectIndexIX, XOR_General);
	// With (IX + d)
	_INSTZ80_FROM (0xFDAE,	3, 19, { },	"XOR (IY+[#1])",		XOR_IndirectIndexIY, XOR_General);
	// With Value
	_INSTZ80_FROM (0xEE,	2, 7, { },	"XOR [#1]",				XOR, XOR_General);
	// With IXH, IXL, IYH and IYL
	_INSTZ80_FROM (0xDDAC,	2, 8, { },	"XOR IXH",				XOR_IXH, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xDDAD,	2, 8, { },	"XOR IXL",				XOR_IXL, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDAC,	2, 8, { },	"XOR IYH",				XOR_IYH, XOR_General);					// Undocumented
	_INSTZ80_FROM (0xFDAD,	2, 8, { },	"XOR IYL",				XOR_IYL, XOR_General);					// Undocumented
}

#endif
  
// End of the file
/*@}*/
