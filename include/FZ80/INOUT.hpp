/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INOUT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/12/2023 \n
 *	Description: The list of INOUT instructions used by any FZX80 family. \n
 *				 The no documented instructions have also been included.
 *	Versions: 1.0 Initial
 */

#ifndef __FZX80_INOUTINSTRUCTIONS__
#define __FZX80_INOUTINSTRUCTIONS__

#include <FZ80/Instruction.hpp>

namespace FZ80
{
	/** IN_General: To load any port with a value. 
		All, except the one managing the accumulator (that is quicker) affects the flags. */
	class IN_General : public Instruction
	{
		public:
		IN_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		/** Just over A with a port value. Flags are not affected. */
		inline bool executeAWith (unsigned char np);
		/** Over any register and affecting flags. */
		inline bool executeWith (MCHEmul::Register& r, unsigned char np);
		/** Just affecting flags. */
		inline bool executeWith (unsigned char np);

		private:
		inline void affectFlags (const MCHEmul::UByte& v);
	};

	// ---
	inline bool IN_General::executeAWith (unsigned char np)
	{ 
		// The value of the component BC is pushed into the address bus...
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true /** Already in big endian. */);
		// ...because it migth be usefull when reading the port!
		registerA ().set ({ static_cast <CZ80*> (cpu ()) -> portValue (np) }); 
		
