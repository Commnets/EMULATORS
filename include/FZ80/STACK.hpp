/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: STACK.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of STACK related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_STACKINSTRUCTIONS__
#define __FZX80_STACKINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** PUSH_General: To move the content of a register into the stack. \n
		Any version of this instruction doesn't affect to any flag. */
	class PUSH_General : public Instruction
	{
		public:
		PUSH_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }
		PUSH_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS)
							{ }
		PUSH_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs)
							{ }

		protected:
		/** To move the contant of a set of registers into the stack. \n
			These intructions don't affect either to any flag. */
		inline bool executeWith (MCHEmul::RefRegisters& r);
	};

	// ---
	inline bool PUSH_General::executeWith (MCHEmul::RefRegisters& r)
	{
		// The values are kept in the same order that they appear...
		stack () -> push (_lastExecutionData._INOUTData = 
			MCHEmul::UBytes ({ r [0] -> values ()[0], r [1]  -> values ()[0] })); // 2 bytes always...
		_lastExecutionData._INOUTAddress = stack () -> currentAddress ();

		return (!stack () -> overflow ());
	}

	// PUSH
	// From AF
	_INSTZ80_MC_FROM (0xF5,	1, 11,		"PUSH AF",				PUSH_AF, PUSH_General,
		_Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	// From BC
	_INSTZ80_MC_FROM (0xC5,	1, 11,		"PUSH BC",				PUSH_BC, PUSH_General,
		_Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	// From DE
	_INSTZ80_MC_FROM (0xD5,	1, 11,		"PUSH DE",				PUSH_DE, PUSH_General,
		_Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	// From HL
	_INSTZ80_MC_FROM (0xE5,	1, 11,		"PUSH HL",				PUSH_HL, PUSH_General,
		_Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	// From IX
	_INSTZ80_MC_FROM (0xDDE5,	2, 15,	"PUSH IX",				PUSH_IX, PUSH_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	// From IY
	_INSTZ80_MC_FROM (0xFDE5,	2, 15,	"PUSH IY",				PUSH_IY, PUSH_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_MW3 (), _Z80_MW3 ());

	/** POP_General: To move the content of the stack to a register \n
		Any version of this instruction doesn't affect to any flag. */
	class POP_General : public Instruction
	{
		public:
		POP_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }
		POP_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS)
							{ }
		POP_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs)
							{ }

		protected:
		/** To move the content of the stack into a set of registers. \n
			These intructions don't affect either to any flag. */
		inline bool executeWith (MCHEmul::RefRegisters& r);
	};

	// ---
	inline bool POP_General::executeWith (MCHEmul::RefRegisters& r)
	{
		// The bystes are extracted in the other way around...
		_lastExecutionData._INOUTData = stack () -> pull (2); // 2 bytes always...
		r [1] -> set ({ _lastExecutionData._INOUTData [0].value () });
		r [0] -> set ({ _lastExecutionData._INOUTData [1].value () });
		_lastExecutionData._INOUTAddress = stack () -> currentAddress ();

		return (!stack () -> overflow ());
	}

	// POP
	// To AF
	_INSTZ80_MC_FROM (0xF1,	1, 10,		"POP AF",				POP_AF, POP_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

	// To BC
	_INSTZ80_MC_FROM (0xC1,	1, 10,		"POP BC",				POP_BC, POP_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

	// To DE
	_INSTZ80_MC_FROM (0xD1,	1, 10,		"POP DE",				POP_DE, POP_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

	// To HL
	_INSTZ80_MC_FROM (0xE1,	1, 10,		"POP HL",				POP_HL, POP_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

	// To IX
	_INSTZ80_MC_FROM (0xDDE1,	2, 14,	"POP IX",				POP_IX, POP_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

	// To IY
	_INSTZ80_MC_FROM (0xFDE1,	2, 14,	"POP IY",				POP_IY, POP_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_MR3 (), _Z80_MR3 ());

}

#endif
  
// End of the file
/*@}*/
