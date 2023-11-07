/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6500.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: All CPUs type 6500 inherits from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6500__
#define __F6500_C6500__

#include <CORE/incs.hpp>

namespace F6500
{
	/** The Chip CPU type 6500 */
	class C6500 : public MCHEmul::CPU
	{
		public:
		/** To identify the number of the registers. */
		static const size_t _ACCUMULATOR = 0;
		static const size_t _XREGISTER = 1;
		static const size_t _YREGISTER = 2;

		/** To identify the position of the flags in the status register. */
		static const size_t _CARRYFLAG = 0;
		static const std::string _CARRYFLAGNAME;
		static const size_t _ZEROFLAG = 1;
		static const std::string _ZEROFLAGNAME;
		static const size_t _IRQFLAG = 2;
		static const std::string _IRQFLAGNAME;
		static const size_t _DECIMALFLAG = 3;
		static const std::string _DECIMALFLAGNAME;
		static const size_t _BREAKFLAG = 4;
		static const std::string _BREAKFLAGNAME;
		static const size_t _OVERFLOWFLAG = 6;
		static const std::string _OVERFLOWFLAGNAME;
		static const size_t _NEGATIVEFLAG = 7;
		static const std::string _NEGATIVEFLAGNAME;

		/** The different possibilities a 6500 instruction set has to understand its parameters. */
		enum class AddressMode
		{
			_IMPLICIT,		// The instruction has no parameter at all.
			_INMEDIATE,		// The parameter is a value
			_ABSOLUTE,		// The parameter is an address
			_ZEROPAGE,		// The parameter is an addresss to but in th first page of the memory
			_ABSOLUTE_X,	// The paremeter is got from an address and the value of the register X
			_ABSOLUTE_Y,	// The paremeter is got from an address and the value of the register Y
			_RELATIVE,		// The parameter is a number of steps foward or backward
			_INDIRECT,		// The parameter is an address where to find other
			_ZEROPAGE_X,	// The paremeter is got from an address (in page 0) and the value of the register X 
			_ZEROPAGE_Y,	// The paremeter is got from an address (in page 0) and the value of the register Y
			_INDIRECT_X,	// The parameter is got from an address that is in other, plus the register X
			_INDIRECT_Y,	// The parameter is got from an address that is in other plus the register Y
		};

		C6500 (const MCHEmul::CPUArchitecture& a);

		virtual MCHEmul::Address IRQVectorAddress () const
							{ return (MCHEmul::Address ({ 0xfe, 0xff }, false /** Little - endian */)); }
		virtual MCHEmul::Address NMIVectorAddress () const
							{ return (MCHEmul::Address ({ 0xfa, 0xff }, false /** Little - endian */)); }
		virtual MCHEmul::Address ResetVectorAddress () const
							{ return (MCHEmul::Address ({ 0xfc, 0xff }, false /** Little - endian */)); }

		// Accessing the registers...
		MCHEmul::Register& accumulator ()
							{ return (internalRegister (_ACCUMULATOR)); }
		const MCHEmul::Register& accumulator () const
							{ return (internalRegister (_ACCUMULATOR)); }
		MCHEmul::Register& xRegister ()
							{ return (internalRegister (_XREGISTER)); }
		const MCHEmul::Register& xRegister () const
							{ return (internalRegister (_XREGISTER)); }
		MCHEmul::Register& yRegister ()
							{ return (internalRegister (_YREGISTER)); }
		const MCHEmul::Register& yRegister () const
							{ return (internalRegister (_YREGISTER)); }

		virtual bool initialize () override;

		/** Just set the program counter with the address where the cpu
			holds the vector address needed to restart the cpu. */
		virtual bool restart () override
							{ programCounter ().setAddress 
								(MCHEmul::Address (memoryRef () -> bytes (ResetVectorAddress (), 2), false)); 
							  return (true); }

		private:
		// Implementation
		static MCHEmul::Registers createInternalRegisters ();
		static MCHEmul::StatusRegister createStatusRegister ();
		static MCHEmul::Instructions createInstructions ();
	};
}

#endif
  
// End of the file
/*@}*/
