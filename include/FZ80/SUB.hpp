/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: SUB.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of SUB instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_SUBINSTRUCTIONS__
#define __FZX80_SUBINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** SUB is always over the value of A. */
	class SUB_General : public Instruction
	{
		public:
		SUB_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		/** With a value. \n
			The 8 bit operations are always done take A register as destination. \n
			The value of the carry is or not taken into account according with the parameter c. \n
			No carry is taken into account by default. */
		inline bool executeWith (const MCHEmul::UByte& v, bool c = false);
		/** With a complex register and a value. 
			All 16 bit operations are SBC type, so carry has to be taken into account. \n
			All 16 bit operations have HL register as destination. */
		inline bool executeWithToHL (const MCHEmul::UBytes& v);
	};

	// ---
	inline bool SUB_General::executeWith (const MCHEmul::UByte& v, bool c)
	{
		MCHEmul::Register& r = registerA ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		bool a = c && st.bitStatus (FZ80::CZ80::_CARRYFLAG);
		MCHEmul::UInt rst = MCHEmul::UInt (r.values ()[0]).substract (MCHEmul::UInt (v), !a); 
		// In Z80 al substractions are "natural" substractions (so with carry)...
		// So when the carry is set behaves in the other way around!
		// To calculate the half borrow...
		int rV = (int) r.values ()[0].value ();
		int cr = rV ^ ((int) v.value ()) ^ (rV - ((int) v.value ()) - (a ? 1 : 0)); // It is a int operation, 
																					// so - it is already always with carry

		r.set (rst.values ()); // and stored back...

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, !rst.carry ()); // Borrow = !carry
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // Always in substractions!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// ---
	inline bool SUB_General::executeWithToHL (const MCHEmul::UBytes& v)
	{
		MCHEmul::RefRegisters& r = registerHL ();
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		bool a = st.bitStatus (FZ80::CZ80::_CARRYFLAG); 
		// Takes into account the carry flag! (it is always a SBC instruction)
		MCHEmul::UInt rV = MCHEmul::UInt ({ r [0] -> values ()[0], r [1] -> values ()[0] }); 
		MCHEmul::UInt vV = MCHEmul::UInt ({ v [0], v [1] });
		// In Z80 al substractions are "natural" substractions (so with carry)...
		// So when the carry is set behaves in the other way around!
		MCHEmul::UInt rst  = rV.substract (vV, !a);
		// To calculate the half borrow in the upper byte...
		int rVI = rV.asInt (); int vVI = vV.asInt ();
		int cr = rVI ^ vVI ^ (rVI - vVI - (a ? 1 : 0)); // It is an int operation,
														// so - is always with carry, that has to be removed later.

		// Put back the value...
		r [0] -> set ({ rst.values ()[0] }); 
		r [1] -> set ({ rst.values ()[1] }); 

		// How the flags are affected...
		st.setBitStatus (CZ80::_CARRYFLAG, !rst.carry ()); // Borrow is = !carry
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // Always! 
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // a copy of the status of the bit 3 of the MSB... but undocumented!
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x1000) != 0x0000);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // a copy of the status of the bit 5 of the MSB... but undocumented!
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UInt ({ 0, 0 }) /** has always two bytes. */);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// Without carry
	// With A
	_INSTZ80_FROM (0x97,	1, 4, { },	"SUB A",				SUB_AWithA, SUB_General);
	_INSTZ80_FROM (0xDD97,	2, 8, { },	"U1SUB A",				U1SUB_AWithA, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD97,	2, 8, { },	"U2SUB A",				U2SUB_AWithA, SUB_General);				// Undocumented
	// With B
	_INSTZ80_FROM (0x90,	1, 4, { },	"SUB B",				SUB_AWithB, SUB_General);
	_INSTZ80_FROM (0xDD90,	2, 8, { },	"U1SUB B",				U1SUB_AWithB, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD90,	2, 8, { },	"U2SUB B",				U2SUB_AWithB, SUB_General);				// Undocumented
	// Wit C
	_INSTZ80_FROM (0x91,	1, 4, { },	"SUB C",				SUB_AWithC, SUB_General);
	_INSTZ80_FROM (0xDD91,	2, 8, { },	"U1SUB C",				U1SUB_AWithC, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD91,	2, 8, { },	"U2SUB C",				U2SUB_AWithC, SUB_General);				// Undocumented
	// Wih D
	_INSTZ80_FROM (0x92,	1, 4, { },	"SUB D",				SUB_AWithD, SUB_General);
	_INSTZ80_FROM (0xDD92,	2, 8, { },	"U1SUB D",				U1SUB_AWithD, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD92,	2, 8, { },	"U2SUB D",				U2SUB_AWithD, SUB_General);				// Undocumented
	// Wth E
	_INSTZ80_FROM (0x93,	1, 4, { },	"SUB E",				SUB_AWithE, SUB_General);
	_INSTZ80_FROM (0xDD93,	2, 8, { },	"U1SUB E",				U1SUB_AWithE, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD93,	2, 8, { },	"U2SUB E",				U2SUB_AWithE, SUB_General);				// Undocumented
	// Wih H
	_INSTZ80_FROM (0x94,	1, 4, { },	"SUB H",				SUB_AWithH, SUB_General);
	// Wih L
	_INSTZ80_FROM (0x95,	1, 4, { },	"SUB L",				SUB_AWithL, SUB_General);
	// With (HL)
	_INSTZ80_FROM (0x96,	1, 7, { },	"SUB (HL)",				SUB_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INSTZ80_FROM (0xDD96, 3, 19, { },	"SUB (IX+[#1])",		SUB_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INSTZ80_FROM (0xFD96,	3, 19, { },	"SUB (IY+[#1])",		SUB_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INSTZ80_FROM (0xD6,	2, 7, { },	"SUB [#1]",				SUB_A, SUB_General);
	// Notice that there is no 16 bit SUB instructions
	// With A and IXH, IXL, IYH or IYL
	_INSTZ80_FROM (0xDD94,	2, 8, { },	"SUB IXH",				SUB_AWithIXH, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xDD95,	2, 8, { },	"SUB IXL",				SUB_AWithIXL, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD94,	2, 8, { },	"SUB IYH",				SUB_AWithIYH, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD95,	2, 8, { },	"SUB IYL",				SUB_AWithIYL, SUB_General);				// Undocumented

	// With carry
	// With A
	_INSTZ80_FROM (0x9F,	1, 4, { },	"SBC A,A",				SBC_AWithA, SUB_General);
	_INSTZ80_FROM (0xDD9F,	2, 8, { },	"U1SBC A,A",			U1SBC_AWithA, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD9F,	2, 8, { },	"U2SBC A,A",			U2SBC_AWithA, SUB_General);				// Undocumented
	// With B
	_INSTZ80_FROM (0x98,	1, 4, { },	"SBC A,B",				SBC_AWithB, SUB_General);
	_INSTZ80_FROM (0xDD98,	2, 8, { },	"U1SBC A,B",			U1SBC_AWithB, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD98,	2, 8, { },	"U2SBC A,B",			U2SBC_AWithB, SUB_General);				// Undocumented
	// Wit C
	_INSTZ80_FROM (0x99,	1, 4, { },	"SBC A,C",				SBC_AWithC, SUB_General);
	_INSTZ80_FROM (0xDD99,	2, 8, { },	"U1SBC A,C",			U1SBC_AWithC, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD99,	2, 8, { },	"U2SBC A,C",			U2SBC_AWithC, SUB_General);				// Undocumented
	// Wih D
	_INSTZ80_FROM (0x9A,	1, 4, { },	"SBC A,D",				SBC_AWithD, SUB_General);
	_INSTZ80_FROM (0xDD9A,	2, 8, { },	"U1SBC A,D",			U1SBC_AWithD, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD9A,	2, 8, { },	"U2SBC A,D",			U2SBC_AWithD, SUB_General);				// Undocumented
	// Wth E
	_INSTZ80_FROM (0x9B,	1, 4, { },	"SBC A,E",				SBC_AWithE, SUB_General);
	_INSTZ80_FROM (0xDD9B,	2, 8, { },	"U1SBC A,E",			U1SBC_AWithE, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD9B,	2, 8, { },	"U2SBC A,E",			U2SBC_AWithE, SUB_General);				// Undocumented
	// Wih H
	_INSTZ80_FROM (0x9C,	1, 4, { },	"SBC A,H",				SBC_AWithH, SUB_General);
	// Wih L
	_INSTZ80_FROM (0x9D,	1, 4, { },	"SBC A,L",				SBC_AWithL, SUB_General);
	// With (HL)
	_INSTZ80_FROM (0x9E,	1, 7, { },	"SBC A,(HL)",			SBC_AWithIndirectHL, SUB_General);
	// With(IX + d)
	_INSTZ80_FROM (0xDD9E, 3, 19, { },	"SBC A,(IX+[#1])",		SBC_AWithIndirectIndexIX, SUB_General);
	// With (IX + d)
	_INSTZ80_FROM (0xFD9E,	3, 19, { },	"SBC A,(IY+[#1])",		SBC_AWithIndirectIndexIY, SUB_General);
	// With Value
	_INSTZ80_FROM (0xDE,	2, 7, { },	"SBC A,[#1]",			SBC_A, SUB_General);
	// With HL and BC
	_INSTZ80_FROM (0xED42,	2, 15, { },	"SBC HL,BC",			SBC_HLWithBC, SUB_General);
	// With HL and DE
	_INSTZ80_FROM (0xED52,	2, 15, { },	"SBC HL,DE",			SBC_HLWithDE, SUB_General);
	// With HL and HL
	_INSTZ80_FROM (0xED62,	2, 15, { },	"SBC HL,HL",			SBC_HLWithHL, SUB_General);
	// With HL and SP
	_INSTZ80_FROM (0xED72,	2, 15, { },	"SBC HL,SP",			SBC_HLWithSP, SUB_General);
	// With A and IXH, IXL, IYH or IYL
	_INSTZ80_FROM (0xDD9C,	2, 8, { },	"SBC A,IXH",			SBC_AWithIXH, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xDD9D,	2, 8, { },	"SBC A,IXL",			SBC_AWithIXL, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD9C,	2, 8, { },	"SBC A,IYH",			SBC_AWithIYH, SUB_General);				// Undocumented
	_INSTZ80_FROM (0xFD9D,	2, 8, { },	"SBC A,IYL",			SBC_AWithIYL, SUB_General);				// Undocumented
}

#endif
  
// End of the file
/*@}*/
