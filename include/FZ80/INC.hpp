/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INC.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of INC instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INCINSTRUCTIONS__
#define __FZX80_INCINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Increment the value of several things. */
	class INC_General : public Instruction
	{
		public:
		INC_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline bool executeWith (MCHEmul::Register& r);
		inline bool executeWith (MCHEmul::RefRegisters& r);
		inline bool executeWith (const MCHEmul::Address& a);
	};

	// ---
	inline bool INC_General::executeWith (MCHEmul::Register& r)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte rD;
		MCHEmul::UInt rst (rD = r.values ()[0]); // 1 byte long...
		rst += MCHEmul::UInt::_1; // INCrement... (with no carry)
		// The length of the value doesn't change...
		// To calculate the half carry...
		int rV = (int) rD.value ();
		int cr = rV ^ 0x01 ^ (rV + 0x01); // It is a int operation, 
										  // so - it is already always with carry

		r.set (rst.bytes ()); // Put back the info

		// How the flags are affected...
		// The carry flag is not modified...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Never in addition!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// ---
	inline bool INC_General::executeWith (MCHEmul::RefRegisters& r)
	{
		// The operation...
		MCHEmul::UInt rst ({ r [0] -> values ()[0], r [1] -> values ()[0] });
		rst += MCHEmul::UInt::_1; // Increment...

		r [0] -> set ({ rst [0] }); // Put back the info
		r [1] -> set ({ rst [1] });

		// With double registers (16 bits operation) there is no impact in the flags

		return (true);
	}

	// ---
	inline bool INC_General::executeWith (const MCHEmul::Address& a)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte vD;
		MCHEmul::UInt rst (vD = memory () -> value (a));
		rst += MCHEmul::UInt::_1; // INCrement...
		// To calculate the half carry...
		int vV = (int) vD.value ();
		int cr = vV ^ 0x01 ^ (vV + 0x01); // It is a int operation, 
										  // so - it is already always with carry

		// Put is back in the memory...
		memory () -> set (a, rst.values ()); // !byte long always...

		// How the flags are affected...
		// Carry flag is not modified...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Just on when the last instruction is a substract!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// With A
	_INSTZ80_FROM (0x3C,	1, 4, { },	"INC A",				INC_A, INC_General);
	_INSTZ80_FROM (0xDD3C,	2, 8, { },	"U1INC A",				U1INC_A, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD3C,	2, 8, { },	"U2INC A",				U2INC_A, INC_General);					// Undocumented
	// With B
	_INSTZ80_FROM (0x04,	1, 4, { },	"INC B",				INC_B, INC_General);
	_INSTZ80_FROM (0xDD04,	2, 8, { },	"U1INC B",				U1INC_B, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD04,	2, 8, { },	"U2INC B",				U2INC_B, INC_General);					// Undocumented
	// With C
	_INSTZ80_FROM (0x0C,	1, 4, { },	"INC C",				INC_C, INC_General);
	_INSTZ80_FROM (0xDD0C,	2, 8, { },	"U1INC C",				U1INC_C, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD0C,	2, 8, { },	"U2INC C",				U2INC_C, INC_General);					// Undocumented
	// With D
	_INSTZ80_FROM (0x14,	1, 4, { },	"INC D",				INC_D, INC_General);
	_INSTZ80_FROM (0xDD14,	2, 8, { },	"U1INC D",				U1INC_D, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD14,	2, 8, { },	"U2INC D",				U2INC_D, INC_General);					// Undocumented
	// With E
	_INSTZ80_FROM (0x1C,	1, 4, { },	"INC E",				INC_E, INC_General);
	_INSTZ80_FROM (0xDD1C,	2, 8, { },	"U1INC E",				U1INC_E, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD1C,	2, 8, { },	"U2INC E",				U2INC_E, INC_General);					// Undocumented
	// With F
	_INSTZ80_FROM (0x24,	1, 4, { },	"INC H",				INC_H, INC_General);
	// With L
	_INSTZ80_FROM (0x2C,	1, 4, { },	"INC L",				INC_L, INC_General);
	// With (HL)
	_INSTZ80_FROM (0x34,	1, 11, { },	"INC (HL)",				INC_IndirectHL, INC_General);
	// With(IX + d)
	_INSTZ80_FROM (0xDD34, 3, 23, { },	"INC (IX+[#1])",		INC_IndirectIndexIX, INC_General);
	// With (IX + d)
	_INSTZ80_FROM (0xFD34,	3, 23, { }, "INC (IY+[#1])",		INC_IndirectIndexIY, INC_General);
	// With BC
	_INSTZ80_FROM (0x03,	1, 6, { },	"INC BC",				INC_BC, INC_General);
	// With DE
	_INSTZ80_FROM (0x13,	1, 6, { },	"INC DE",				INC_DE, INC_General);
	// With HL
	_INSTZ80_FROM (0x23,	1, 6, { },	"INC HL",				INC_HL, INC_General);
	// With SP
	_INSTZ80_FROM (0x33,	1, 6, { },	"INC SP",				INC_SP, INC_General);
	// With IX
	_INSTZ80_FROM (0xDD23,	2, 10, { },	"INC IX",				INC_IX, INC_General);
	// With IY
	_INSTZ80_FROM (0xFD23,	2, 10, { },	"INC IY",				INC_IY, INC_General);
	// With IXH, IXL, IYH or IYL
	_INSTZ80_FROM (0xDD24,	2, 8, { },	"INC IXH",				INC_IXH, INC_General);					// Undocumented
	_INSTZ80_FROM (0xDD2C,	2, 8, { },	"INC IXL",				INC_IXL, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD24,	2, 8, { },	"INC IYH",				INC_IYH, INC_General);					// Undocumented
	_INSTZ80_FROM (0xFD2C,	2, 8, { },	"INC IYL",				INC_IYL, INC_General);					// Undocumented
}

#endif
  
// End of the file
/*@}*/
