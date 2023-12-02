/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: EX.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of EX instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_EXINSTRUCTIONS__
#define __FZX80_EXINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** These instructions are used to interchange content between registers. \n
		None of these instructions affects the register flag! */
	class EX_General : public Instruction
	{
		public:
		EX_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** To exchange the context of two set of registers. \n
			These instructions don't affect either to the registers. */
		inline bool executeWith (MCHEmul::RefRegisters& r, MCHEmul::RefRegisters& rp);
	};

	// ---
	inline bool EX_General::executeWith (MCHEmul::RefRegisters& r, MCHEmul::RefRegisters& rp)
	{
		// Keep a copy of first register content..
		MCHEmul::UBytes o ({ r [0] -> values ()[0], r [1] -> values ()[0] });
		// ...Move the second register content into the first
		r  [0] -> set (MCHEmul::UBytes ({ rp [0]  -> values ()[0] }));
		r  [1] -> set (MCHEmul::UBytes ({ rp [1]  -> values ()[0] }));
		// ...Move the copy kept into the second register...
		rp [0] -> set (MCHEmul::UBytes ({ o  [0] }));
		rp [1] -> set (MCHEmul::UBytes ({ o  [1] }));

		return (true);
	}

	// AF and AF'
	_INST_FROM (0x08,	1, 4, 4,	"EX AF,AF'",			EX_AF, EX_General);
	// DE and HL
	_INST_FROM (0xEB,	1, 4, 4,	"EX DE,HL",				EX_DEWithHL, EX_General);
	// EXX (all)
	_INST_FROM (0xD9,	1, 4, 4,	"EXX",					EX_X, EX_General);
	// With SP
	_INST_FROM (0xE3,	1, 19, 19,	"EX (SP),HL",			EX_SPWithHL, EX_General);
	_INST_FROM (0xDDE3,	2, 23, 23,	"EX (SP),IX",			EX_SPWithIX, EX_General);
	_INST_FROM (0xFDE3,	2, 23, 23,	"EX (SP),IY",			EX_SPWithIY, EX_General);

	/** To move a block of data. \n
		These instructions affects the flags in a similar way than DEC/INC instructions for LDD/LDDR/LDI/LDIR or 
		or CP for CPI/CPIR/CPD/CPDR. */
	class InstBlock_General : public Instruction
	{
		public:
		InstBlock_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t),
			  _inExecution (false),
			  _bc0 (false)
							{ }

		protected:
		/** The parameter a idicates the quantity to move up or down. \n
			It has to be -1 or 1. */
		bool executeMoveWith (int a);
		bool executeCompareWith (int a);

		// Implementation
		/** True when it is in execution. */
		bool _inExecution;
		/** True when _bc0. */
		bool _bc0;
	};

	// (DE) <- (HL) & Next memory position
	_INST_FROM (0xEDA0,	2, 16, 16,	"LDI",					LDI, InstBlock_General);
	// (DE) <- (HL) & Next memory position until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INST_FROM (0xEDB0,	2, 16, 16,	"LDIR",					LDIR, InstBlock_General);
	// (DE) <- (HL) & Previous memory position
	_INST_FROM (0xEDA8,	2, 16, 16,	"LDD",					LDD, InstBlock_General);
	// (DE) <- (HL) & Previous memory position until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INST_FROM (0xEDB8,	2, 16, 16,	"LDDR",					LDDR, InstBlock_General);
	// A compared with (HL) & Next memory position
	_INST_FROM (0xEDA1,	2, 16, 16,	"CPI",					CPI, InstBlock_General);
	// A compared with (HL) & Next memory position until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INST_FROM (0xEDB1,	2, 16, 16,	"CPIR",					CPIR, InstBlock_General);
	// A compared with (HL) & Previous memory position
	_INST_FROM (0xEDA9,	2, 16, 16,	"CPD",					CPD, InstBlock_General);
	// A compared with (HL) & Previous memory position until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INST_FROM (0xEDB9,	2, 16, 16,	"CPDR",					CPDR, InstBlock_General);
}

#endif
  
// End of the file
/*@}*/
