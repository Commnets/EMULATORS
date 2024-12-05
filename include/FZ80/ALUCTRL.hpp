/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ALUCTRL.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of generic ALU related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_ALUCTRLINSTRUCTIONS__
#define __FZX80_ALUCTRLINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	// DAA: Decimal Adjust Accumulator (after addition usually)
	_INSTZ80_FROM (0x27,	1, 4, { },	"DAA",					DAA, Instruction);
	// CPL: Complement Accumulator
	_INSTZ80_FROM (0x2F,	1, 4, { },	"CPL",					CPL, Instruction);
	// NEG: Negated Accumulator (two's complement)
	_INSTZ80_FROM (0xED44,	2, 8, { },	"NEG",					NEG, Instruction);
	// CCF: Inverts carry flag
	_INSTZ80_FROM (0x3F,	1, 4, { },	"CCF",					CCF, Instruction);
	// SCF: Set carry flag
	_INSTZ80_FROM (0x37,	1, 4, { },	"SCF",					SCF, Instruction);
	// NOP: Does nothing
	_INSTZ80_FROM (0x00,	1, 4, { },	"NOP",					NOP, Instruction);
	// HALT: Stop processor until interrupt happens!
	_INSTZ80_FROM (0x76,	1, 4, { },	"HALT",					HALT, Instruction);
	// DI: Disable interrupts (INT)
	_INSTZ80_FROM (0xF3,	1, 4, { },	"DI",					DI, Instruction);
	// EI: Enable interrupts (INT)
	_INSTZ80_FROM (0xFB,	1, 4, { },	"EI",					EI, Instruction);
	// IM: Change the type of interrupt (INT)
	_INSTZ80_FROM (0xED46,	2, 8, { },	"IM 0",					IM_0, Instruction);
	_INSTZ80_FROM (0xED56,	2, 8, { },	"IM 1",					IM_1, Instruction);
	_INSTZ80_FROM (0xED5E,	2, 8, { },	"IM 2",					IM_2, Instruction);
}

#endif
  
// End of the file
/*@}*/
