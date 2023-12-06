/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: CALL.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/10/2023 \n
 *	Description: The list of generic CALL related instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_CALLINSTRUCTIONS__
#define __FZX80_CALLINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** To call a subrotine. */
	class CALL_General : public Instruction
	{
		public:
		CALL_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		inline void executeBranch (); // Using the parameter 1 and 2 always!
	};

	// ---
	inline void CALL_General::executeBranch ()
	{
		_additionalCycles = 7; // Always 7 cycles more when the condition is found!

		MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
		stack () -> push (pc.asAddress ().previous (2).values ());
		pc.setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, false));
	}

	// Absolute...
	_INST_FROM (0xCD,		3, 17, 17,	"CALL [%2]",			CALL, CALL_General);
	// CALL Depending on the value of the flags...
	_INST_FROM (0xC4,		3, 10, 10,	"CALL NZ,[%2]",			CALL_NZ, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xCC,		3, 10, 10,	"CALL Z,[%2]",			CALL_Z, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xD4,		3, 10, 10,	"CALL NC,[%2]",			CALL_NC, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xDC,		3, 10, 10,	"CALL C,[%2]",			CALL_C, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xE4,		3, 10, 10,	"CALL PO,[%2]",			CALL_PO, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xEC,		3, 10, 10,	"CALL PE,[%2]",			CALL_PE, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xF4,		3, 10, 10,	"CALL P,[%2]",			CALL_P, CALL_General);				// 7 cycles more when the condition is true
	_INST_FROM (0xFC,		3, 10, 10,	"CALL M,[%2]",			CALL_M, CALL_General);				// 7 cycles more when the condition is true

	// RET
	class RET_General : public Instruction
	{
		public:
		RET_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		inline void executeReturn (); // Using the parameter 1 always!
	};

	// ---
	inline void RET_General::executeReturn()
	{
		_additionalCycles = 6; // Always 6 cycles more when the condition is true!

		// TODO
	}

	// Absolute
	_INST_FROM (0xC9,		3, 10, 10,	"RET",					RET, RET_General);
	// RET Depending on the value of the flags...
	_INST_FROM (0xC0,		1, 7, 7,	"RET NZ",				RET_NZ, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xC8,		1, 7, 7,	"RET Z",				RET_Z, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xD0,		1, 7, 7,	"RET NC",				RET_NC, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xD8,		1, 7, 7,	"RET C",				RET_C, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xE0,		1, 7, 7,	"RET PO",				RET_PO, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xE8,		1, 7, 7,	"RET PE",				RET_PE, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xF0,		1, 7, 7,	"RET P",				RET_P, RET_General);			// 6 cycles more when the conditiion is true
	_INST_FROM (0xF8,		1, 7, 7,	"RET M",				RET_M, RET_General);			// 6 cycles more when the conditiion is true
}

#endif
  
// End of the file
/*@}*/
