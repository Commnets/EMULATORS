/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6510.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The main CPU of the C64.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6510__
#define __F6500_C6510__

#include <core/CPU.hpp>

namespace F6500
{
	/** The Chip CPU 6510 */
	class C6510 : public MCHEmul::CPU
	{
		public:
		/** The different possibilities a 6500 instruction has to understand its parameters. */
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

		C6510 ();

		MCHEmul::Register& accumulator ()
							{ return (internalRegister (_ACCUMULATOR)); }
		MCHEmul::Register& xRegister ()
							{ return (internalRegister (_XREGISTER)); }
		MCHEmul::Register& yRegister ()
							{ return (internalRegister (_YREGISTER)); }

		/** To identify the number of the registers. */
		static const size_t _ACCUMULATOR = 0;
		static const size_t _XREGISTER = 1;
		static const size_t _YREGISTER = 2;

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
