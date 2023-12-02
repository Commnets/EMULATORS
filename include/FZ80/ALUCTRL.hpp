/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: ALUCTRL_Generic.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
	// ALU Generic Instructions
	// Decimal Adjust Accumulator
	_INST_FROM (0x27,	1, 4, 4,	"DAA",					DAA, Instruction);
	// Complement Accumulator
	_INST_FROM (0x2F,	1, 4, 4,	"CPL",					CPL, Instruction);
	// Negated Accumulator (two's complement)
	_INST_FROM (0xED44,	2, 8, 8,	"NEG",					NEG, Instruction);
	// Complement carry flag
	_INST_FROM (0x3F,	1, 4, 4,	"CCF",					CCF, Instruction);
	// Set carry flag
	_INST_FROM (0x37,	1, 4, 4,	"SCF",					SCF, Instruction);
}

#endif
  
// End of the file
/*@}*/
