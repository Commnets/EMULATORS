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
		IN_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		/** Installs the input byte and applies the selected flag behavior. \n
			A null destination represents the flags-only input instruction. */
		virtual void completePortRead
			(const MCHEmul::UByte& v,
			 const Z80Port::Access::ReadContext& rC) override;

		protected:
		/** Just over A with a port value. Flags are not affected. */
		inline bool executeAWith (unsigned char np);
		/** Over any register but from the value of the register C. */
		inline bool executeWithFromC (MCHEmul::Register& r);
		/** Just affecting flags. */
		inline bool execute0FromC ();

		private:
		static const unsigned int _INAIOSTARTCYCLE		= 7;
		static const unsigned int _INAIOACCESSCYCLE		= 10;
		static const unsigned int _INRCIOSTARTCYCLE		= 8;
		static const unsigned int _INRCIOACCESSCYCLE	= 11;

		inline void affectFlags (const MCHEmul::UByte& v);
	};

	// ---
	inline bool IN_General::executeAWith (unsigned char np)
	{ 
		// The value of the component A(MSB)|np(LSB) is pushed into the address bus...
		// ...because it migth be useful when reading the port! (and it is the behaviour defined)
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ registerA ().values ()[0], np }, true /** Already in big endian. */);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		prepareIOAccess (ab, np, _INAIOSTARTCYCLE);
		setIOAccessClockCycle (_INAIOACCESSCYCLE);
		// Capture the old A in the port address; install the input only on release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._instruction = this;
		access._readContext._destination = &registerA ();
		// The default context preserves flags for IN A,(n).
		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// ---
	inline bool IN_General::executeWithFromC (MCHEmul::Register& r)
	{
		unsigned char np = registerC ().values ()[0].value ();

		// The value of the component B(MSB)|np(LSB) is pushed into the address bus...
		// ...because it migth be useful when reading the port! (and it is the behaviour defined)
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ registerB ().values ()[0], np }, true);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		prepareIOAccess (ab, np, _INRCIOSTARTCYCLE);
		setIOAccessClockCycle (_INRCIOACCESSCYCLE);
		// Keep BC before an input into B or C can replace part of the address.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._instruction = this;
		access._readContext._destination = &r;
		access._readContext._affectFlags = true;

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// ---
	inline bool IN_General::execute0FromC ()
	{
		unsigned char np = registerC ().values ()[0].value ();

		// The value of the component B(MSB)|np(LSB) is pushed into the address bus...
		// ...because it migth be useful when reading the port! (and it is the behaviour defined)
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ registerB ().values ()[0], np }, true);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		// The value is not kept anywhere...(lost)
		prepareIOAccess (ab, np, _INRCIOSTARTCYCLE);
		setIOAccessClockCycle (_INRCIOACCESSCYCLE);
		// The flags-only input still performs a real port read when released.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._instruction = this;
		access._readContext._affectFlags = true;

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
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
	_INSTZ80_FROM (0xDB,		2, 11, { },	"IN A,([#1])",			IN_A, IN_General);
	// With a register from a port
	_INSTZ80_FROM (0xED78,		2, 12, { },	"IN A,(C)",				IN_AFromC, IN_General);
	_INSTZ80_FROM (0xED40,		2, 12, { },	"IN B,(C)",				IN_BFromC, IN_General);
	_INSTZ80_FROM (0xED48,		2, 12, { },	"IN C,(C)",				IN_CFromC, IN_General);
	_INSTZ80_FROM (0xED50,		2, 12, { },	"IN D,(C)",				IN_DFromC, IN_General);
	_INSTZ80_FROM (0xED58,		2, 12, { },	"IN E,(C)",				IN_EFromC, IN_General);
	_INSTZ80_FROM (0xED60,		2, 12, { },	"IN H,(C)",				IN_HFromC, IN_General);
	_INSTZ80_FROM (0xED68,		2, 12, { },	"IN L,(C)",				IN_LFromC, IN_General);
	// Just affecting the flags, but not loading anything in anyplace...
	_INSTZ80_FROM (0xED70,		2, 12, { }, "IN F,(C)",				IN_FFromC, IN_General);		// Undocumented 
																							// (other representation is IN (C))

	/** IN as a block. */
	class INBlock_General : public Instruction
	{
		public:
		INBlock_General (unsigned int c, unsigned int mp, unsigned int cc, 
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t),
			  _inExecution (false),
			  _b0 (false)
							{ }

		/** Stores the input byte at the original memory destination and updates flags. \n
			Register progression, repetition and duration are handled during preparation. */
		virtual void completePortRead
			(const MCHEmul::UByte& v,
			 const Z80Port::Access::ReadContext& rC) override;

		protected:
		static const unsigned int _IOSTARTCYCLE		= 9;
		static const unsigned int _IOACCESSCYCLE	= 12;

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
	_INSTZ80_FROM (0xEDA2,		2, 16, { }, "INI",				INI, INBlock_General);
	// (HL) <- IN(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_FROM (0xEDB2,		2, 16, { }, "INIR",				INIR, INBlock_General);
	// (HL) <- IN(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INSTZ80_FROM (0xEDAA,		2, 16, { }, "IND",				IND, INBlock_General);
	// (HL) <- IN(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_FROM (0xEDBA,		2, 16, { }, "INDR",				INDR, INBlock_General);

	/** OUT_General: To save any value to a port. \n
		None affects the flags. */
	class OUT_General : public Instruction
	{
		public:
		OUT_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t)
							{ }

		protected:
		/** Just from A to a port. */
		inline bool executeAWith (unsigned char np);
		/** From any register to the port indicated in register C. */
		inline bool executeWithToC (MCHEmul::Register& r);
		/** Put 0 in the port pointed by C. */
		inline bool execute0WithToC ();

		private:
		static const unsigned int _OUTAIOSTARTCYCLE		= 7;
		static const unsigned int _OUTRCIOSTARTCYCLE	= 8;
		// Record the final nominal I/O T-state; release remains at a transaction boundary.
		static const unsigned int _OUTAIOACCESSCYCLE	= 10;
		static const unsigned int _OUTRCIOACCESSCYCLE	= 11;
	};

	// ---
	inline bool OUT_General::executeAWith (unsigned char np)
	{ 
		// In the case of using the OUT (n), A...
		// ...the address bus is kept in the address bus A(MSB)|n(LSB)
		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ registerA ().values ()[0], np }, true);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		prepareIOAccess (ab, np, _OUTAIOSTARTCYCLE);
		setIOAccessClockCycle (_OUTAIOACCESSCYCLE);

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._value = registerA ().values ()[0];

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// ---
	inline bool OUT_General::executeWithToC (MCHEmul::Register& r)
	{
		unsigned char np = registerC ().values ()[0].value ();

		// In the case of using the OUT r, (C)...
		// ...the address bus is kept with Bn
		_lastExecutionData._INOUTAddress =
			MCHEmul::Address ({ registerB ().values ()[0], np }, true);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		prepareIOAccess (ab, np, _OUTRCIOSTARTCYCLE);
		setIOAccessClockCycle (_OUTRCIOACCESSCYCLE);

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._value = r.values ()[0];

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// ---
	inline bool OUT_General::execute0WithToC ()
	{ 
		unsigned char np = registerC ().values ()[0].value ();

		_lastExecutionData._INOUTAddress = 
			MCHEmul::Address ({ registerB ().values ()[0], np }, true /** Already in big endian. */);

		unsigned short ab =
			(unsigned short) _lastExecutionData._INOUTAddress.value ();

		// Undocumented OUT (C),0 writes a zero byte to the captured BC address.
		prepareIOAccess (ab, np, _OUTRCIOSTARTCYCLE);
		setIOAccessClockCycle (_OUTRCIOACCESSCYCLE);

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._clockCycle = IOAccessClockCycle ();
		access._address = ab;
		access._value = MCHEmul::UByte::_0;

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// To A. Quicker...
	_INSTZ80_FROM (0xD3,		2, 11, { }, "OUT ([#1]),A",			OUT_A, OUT_General);
	// From a register to a Port...
	_INSTZ80_FROM (0xED79,		2, 12, { },	"OUT (C),A",			OUT_AToC, OUT_General);
	_INSTZ80_FROM (0xED41,		2, 12, { },	"OUT (C),B",			OUT_BToC, OUT_General);
	_INSTZ80_FROM (0xED49,		2, 12, { },	"OUT (C),C",			OUT_CToC, OUT_General);
	_INSTZ80_FROM (0xED51,		2, 12, { },	"OUT (C),D",			OUT_DToC, OUT_General);
	_INSTZ80_FROM (0xED59,		2, 12, { },	"OUT (C),E",			OUT_EToC, OUT_General);
	_INSTZ80_FROM (0xED61,		2, 12, { },	"OUT (C),H",			OUT_HToC, OUT_General);
	_INSTZ80_FROM (0xED69,		2, 12, { },	"OUT (C),L",			OUT_LToC, OUT_General);
	// Just 0 to a port
	_INSTZ80_FROM (0xED71,		2, 12, { }, "OUT (C),0",			OUT_0ToC, OUT_General);		// Undocumented

	/** OUT as a block. */
	class OUTBlock_General : public Instruction
	{
		public:
		OUTBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
				const MCHEmul::InstructionDefined::CycleStructure& cS, 
				const std::string& t)
			: Instruction (c, mp, cc, cS, t),
			  _inExecution (false),
			  _b0 (false)
							{ }

		protected:
		static const unsigned int _IOSTARTCYCLE = 12;
		// The repeated form retains this access phase before its five extra T-states.
		static const unsigned int _IOACCESSCYCLE = 15;

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
	_INSTZ80_FROM (0xEDA3,		2, 16, { }, "OUTI",				OUTI, OUTBlock_General);
	// (HL) -> OUT(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_FROM (0xEDB3,		2, 16, { }, "OTIR",				OTIR, OUTBlock_General);
	// (HL) -> OUT(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INSTZ80_FROM (0xEDAB,		2, 16, { }, "OUTD",				OUTD, OUTBlock_General);
	// (HL) -> OUT(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_FROM (0xEDBB,		2, 16, { }, "OTDR",				OTDR, OUTBlock_General);
}

#endif
  
// End of the file
/*@}*/
