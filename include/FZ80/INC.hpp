/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INC.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of INC instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INCINSTRUCTIONS__
#define __FZX80_INCINSTRUCTIONS__

namespace FZ80
{
	/** Increment the value of several things. */
	class INC_General : public Instruction
	{
		public:
		INC_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		bool executeWith (MCHEmul::Register& r);
		bool executeWith (MCHEmul::RefRegisters& r);
		bool executeWith (const MCHEmul::Address& a);
	};

	// With A
	_INST_FROM (0x3C,	1, 4, 4,	"INC A",				INC_A, INC_General);					// Also undocumented with codes: DD3C & FD3C
	// With B
	_INST_FROM (0x04,	1, 4, 4,	"INC B",				INC_B, INC_General);					// Also undocumented with codes: DD04 & FD04
	// Wit C
	_INST_FROM (0x0C,	1, 4, 4,	"INC C",				INC_C, INC_General);					// Also undocumented with codes: DD0C & FD0C
	// Wih D
	_INST_FROM (0x14,	1, 4, 4,	"INC D",				INC_D, INC_General);					// Also undocumented with codes: DD14 & FD14
	// Wth E
	_INST_FROM (0x1C,	1, 4, 4,	"INC E",				INC_E, INC_General);					// Also undocumented with codes: DD1C & FD1C
	// Wih F
	_INST_FROM (0x24,	1, 4, 4,	"INC H",				INC_H, INC_General);
	// Wih L
	_INST_FROM (0x2C,	1, 4, 4,	"INC L",				INC_L, INC_General);
	// With (HL)
	_INST_FROM (0x34,	1, 11, 11,	"INC (HL)",				INC_IndirectHL, INC_General);
	// With(IX + d)
	_INST_FROM (0xDD34, 3, 23, 23,	"INC (IX+[#1])",		INC_IndirectIndexIX, INC_General);
	// With (IX + d)
	_INST_FROM (0xFD34,	3, 23, 23,	"INC (IY+[#1])",		INC_IndirectIndexIY, INC_General);
	// With BC
	_INST_FROM (0x03,	1, 6, 6,	"INC BC",				INC_BC, INC_General);
	// With DE
	_INST_FROM (0x13,	1, 6, 6,	"INC DE",				INC_DE, INC_General);
	// With HL
	_INST_FROM (0x23,	1, 6, 6,	"INC HL",				INC_HL, INC_General);
	// With SP
	_INST_FROM (0x33,	1, 6, 6,	"INC SP",				INC_SP, INC_General);
	// With IX
	_INST_FROM (0xDD23,	2, 10, 10,	"INC IX",				INC_IX, INC_General);
	// With IY
	_INST_FROM (0xFD23,	2, 10, 10,	"INC IY",				INC_IY, INC_General);
	// With IXH, IXL, IYH or IYL
	_INST_FROM (0xDD24,	2, 8, 8,	"INC IXH",				INC_IXH, INC_General);						// Undocumented
	_INST_FROM (0xDD2C,	2, 8, 8,	"INC IXL",				INC_IXL, INC_General);						// Undocumented
	_INST_FROM (0xFD24,	2, 8, 8,	"INC IYH",				INC_IYH, INC_General);						// Undocumented
	_INST_FROM (0xFD2C,	2, 8, 8,	"INC IYL",				INC_IYL, INC_General);						// Undocumented
}

#endif
  
// End of the file
/*@}*/