		return (true); 
	}

	// ---
	inline bool IN_General::executeWith (MCHEmul::Register& r, unsigned char np)
	{
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true);

		MCHEmul::UByte v;
		r.set ({ v = static_cast <CZ80*> (cpu ()) -> portValue (np) });

		affectFlags (v);

		return (true);
	}

	// ---
	inline bool IN_General::executeWith (unsigned char np)
	{
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true);

		affectFlags (static_cast <CZ80*> (cpu ()) -> portValue (np));

		return (true);
	}

	// ---
	inline void IN_General::affectFlags (const MCHEmul::UByte& v)
	{
		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// Carry flag is not affected...
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Always...
		st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, v.bit (3));
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false); // Always...
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, v.bit (5));
		st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
		st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.bit (7));
	}

	// With the register A from a value, quicker!
	_INST_FROM (0xDB,		2, 11, 11,	"IN A,([#1])",			IN_A, IN_General);
	// With a register from a port
	_INST_FROM (0xED78,		3, 12, 12,	"IN A,([#1])",			IN_AFromPort, IN_General);
	_INST_FROM (0xED40,		3, 12, 12,	"IN B,([#1])",			IN_BFromPort, IN_General);
	_INST_FROM (0xED48,		3, 12, 12,	"IN C,([#1])",			IN_CFromPort, IN_General);
	_INST_FROM (0xED50,		3, 12, 12,	"IN D,([#1])",			IN_DFromPort, IN_General);
	_INST_FROM (0xED58,		3, 12, 12,	"IN E,([#1])",			IN_EFromPort, IN_General);
	_INST_FROM (0xED60,		3, 12, 12,	"IN H,([#1])",			IN_HFromPort, IN_General);
	_INST_FROM (0xED68,		3, 12, 12,	"IN H,([#1])",			IN_LFromPort, IN_General);
	// Just affecting the flags, but not loading anything in anyplace...
	_INST_FROM (0xED70,		3, 12, 12,	"IN ([#1])",			IN_FFromPort, IN_General);		// Undocumented

	/** IN as a block. */
	class INBlock_General : public Instruction
	{
		public:
		INBlock_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t),
			  _inExecution (false),
			  _b0 (false)
							{ }

		protected:
		/** The parameter a indicates the quantity to move up or down. \n
			It has to be -1 or 1. */
		bool executeWith (int a);

		// Implementation
		/** True when it is in execution. */
		bool _inExecution;
		/** True when _bc0. */
		bool _b0;
	};

	// (HL) <- IN(BC) & Next memory position (HL = HL + 1) & --Counter (B = B - 1)
	_INST_FROM (0xEDA2,		2, 16, 16,	"INI",				INI, INBlock_General);
	// (HL) <- IN(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INST_FROM (0xEDB2,		2, 16, 16,	"INIR",				INIR, INBlock_General);	
	// (HL) <- IN(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INST_FROM (0xEDAA,		2, 16, 16,	"IND",				IND, INBlock_General);
	// (HL) <- IN(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INST_FROM (0xEDBA,		2, 16, 16,	"INDR",				INDR, INBlock_General);	

	/** OUT_General: To save any value to a port. \n
		None affects the flags. */
	class OUT_General : public Instruction
	{
		public:
		OUT_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t)
							{ }

		protected:
		inline bool execute0With (unsigned char np);
		/** Just from A to a port. */
		inline bool executeAWith (unsigned char np);
		/** From any register to a port */
		inline bool executeWith (MCHEmul::Register& r, unsigned char np);
	};

	// ---
	inline bool OUT_General::execute0With (unsigned char np)
	{ 
		// The value of the component BC is pushed into the address bus...
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true /** Already in big endian. */);
		// ...because it migth be usefull when writting into the port!
		static_cast <CZ80*> (cpu ()) -> setPortValue (np, MCHEmul::UByte::_0); 
		
		return (true);  
	}

	// ---
	inline bool OUT_General::executeAWith (unsigned char np)
	{ 
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true);

		static_cast <CZ80*> (cpu ()) -> setPortValue (np, registerA ().values ()[0].value ()); 
		
		return (true);
	}

	// ---
	inline bool OUT_General::executeWith (MCHEmul::Register& r, unsigned char np)
	{ 
		_lastINOUTAddress = MCHEmul::Address ({ registerA ().values ()[0], np }, true);

		static_cast <CZ80*> (cpu ()) -> setPortValue (np, r.values ()[0].value ()); 
		
		return (true); 
	}

	// To A. Quicker...
	_INST_FROM (0xD3,		2, 11, 11,	"OUT ([#1]),A",			OUT_A, OUT_General);
	// From a register to a Port...
	_INST_FROM (0xED79,		3, 12, 12,	"OUT ([#1]),A",			OUT_AToPort, OUT_General);
	_INST_FROM (0xED41,		3, 12, 12,	"OUT ([#1]),B",			OUT_BToPort, OUT_General);
	_INST_FROM (0xED49,		3, 12, 12,	"OUT ([#1]),C",			OUT_CToPort, OUT_General);
	_INST_FROM (0xED51,		3, 12, 12,	"OUT ([#1]),D",			OUT_DToPort, OUT_General);
	_INST_FROM (0xED59,		3, 12, 12,	"OUT ([#1]),E",			OUT_EToPort, OUT_General);
	_INST_FROM (0xED61,		3, 12, 12,	"OUT ([#1]),H",			OUT_HToPort, OUT_General);
	_INST_FROM (0xED69,		3, 12, 12,	"OUT ([#1]),L",			OUT_LToPort, OUT_General);
	// Just 0 to a port
	_INST_FROM (0xED71,		2, 12, 12,	"OUT ([#1]),0",			OUT_0ToPort, OUT_General);		// Undocumented

	/** OUT as a block. */
	class OUTBlock_General : public Instruction
	{
		public:
		OUTBlock_General (unsigned int c, unsigned int mp, unsigned int cc, unsigned int rcc, 
				const std::string& t)
			: Instruction (c, mp, cc, rcc, t),
			  _inExecution (false),
			  _b0 (false)
							{ }

		protected:
		/** The parameter a indicates the quantity to move up or down. \n
			It has to be -1 or 1. */
		bool executeWith (int a);

		// Implementation
		/** True when it is in execution. */
		bool _inExecution;
		/** True when _bc0. */
		bool _b0;
	};

	// (HL) -> OUT(BC) & Next memory position (HL = HL + 1) & --Counter (B = B - 1)
	_INST_FROM (0xEDA3,		2, 16, 16,	"OUTI",				OUTI, OUTBlock_General);
	// (HL) -> OUT(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INST_FROM (0xEDB3,		2, 16, 16,	"OTIR",				OTIR, OUTBlock_General);	
	// (HL) -> OUT(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INST_FROM (0xEDAB,		2, 16, 16,	"OUTD",				OUTD, OUTBlock_General);
	// (HL) -> OUT(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INST_FROM (0xEDBB,		2, 16, 16,	"OTDR",				OTDR, OUTBlock_General);	
}

#endif
  
// End of the file
/*@}*/
