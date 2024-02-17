/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: CP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of CP instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_CPINSTRUCTIONS__
#define __FZX80_CPINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

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
		inline bool executeWith (const MCHEmul::UByte& v);
	};

	// ---
	inline bool CP_General::executeWith (const MCHEmul::UByte& v)
	{
		MCHEmul::Register& r = registerA ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]) - MCHEmul::UInt (v);
		MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f) - MCHEmul::UInt (v & 0x0f); // Just to calculate the half borrow!

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, rst.carry ());
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // Always!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, v.bit (3)); // Undocuented... Not from A - s but from s!
		st.setBitStatus (CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half borrow!
		st.setBitStatus (CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...Not from A - s but from s!
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// With A
	_INST_FROM (0xBF,	1, 4, 4,	"CP A",					CP_WithA, CP_General);
	_INST_FROM (0xDDBF,	2, 8, 8,	"U1CP A",				U1CP_WithA, CP_General);				// Undocumented
	_INST_FROM (0xFDBF,	2, 8, 8,	"U2CP A",				U2CP_WithA, CP_General);				// Undocumented
	// With B
	_INST_FROM (0xB8,	1, 4, 4,	"CP B",					CP_WithB, CP_General);
	_INST_FROM (0xDDB8,	2, 8, 8,	"U1CP B",				U1CP_WithB, CP_General);				// Undocumented
	_INST_FROM (0xFDB8,	2, 8, 8,	"U2CP B",				U2CP_WithB, CP_General);				// Undocumented
	// With C
	_INST_FROM (0xB9,	1, 4, 4,	"CP C",					CP_WithC, CP_General);
	_INST_FROM (0xDDB9,	2, 8, 8,	"U1CP C",				U1CP_WithC, CP_General);				// Undocumented
	_INST_FROM (0xFDB9,	2, 8, 8,	"U2CP C",				U2CP_WithC, CP_General);				// Undocumented
	// With D
	_INST_FROM (0xBA,	1, 4, 4,	"CP D",					CP_WithD, CP_General);
	_INST_FROM (0xDDBA,	2, 8, 8,	"U1CP D",				U1CP_WithD, CP_General);				// Undocumented
	_INST_FROM (0xFDBA,	2, 8, 8,	"U2CP D",				U2CP_WithD, CP_General);				// Undocumented
	// With E
	_INST_FROM (0xBB,	1, 4, 4,	"CP E",					CP_WithE, CP_General);
	_INST_FROM (0xDDBB,	2, 8, 8,	"U1CP E",				U1CP_WithE, CP_General);				// Undocumented
	_INST_FROM (0xFDBB,	2, 8, 8,	"U2CP E",				U2CP_WithE, CP_General);				// Undocumented
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
