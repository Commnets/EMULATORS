/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INOUT.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
		bool executeAWith (unsigned char np)
							{ registerA ().set ({ static_cast <CZ80*> (cpu ()) -> port (np) }); return (true); }
		/** Over any register and affecting flags. */
		inline bool executeWith (MCHEmul::Register& r, unsigned char np);
		/** Just affecting flags. */
		inline bool executeWith (unsigned char np);

		private:
		inline void affectFlags (const MCHEmul::UByte& v);
	};

	// ---
	inline bool IN_General::executeWith (MCHEmul::Register& r, unsigned char np)
	{
		MCHEmul::UByte v;
		r.set ({ v = static_cast <CZ80*> (cpu ()) -> port (np) });

		affectFlags (v);

		return (true);
	}

	// ---
	inline bool IN_General::executeWith (unsigned char np)
	{
		affectFlags (static_cast <CZ80*> (cpu ()) -> port (np));

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
		bool execute0With (unsigned char np)
							{ static_cast <CZ80*> (cpu ()) -> setPort (np, MCHEmul::UByte::_0); return (true);  }
		/** Just from A to a port. */
		bool executeAWith (unsigned char np)
							{ static_cast <CZ80*> (cpu ()) -> setPort (np, registerA ().values ()[0].value ()); return (true); }
		/** From any register to a port */
		bool executeWith (MCHEmul::Register& r, unsigned char np)
							{ static_cast <CZ80*> (cpu ()) -> setPort (np, r.values ()[0].value ()); return (true); }
	};

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
}

#endif
  
// End of the file
/*@}*/
