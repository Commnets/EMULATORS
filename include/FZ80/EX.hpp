/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: EX.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		EX_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		/** To exchange the context of two set of registers. \n
			These instructions don't affect either to the registers. */
		inline bool executeWith (MCHEmul::RefRegisters& r, MCHEmul::RefRegisters& rp);
		/** All interactions with the stack pointer and 16 bits registers are quite similar. */
		inline bool executeWithSP (MCHEmul::RefRegisters& r);
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

	// ---
	inline bool EX_General::executeWithSP (MCHEmul::RefRegisters& r)
	{
		MCHEmul::Address spA = memory () -> stack () -> currentAddress ();
		// There is also a manage of the address bus and data bus but the final real value is later...
		MCHEmul::UBytes mD = memory () -> values (spA, 2); 
		MCHEmul::UBytes rD ({ r [0] -> values ()[0], r [1] -> values ()[0] }, false); // To wrtite it later in the right order...
		r [0] -> set ({ mD [1] }); // Notice that they are set in the reverse order!
		r [1] -> set ({ mD [0] });
		memory () -> set (_lastExecutionData._INOUTAddress = spA, _lastExecutionData._INOUTData = rD);

		// It could be done using the method stack -> push seberal times...
		// ...but seems this way is quicker as it avoids stupid calculus!

		return (true);
	}

	// AF and AF'
	_INSTZ80_FROM (0x08,	1, 4, { },	"EX AF,AF'",			EX_AF, EX_General);
	// DE and HL
	_INSTZ80_FROM (0xEB,	1, 4, { },	"EX DE,HL",				EX_DEWithHL, EX_General);
	// EXX (all)
	_INSTZ80_FROM (0xD9,	1, 4, { },	"EXX",					EX_X, EX_General);
	// With SP
	_INSTZ80_FROM (0xE3,	1, 19, { },	"EX (SP),HL",			EX_SPWithHL, EX_General);
	_INSTZ80_FROM (0xDDE3,	2, 23, { },	"EX (SP),IX",			EX_SPWithIX, EX_General);
	_INSTZ80_FROM (0xFDE3,	2, 23, { },	"EX (SP),IY",			EX_SPWithIY, EX_General);

	/** To move a block of data. \n
		These instructions affects the flags in a similar way than DEC/INC instructions for LDD/LDDR/LDI/LDIR or 
		or CP for CPI/CPIR/CPD/CPDR. */
	class InstBlock_General : public Instruction
	{
		public:
		InstBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t),
			  _inExecution (false),
			  _bc0 (false)
							{ }

		protected:
		/** The parameter a indicates the quantity to move up or down. \n
			It has to be -1 or 1. */
		inline bool executeMoveWith (int a);
		inline bool executeCompareWith (int a);

		// Implementation
		/** True when it is in execution. */
		bool _inExecution;
		/** True when _bc0. */
		bool _bc0;
	};

	// ---
	inline bool InstBlock_General::executeMoveWith (int a)
	{
		assert (a == 1 || a == -1);

		// The registers involved...
		MCHEmul::Register& rB		= registerB ();
		MCHEmul::Register& rC		= registerC ();
		MCHEmul::Register& rD		= registerD ();
		MCHEmul::Register& rE		= registerE ();
		MCHEmul::Register& rH		= registerH ();
		MCHEmul::Register& rL		= registerL ();

		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The data of those registers, and the way they have to be managed...
		// Numbers and Registers from registers are big - endian...
		MCHEmul::UInt bcI	 (MCHEmul::UBytes ({ rB.values ()[0], rC.values ()[0] })); // Counter with the data to move... 
		MCHEmul::Address deA (MCHEmul::UBytes ({ rD.values ()[0], rE.values ()[0] })); // Destination...
		MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

		// Move the content from (hlA) into (deA)
		MCHEmul::UBytes vc = { };
		memory () -> set 
			(_lastExecutionData._INOUTAddress = deA, 
			 _lastExecutionData._INOUTData = vc = memory () -> values (hlA, 1)); // 1 byte...
		// This value is calculated to determine the value of the status flag 3 and 5 as described in: 
		// http://www.z80.info/zip/z80-documented.pdf (section 4.2)
		MCHEmul::UByte n ((unsigned char) (vc [0].value () + registerA ().values ()[0].value ())); // Possible carry is not taken into account
		// The positions target and destination are moved according to "a" (that can be either 1 or -1)
		deA = (a > 0) ? (deA + 1) : (deA - 1); 
		hlA = (a > 0) ? (hlA + 1) : (hlA - 1); 
		// The counter is decremented into 1, and _bc0 becomes true if data data is 0 doing so...
		_bc0 = ((bcI -= 1).asUnsignedInt () == (unsigned int) 0); 
	
		// Move the new content back into the registers...
		rB.set ({ bcI.bytes ()[0] }); rC.set ( { bcI.bytes ()[1] });
		rD.set ({ deA.bytes ()[0] }); rE.set ( { deA.bytes ()[1] });
		rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

		// How the flags are affected...
		// Carry flag is not affected...
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
		st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, !_bc0); // Only false at the end of the instruction if repeat!
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, n.bit (3)); // Undocumented..
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, n.bit (1)); // Undocumented...
		// Negative flag is not affected...
		// Sign flag is not affected...

		return (true);
	}

	// ---
	inline bool FZ80::InstBlock_General::executeCompareWith (int a)
	{
		// The registers involved...
		MCHEmul::Register& rB		= registerB ();
		MCHEmul::Register& rC		= registerC ();
		MCHEmul::Register& rH		= registerH ();
		MCHEmul::Register& rL		= registerL ();

		MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

		// The data of those registers, and the way they have to be managed...
		// Numbers and Registers from registers are big - endian...
		MCHEmul::UInt bcI	 (MCHEmul::UBytes ({ rB.values ()[0], rC.values ()[0] })); // Number of data to move... 
		MCHEmul::Address hlA (MCHEmul::UBytes ({ rH.values ()[0], rL.values ()[0] })); // Target...

		// To compare the two registers a mathematical operation is done...
		MCHEmul::UByte rAV = registerA ().values ()[0];
		_lastExecutionData._INOUTData = 
			MCHEmul::UBytes ({ memory () -> value (_lastExecutionData._INOUTAddress = hlA) }); // Just only 1 byte...
		MCHEmul::UInt mV (_lastExecutionData._INOUTData [0]);
		MCHEmul::UInt rst  = MCHEmul::UInt (rAV) -  mV;
		// This is needed to calculate later the status of the half carry...
		int mVI = mV.asInt ();
		int cr = int (rAV.value ()) ^ mVI ^ (int (rAV.value ()) - mVI);
		// Used later for the non documented XF and YF...
		MCHEmul::UInt n = rst - 
			(st.bitStatus (FZ80::CZ80::_HALFCARRYFLAG) ? MCHEmul::UInt::_1 : MCHEmul::UInt::_0); 

		// The next position is moved according to "a"
		hlA = (a > 0) ? (hlA + 1) : (hlA - 1); 
		// The number of elements to move is decremented into 1, 
		// and _bc= becomes true if data data is 0
		_bc0 = ((bcI -= 1).asUnsignedInt () == (unsigned int) 0); 
	
		// Restore the new content into the registers...
		rB.set ({ bcI.bytes ()[0] }); rC.set ( { bcI.bytes ()[1] });
		rH.set ({ hlA.bytes ()[0] }); rL.set ( { hlA.bytes ()[1] });

		// How the flags are affected...
		// Carry flag is not affected...
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true);
		st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, !_bc0);
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, (cr & 0x10) != 0x00);
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rst [0].bit (1)); // Undocumented...
		st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
		st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());
		// The rest of the flags are not affected...

		return (true);
	}

	// (DE) <- (HL) & Next memory position (HL = HL + 1) & --Counter (BC = BC - 1)
	_INSTZ80_FROM (0xEDA0,	2, 16, { },	"LDI",					LDI, InstBlock_General);
	// (DE) <- (HL) & Next memory position & Counter-- until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INSTZ80_FROM (0xEDB0,	2, 16, { },	"LDIR",					LDIR, InstBlock_General);
	// (DE) <- (HL) & Previous memory position (HL = HL - 1) & --Counter
	_INSTZ80_FROM (0xEDA8,	2, 16, { },	"LDD",					LDD, InstBlock_General);
	// (DE) <- (HL) & Previous memory position until BC == 0
	// 21 Cycles when BC != 0 (_additionalCycles = 5). _FINISH = true when BC == 0
	_INSTZ80_FROM (0xEDB8,	2, 16, { },	"LDDR",					LDDR, InstBlock_General);
	// A compared with (HL) & Next memory position (HL = HL + 1) & --Counter (BC = BC - 1)
	_INSTZ80_FROM (0xEDA1,	2, 16, { }, "CPI",					CPI, InstBlock_General);
	// A compared with (HL) & Next memory position & --Counter until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INSTZ80_FROM (0xEDB1,	2, 16, { }, "CPIR",					CPIR, InstBlock_General);
	// A compared with (HL) & Previous memory position& --Counter
	_INSTZ80_FROM (0xEDA9,	2, 16, { }, "CPD",					CPD, InstBlock_General);
	// A compared with (HL) & Previous memory position & --Counter until BC == 0
	// 21 Cycles when BC != 0 and A != (HL) (_additionalCycles = 5). _FINISH = true when BC == 0 or A == (HL)
	_INSTZ80_FROM (0xEDB9,	2, 16, { }, "CPDR",					CPDR, InstBlock_General);
}

#endif
  
// End of the file
/*@}*/
