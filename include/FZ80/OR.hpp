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

#include <FZ80/Instruction.hpp>

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
		inline bool executeWith (const MCHEmul::UByte& v);
	};

	// ---
	inline bool OR_General::executeWith (const MCHEmul::UByte& v)
	{
		MCHEmul::Register& a = registerA ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte r = a.values ()[0] | v;
		a.set ({ r }); // and stored it back...

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, false);
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (r.numberBitsOn () % 2) == 0); // If it isn even number then it is set...
		st.setBitStatus (CZ80::_BIT3FLAG, r.bit (3)); // a copy of the status of the bit 3... but undocumented!
		st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (CZ80::_BIT5FLAG, r.bit (5)); // a copy of the status of the bit 5... but undocumented!
		st.setBitStatus (CZ80::_ZEROFLAG, r == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, r.bit (7));

		return (true);
	}

	// With A
	_INST_FROM (0xB7,	1, 4, 4,	"OR A",					OR_A, OR_General);
	_INST_FROM (0xDDB7,	2, 8, 8,	"U1OR A",				U1OR_A, OR_General);					// Undocumented
	_INST_FROM (0xFDB7,	2, 8, 8,	"U2OR A",				U2OR_A, OR_General);					// Undocumented
	// With B
	_INST_FROM (0xB0,	1, 4, 4,	"OR B",					OR_B, OR_General);
	_INST_FROM (0xDDB0,	2, 8, 8,	"U1OR B",				U1OR_B, OR_General);					// Undocumented
	_INST_FROM (0xFDB0,	2, 8, 8,	"U2OR B",				U2OR_B, OR_General);					// Undocumented
	// With C
	_INST_FROM (0xB1,	1, 4, 4,	"OR C",					OR_C, OR_General);
	_INST_FROM (0xDDB1,	2, 8, 8,	"U1OR C",				U1OR_C, OR_General);					// Undocumented
	_INST_FROM (0xFDB1,	2, 8, 8,	"U2OR C",				U2OR_C, OR_General);					// Undocumented
	// With D
	_INST_FROM (0xB2,	1, 4, 4,	"OR D",					OR_D, OR_General);
	_INST_FROM (0xDDB2,	2, 8, 8,	"U1OR D",				U1OR_D, OR_General);					// Undocumented
	_INST_FROM (0xFDB2,	2, 8, 8,	"U2OR D",				U2OR_D, OR_General);					// Undocumented
	// With E
	_INST_FROM (0xB3,	1, 4, 4,	"OR E",					OR_E, OR_General);
	_INST_FROM (0xDDB3,	2, 8, 8,	"U1OR E",				U1OR_E, OR_General);					// Undocumented
	_INST_FROM (0xFDB3,	2, 8, 8,	"U2OR E",				U2OR_E, OR_General);					// Undocumented
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
