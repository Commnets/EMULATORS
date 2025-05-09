/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: JUMP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of generic JUMP related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_JUMPINSTRUCTIONS__
#define __FZX80_JUMPINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** Flags not affected. */
	// Absolute...
	// JP
	_INSTZ80_FROM (0xC3,		3, 10, { }, "JP [%2]",				JP, Instruction);
	// JP Depending on the value of the flags...
	_INSTZ80_FROM (0xC2,		3, 10, { },	"JP NZ,[%2]",			JP_NZ, Instruction);
	_INSTZ80_FROM (0xCA,		3, 10, { },	"JP Z,[%2]",			JP_Z, Instruction);
	_INSTZ80_FROM (0xD2,		3, 10, { },	"JP NC,[%2]",			JP_NC, Instruction);
	_INSTZ80_FROM (0xDA,		3, 10, { },	"JP C,[%2]",			JP_C, Instruction);
	_INSTZ80_FROM (0xE2,		3, 10, { },	"JP PO,[%2]",			JP_PO, Instruction);
	_INSTZ80_FROM (0xEA,		3, 10, { },	"JP PE,[%2]",			JP_PE, Instruction);
	_INSTZ80_FROM (0xF2,		3, 10, { },	"JP P,[%2]",			JP_P, Instruction);
	_INSTZ80_FROM (0xFA,		3, 10, { },	"JP M,[%2]",			JP_M, Instruction);
	// JP to the address sit at (HL), (IX) ot (IY)
	_INSTZ80_FROM (0xE9,		1, 4, { },	"JP (HL)",				JP_IndirectHL, Instruction);
	_INSTZ80_FROM (0xDDE9,		2, 8, { },	"JP (IX)",				JP_IndirectIX, Instruction);
	_INSTZ80_FROM (0xFDE9,		2, 8, { },	"JP (IY)",				JP_IndirectIY, Instruction);

	/** JR. \n
		Flags are not affected. */
	class JR_General : public Instruction
	{
		public:
		JR_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		inline void executeBranch (bool a); // Using the parameter 1 always!
	};

	// ---
	inline void JR_General::executeBranch (bool a)
	{
		if (a)
			_additionalCycles = 5;

		// The value can be negative meaning back jump!
		// At this point the pc has already been incremened in two, as it is an IntructionDefined!
		int jR = MCHEmul::UInt ({ parameters ()[1] /** to use UByte constructor. */ }).asInt ();
		if (jR == 0)
			return; // No need to continue...

		MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
		if (jR > 0) pc.increment ((size_t) jR);
		else pc.decrement ((size_t) -jR); // Parameter to "decrement" always positive...

		/** Used later in BIT (IX...) instructions. */
		static_cast <FZ80::CZ80*> (_lastExecutionData._cpu) -> 
			setRWInternalRegister (pc.asAddress ()[0]);
	}

	// Relative
	_INSTZ80_FROM (0x18,		2, 12, { }, "JR [&1]",				JR, JR_General);
	
	// JR Depending on the value of the flags...
	_INSTZ80_FROM (0x20,		2, 7, { },	"JR NZ,[&1]",			JR_NZ, JR_General);			// 5 cycles more when the condition is true
	_INSTZ80_FROM (0x28,		2, 7, { },	"JR Z,[&1]",			JR_Z, JR_General);			// 5 cycles more when the condition is true
	_INSTZ80_FROM (0x30,		2, 7, { },	"JR NC,[&1]",			JR_NC, JR_General);			// 5 cycles more when the condition is true
	_INSTZ80_FROM (0x38,		2, 7, { },	"JR C,[&1]",			JR_C, JR_General);			// 5 cycles more when the condition is true
	
	// DJNZ
	_INSTZ80_FROM (0x10,		2, 8, { },	"DJNZ [&1]",			DJNZ, JR_General);			// 5 cycles more when the condition is true
}

#endif
  
// End of the file
/*@}*/
