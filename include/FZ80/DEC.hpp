/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: DEC.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of DEC instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_DECINSTRUCTIONS__
#define __FZX80_DECINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Decrement the value of several things. */
	class DEC_General : public Instruction
	{
		public:
		DEC_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		inline bool executeWith (MCHEmul::Register& r);
		inline bool executeWith (MCHEmul::RefRegisters& r);
		inline bool executeWith (const MCHEmul::Address& a);
	};

	// ---
	inline bool DEC_General::executeWith (MCHEmul::Register& r)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte rD;
		MCHEmul::UInt rst (rD = r.values ()[0]);
		rst -= MCHEmul::UInt::_1; // DECrement...1 byte long...
		// To calculate the half borrow...
		int rV = (int) rD.value ();
		int cr = rV ^ 0x01 ^ (rV - 0x01); // It is a int operation, 
										  // so - it is already always with carry

		r.set (rst.bytes ()); // Put back the info

		// How the flags are affected...
		// The carry flag is not modified...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // as the last instruction has been a substract!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// ---
	inline bool DEC_General::executeWith (MCHEmul::RefRegisters& r)
	{
		MCHEmul::UInt rst ({ r [0] -> values ()[0], r [1] -> values ()[0] });
		rst -= MCHEmul::UInt::_1; // Decrement

		r [0] -> set ({ rst [0] }); // Put the info back...
		r [1] -> set ({ rst [1] });

		// With double registers (16 bits operation) there is no impact in the flags

		return (true);
	}

	// ---
	inline bool DEC_General::executeWith (const MCHEmul::Address& a)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UByte vD;
		MCHEmul::UInt rst (vD = memory () -> value (a));
		rst -= 1; // DECrement...
		// To calculate the half borrow...
		int vV = (int) vD.value ();
		int cr = vV ^ 0x01 ^ (vV - 0x01); // It is a int operation, 
										  // so - it is already always with carry

		memory () -> set (a, rst.values ()[0]);

		// How the flags are affected...
		// Carry is not affected...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // Always when it is a substract!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, rst == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, rst.negative ());

		return (true);
	}

	// With A
	_INST_FROM (0x3D,	1, 4, 4,	"DEC A",				DEC_A, DEC_General);
	_INST_FROM (0xDD3D,	2, 8, 8,	"U1DEC A",				U1DEC_A, DEC_General);					// Undocumented
	_INST_FROM (0xFD3D,	2, 8, 8,	"U2DEC A",				U2DEC_A, DEC_General);					// Undocumented
	// With B
	_INST_FROM (0x05,	1, 4, 4,	"DEC B",				DEC_B, DEC_General);
	_INST_FROM (0xDD05,	2, 8, 8,	"U1DEC B",				U1DEC_B, DEC_General);					// Undocumented
	_INST_FROM (0xFD05,	2, 8, 8,	"U2DEC B",				U2DEC_B, DEC_General);					// Undocumented
	// Wit C
	_INST_FROM (0x0D,	1, 4, 4,	"DEC C",				DEC_C, DEC_General);
	_INST_FROM (0xDD0D,	2, 8, 8,	"U1DEC C",				U1DEC_C, DEC_General);					// Undocumented
	_INST_FROM (0xFD0D,	2, 8, 8,	"U2DEC C",				U2DEC_C, DEC_General);					// Undocumented
	// Wih D
	_INST_FROM (0x15,	1, 4, 4,	"DEC D",				DEC_D, DEC_General);
	_INST_FROM (0xDD15,	2, 8, 8,	"U1DEC D",				U1DEC_D, DEC_General);					// Undocumented
	_INST_FROM (0xFD15,	2, 8, 8,	"U2DEC D",				U2DEC_D, DEC_General);					// Undocumented
	// Wth E
	_INST_FROM (0x1D,	1, 4, 4,	"DEC E",				DEC_E, DEC_General);
	_INST_FROM (0xDD1D,	2, 8, 8,	"U1DEC E",				U1DEC_E, DEC_General);					// Undocumented
	_INST_FROM (0xFD1D,	2, 8, 8,	"U2DEC E",				U2DEC_E, DEC_General);					// Undocumented
	// Wih F
	_INST_FROM (0x25,	1, 4, 4,	"DEC H",				DEC_H, DEC_General);
	// Wih L
	_INST_FROM (0x2D,	1, 4, 4,	"DEC L",				DEC_L, DEC_General);
	// With (HL)
	_INST_FROM (0x35,	1, 11, 11,	"DEC (HL)",				DEC_IndirectHL, DEC_General);
	// With(IX + d)
	_INST_FROM (0xDD35, 3, 23, 23,	"DEC (IX+[#1])",		DEC_IndirectIndexIX, DEC_General);
	// With (IX + d)
	_INST_FROM (0xFD35,	3, 23, 23,	"DEC (IY+[#1])",		DEC_IndirectIndexIY, DEC_General);
	// With BC
	_INST_FROM (0x0B,	1, 6, 6,	"DEC BC",				DEC_BC, DEC_General);
	// With DE
	_INST_FROM (0x1B,	1, 6, 6,	"DEC DE",				DEC_DE, DEC_General);
	// With HL
	_INST_FROM (0x2B,	1, 6, 6,	"DEC HL",				DEC_HL, DEC_General);
	// With SP
	_INST_FROM (0x3B,	1, 6, 6,	"DEC SP",				DEC_SP, DEC_General);
	// With IX
	_INST_FROM (0xDD2B,	2, 10, 10,	"DEC IX",				DEC_IX, DEC_General);
	// With IY
	_INST_FROM (0xFD2B,	2, 10, 10,	"DEC IY",				DEC_IY, DEC_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDD25,	2, 8, 8,	"DEC IXH",				DEC_IXH, DEC_General);					// Undocumented
	_INST_FROM (0xDD2D,	2, 8, 8,	"DEC IXL",				DEC_IXL, DEC_General);					// Undocumented
	_INST_FROM (0xFD25,	2, 8, 8,	"DEC IYH",				DEC_IYH, DEC_General);					// Undocumented
	_INST_FROM (0xFD2D,	2, 8, 8,	"DEC IYL",				DEC_IYL, DEC_General);					// Undocumented
}

#endif
  
// End of the file
/*@}*/
