/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: CALL.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		inline void executeBranch (bool a); // Using the parameter 1 and 2 always!
	};

	// ---
	inline void CALL_General::executeBranch (bool a)
	{
		if (a)
			_additionalCycles = 7; // Always 7 cycles more when the condition is found!

		MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
		stack () -> push (pc.asAddress ().values ());
		pc.setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 
		// Little endian, but the interchage of the bytes has already been done in the code, 
		// That's why the value true, instead false! (it is quicker)...
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

	/** Like call but to specific locations onpage 0. */
	class RST_General : public Instruction
	{
		public:
		RST_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// Using the parameter t as an index to the address...
		inline void executeBranch (char t);
	};

	// ---
	inline void RST_General::executeBranch (char t)
	{
		static const std::vector <MCHEmul::Address> _DIRS =
			{
				MCHEmul::Address ({ 0x00, 0x00 }, false), // All little endian!
				MCHEmul::Address ({ 0x08, 0x00 }, false),
				MCHEmul::Address ({ 0x10, 0x00 }, false),
				MCHEmul::Address ({ 0x18, 0x00 }, false),
				MCHEmul::Address ({ 0x20, 0x00 }, false),
				MCHEmul::Address ({ 0x28, 0x00 }, false),
				MCHEmul::Address ({ 0x30, 0x00 }, false),
				MCHEmul::Address ({ 0x38, 0x00 }, false)
			};

		MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
		stack () -> push (pc.asAddress ().values ());
		pc.setAddress (_DIRS [(size_t) t]);
	}

	// Absolute...
	_INST_FROM (0xC7,		1, 11, 11,	"RST 00h",				RST_00, RST_General);
	_INST_FROM (0xCF,		1, 11, 11,	"RST 08h",				RST_08, RST_General);
	_INST_FROM (0xD7,		1, 11, 11,	"RST 10h",				RST_10, RST_General);
	_INST_FROM (0xDF,		1, 11, 11,	"RST 18h",				RST_18, RST_General);
	_INST_FROM (0xE7,		1, 11, 11,	"RST 20h",				RST_20, RST_General);
	_INST_FROM (0xEF,		1, 11, 11,	"RST 28h",				RST_28, RST_General);
	_INST_FROM (0xF7,		1, 11, 11,	"RST 30h",				RST_30, RST_General);
	_INST_FROM (0xFF,		1, 11, 11,	"RST 38h",				RST_38, RST_General);

	// RET
	class RET_General : public Instruction
	{
		public:
		RET_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		// a = true when additional cycles mut taken into account. true by default!
		inline void executeReturn (bool a = true); // Using the parameter 1 always!
	};

	// ---
	inline void RET_General::executeReturn (bool a)
	{
		if (a) 
			_additionalCycles = 5; 

		cpu () -> programCounter ().setAddress (MCHEmul::Address (stack () -> pull (2), false)); // Little endian!
	}

	// Absolute
	_INST_FROM (0xC9,		1, 10, 10,	"RET",					RET, RET_General);
	// RET Depending on the value of the flags...
	_INST_FROM (0xC0,		1, 5, 5,	"RET NZ",				RET_NZ, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xC8,		1, 5, 5,	"RET Z",				RET_Z, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xD0,		1, 5, 5,	"RET NC",				RET_NC, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xD8,		1, 5, 5,	"RET C",				RET_C, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xE0,		1, 5, 5,	"RET PO",				RET_PO, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xE8,		1, 5, 5,	"RET PE",				RET_PE, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xF0,		1, 5, 5,	"RET P",				RET_P, RET_General);			// 5 cycles more when the conditiion is true
	_INST_FROM (0xF8,		1, 5, 5,	"RET M",				RET_M, RET_General);			// 5 cycles more when the conditiion is true
	// From interrupt!
	// Maskarable...
	_INST_FROM (0xED4D,		2, 14, 14,	"RETI",					RET_I, RET_General);
	// Non maskarable!
	_INST_FROM (0xED45,		2, 14, 14,	"RETN",					RET_N, RET_General);
}

#endif
  
// End of the file
/*@}*/
