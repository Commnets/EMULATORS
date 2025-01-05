/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6500.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: All CPUs type 6500 inherits from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6500__
#define __F6500_C6500__

#include <CORE/incs.hpp>
#include <F6500/IRQInterrupt.hpp>
#include <F6500/NMIInterrupt.hpp>

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

		C6500 (int id, const MCHEmul::Attributes& attrs = { });

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

		virtual void restartPC () override
							{ programCounter ().setAddress (MCHEmul::Address 
								(memoryRef () -> values (ResetVectorAddress (), 2), false /** little - endian */)); }

		/** Just set the program counter with the address where the cpu
			holds the vector address needed to restart the cpu. */
		virtual bool restart () override
							{ programCounter ().setAddress 
								(MCHEmul::Address (memoryRef () -> bytes (ResetVectorAddress (), 2), false)); 
							  return (true); }

		protected:
		/** In the case of the 6500, when a stop request is received, 
			it will only take effect when the first `_CYCLEREAD` of the next instruction is about to be executed. \n
			If the cycle being executed during the current instruction is a `_CYCLEWRITE`, the CPU will continue executing.  
			This code is usually invoked when simulating a chip in the computer where the CPU is inserted. 
			In such cases, the instruction would have already been executed, along with all its cycles. \n
			To handle this scenario, the solution involves checking the type of the first overlapping cycle. 
			If it is a `_CYCLEREAD`, the waiting period should be extended by the number of overlapped cycles. 
			If it is a `_CYCLEWRITE`, the waiting period should only be extended for the cycles following the last 
			`_CYCLEWRITE` type (usually none, except in the cases of the BRK and JSR instructions)....\n
			IMPORTANT NOTE: In the first case, the actualization of the memory should be stopped. \n
			That actualization has to be configured as "buffered" in the definition of the memory. */
		virtual bool unbufferCommands () override;

		virtual MCHEmul::CPUInterruptSystem* createInterruptSystem () const override
							{ return (new MCHEmul::StandardCPUInterruptSystem 
								({ { F6500::IRQInterrupt::_ID, new F6500::IRQInterrupt }, 
								   { F6500::NMIInterrupt::_ID, new F6500::NMIInterrupt } })); }

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		void debugUnbufferCommands ();

		private:
		// Implementation
		static MCHEmul::CPUArchitecture createArchitecture ();
		static MCHEmul::Registers createInternalRegisters ();
		static MCHEmul::StatusRegister createStatusRegister ();
		static MCHEmul::Instructions createInstructions ();
	};
}

#endif
  
// End of the file
/*@}*/
