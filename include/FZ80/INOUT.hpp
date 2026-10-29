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
		IN_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS)
							{ }
		IN_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs)
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

		// Capture the old A in the port address; install the input only on release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._address = ab;
		access._instruction = this;
		access._readContext._destination = &registerA ();
		// The default context preserves flags for IN A,(n).
		prepareIOAccess (access);

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

		// Keep BC before an input into B or C can replace part of the address.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._address = ab;
		access._instruction = this;
		access._readContext._destination = &r;
		access._readContext._affectFlags = true;

		prepareIOAccess (access);

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
		// The flags-only input still performs a real port read when released.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_READ;
		access._address = ab;
		access._instruction = this;
		access._readContext._affectFlags = true;

		prepareIOAccess (access);

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
	_INSTZ80_MC_FROM (0xDB,	2, 11,		"IN A,([#1])",			IN_A, IN_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_IOR4 ());

	// With a register from a port
	_INSTZ80_MC_FROM (0xED78,	2, 12,	"IN A,(C)",				IN_AFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED40,	2, 12,	"IN B,(C)",				IN_BFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED48,	2, 12,	"IN C,(C)",				IN_CFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED50,	2, 12,	"IN D,(C)",				IN_DFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED58,	2, 12,	"IN E,(C)",				IN_EFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED60,	2, 12,	"IN H,(C)",				IN_HFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	_INSTZ80_MC_FROM (0xED68,	2, 12,	"IN L,(C)",				IN_LFromC, IN_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());

	// Just affecting the flags, but not loading anything in anyplace...
	_INSTZ80_MC_FROM (0xED70,	2, 12,	"IN F,(C)",				IN_FFromC, IN_General,							// Undocumented
		_Z80_M1 (), _Z80_M1 (), _Z80_IOR4 ());
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
		INBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS),
			  _inExecution (false),
			  _b0 (false)
							{ }
		INBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs),
			  _inExecution (false),
			  _b0 (false)
							{ }

		/** Stores the input byte at the original memory destination and updates flags. \n
			Register progression, repetition and duration are handled during preparation. */
		virtual void completePortRead
			(const MCHEmul::UByte& v,
			 const Z80Port::Access::ReadContext& rC) override;

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

	// IN reads the port before writing memory: 4,5,4,3, not the manual's 4,5,3,4.
	// Repeating forms describe one iteration: alternative 0 finishes, 1 repeats.
	// (HL) <- IN(BC) & Next memory position (HL = HL + 1) & --Counter (B = B - 1)
	_INSTZ80_MC_FROM (0xEDA2,	2, 16,	"INI",					INI, INBlock_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 ());

	// (HL) <- IN(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_MCS_FROM (0xEDB2,	2, 16,	"INIR",					INIR, INBlock_General,
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 () },
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 (), _Z80_I5 () });

	// (HL) <- IN(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INSTZ80_MC_FROM (0xEDAA,	2, 16,	"IND",					IND, INBlock_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 ());

	// (HL) <- IN(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_MCS_FROM (0xEDBA,	2, 16,	"INDR",					INDR, INBlock_General,
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 () },
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_IOR4 (), _Z80_MW3 (), _Z80_I5 () });

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
		OUT_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS)
							{ }
		OUT_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs)
							{ }

		protected:
		/** Just from A to a port. */
		inline bool executeAWith (unsigned char np);
		/** From any register to the port indicated in register C. */
		inline bool executeWithToC (MCHEmul::Register& r);
		/** Put 0 in the port pointed by C. */
		inline bool execute0WithToC ();
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

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._address = ab;
		access._value = registerA ().values ()[0];

		prepareIOAccess (access);

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

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._address = ab;
		access._value = r.values ()[0];

		prepareIOAccess (access);

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

		// Preserve the full port address and output byte until CPU release.
		Z80Port::Access access;
		access._type = Z80Port::Access::Type::_WRITE;
		access._address = ab;
		access._value = MCHEmul::UByte::_0;

		prepareIOAccess (access);

		return (static_cast <CZ80*> (cpu ()) -> schedulePortAccess (access));
	}

	// To A. Quicker...
	_INSTZ80_MC_FROM (0xD3,	2, 11,		"OUT ([#1]),A",			OUT_A, OUT_General,
		_Z80_M1 (), _Z80_MR3 (), _Z80_IOW4 ());

	// From a register to a Port...
	_INSTZ80_MC_FROM (0xED79,	2, 12,	"OUT (C),A",			OUT_AToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED41,	2, 12,	"OUT (C),B",			OUT_BToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED49,	2, 12,	"OUT (C),C",			OUT_CToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED51,	2, 12,	"OUT (C),D",			OUT_DToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED59,	2, 12,	"OUT (C),E",			OUT_EToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED61,	2, 12,	"OUT (C),H",			OUT_HToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	_INSTZ80_MC_FROM (0xED69,	2, 12,	"OUT (C),L",			OUT_LToC, OUT_General,
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

	// Just 0 to a port
	_INSTZ80_MC_FROM (0xED71,	2, 12,	"OUT (C),0",			OUT_0ToC, OUT_General,							// Undocumented
		_Z80_M1 (), _Z80_M1 (), _Z80_IOW4 ());

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
		OUTBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructure& mCS)
			: Instruction (c, mp, cc, t, mCS),
			  _inExecution (false),
			  _b0 (false)
							{ }
		OUTBlock_General (unsigned int c, unsigned int mp, unsigned int cc,
			const std::string& t, const FZ80::MCyclesStructures& mCSs)
			: Instruction (c, mp, cc, t, mCSs),
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

	// OUT reads memory before writing the port; the five-T-state repeat adds no transfer.
	// (HL) -> OUT(BC) & Next memory position (HL = HL + 1) & --Counter (B = B - 1)
	_INSTZ80_MC_FROM (0xEDA3,	2, 16,	"OUTI",					OUTI, OUTBlock_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 ());

	// (HL) -> OUT(BC) & Next memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_MCS_FROM (0xEDB3,	2, 16,	"OTIR",					OTIR, OUTBlock_General,
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 () },
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 (), _Z80_I5 () });

	// (HL) -> OUT(BC) & Previous memory position (HL = HL - 1) & --Counter (B = B - 1)
	_INSTZ80_MC_FROM (0xEDAB,	2, 16,	"OUTD",					OUTD, OUTBlock_General,
		_Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 ());

	// (HL) -> OUT(BC) & Previous memory position & --Counter until B = 0;
	// 21 Cycles when B != 0 (_additionalCycles = 5). _FINISH = true when B == 0
	_INSTZ80_MCS_FROM (0xEDBB,	2, 16,	"OTDR",					OTDR, OUTBlock_General,
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 () },
		FZ80::MCyclesStructure { _Z80_M1 (), _Z80_M1_5 (), _Z80_MR3 (), _Z80_IOW4 (), _Z80_I5 () });

}

#endif
  
// End of the file
/*@}*/
