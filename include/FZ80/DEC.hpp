/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: DEC.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
		MCHEmul::UInt v  (r.values ()[0]);
		MCHEmul::UInt hv (r.values ()[0] & 0x0f); // Just half byte!
		bool i80 = (v == MCHEmul::UInt (MCHEmul::UByte (0x80)));
		v  -= MCHEmul::UInt::_1; // Decrement...
		hv -= MCHEmul::UInt::_1; // ..just to see whether there is half borrow!
		r.set (v.bytes ()); // Put back the info

		// How the flags are affected...
		// The carry flag is not modified...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, true); // as the last instruction has been a substract!
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, v.overflow ());
		st.setBitStatus (CZ80::_BIT3FLAG, v [0].bit (3)); // Undocumented...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, hv [0].bit (4)); // If true, there will have been half carry!
		st.setBitStatus (CZ80::_BIT5FLAG, v [0].bit (5)); // Undocumented...
		st.setBitStatus (CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, v.negative ());

		return (true);
	}

	// ---
	inline bool DEC_General::executeWith (MCHEmul::RefRegisters& r)
	{
		MCHEmul::UInt v  ({ r [0] -> values ()[0], r [1] -> values ()[0] });
		v  -= MCHEmul::UInt::_1; // Decrement
		r [0] -> set ({ v [0] }); // Put the info back...
		r [1] -> set ({ v [1] });

		return (true);
	}

	// ---
	inline bool DEC_General::executeWith (const MCHEmul::Address& a)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The operation...
		MCHEmul::UInt v (memory () -> value (a));
		bool i7F = (v == MCHEmul::UInt (MCHEmul::UByte (0x7f)));
		v -= 1; // DECrement...
		memory () -> set (a, v.values ()[0]);

		// How the flags are affected...
		st.setBitStatus (CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, i7F); // Only if it was 7F before the operation...
		st.setBitStatus (CZ80::_HALFCARRYFLAG, true);
		st.setBitStatus (CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (CZ80::_SIGNFLAG, v.negative ());

		return (true);
	}

	// With A
	_INST_FROM (0x3D,	1, 4, 4,	"DEC A",				DEC_A, DEC_General);					// Also undocumented with codes: DD3D & FD3D
	// With B
	_INST_FROM (0x05,	1, 4, 4,	"DEC B",				DEC_B, DEC_General);					// Also undocumented with codes: DD05 & FD05
	// Wit C
	_INST_FROM (0x0D,	1, 4, 4,	"DEC C",				DEC_C, DEC_General);					// Also undocumented with codes: DD0D & FD0D
	// Wih D
	_INST_FROM (0x15,	1, 4, 4,	"DEC D",				DEC_D, DEC_General);					// Also undocumented with codes: DD15 & FD15
	// Wth E
	_INST_FROM (0x1D,	1, 4, 4,	"DEC E",				DEC_E, DEC_General);					// Also undocumented with codes: DD1D & FD1D
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
	_INST_FROM (0xDD25,	2, 8, 8,	"DEC IXH",				DEC_IXH, DEC_General);						// Undocumented
	_INST_FROM (0xDD2D,	2, 8, 8,	"DEC IXL",				DEC_IXL, DEC_General);						// Undocumented
	_INST_FROM (0xFD25,	2, 8, 8,	"DEC IYH",				DEC_IYH, DEC_General);						// Undocumented
	_INST_FROM (0xFD2D,	2, 8, 8,	"DEC IYL",				DEC_IYL, DEC_General);						// Undocumented
}

#endif
  
// End of the file
/*@}*/
