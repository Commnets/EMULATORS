/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: CPU.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines common elements to any CPU.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPU__
#define __MCHEMUL_CPU__

#include <assert.h>
#include <ostream>

#include <global.hpp>
#include <core/CPUArchitecture.hpp>
#include <core/Chip.hpp>
#include <core/Memory.hpp>
#include <core/Register.hpp>
#include <core/ProgramCounter.hpp>
#include <core/StatusRegister.hpp>
#include <language/Instruction.hpp>

namespace MCHEmul
{
	/** The center of any Machine. */
	class CPU
	{
		public:
		CPU () = delete;

		CPU (const CPUArchitecture& a, const Registers& r, const StatusRegister& sR, const Instructions& ins)
			: _architecture (a), _registers (r), _statusRegister (sR), _instructions (ins),
			  _programCounter (a.numberBytes ()), _memory (nullptr),
			  _lastError (_NOERROR), _clockCycles (0) 
							{ assert (_registers.size () > 0 && _instructions.size () > 0); }

		CPU (const CPU&) = delete;

		CPU& operator = (const CPU&) = delete; 

		virtual ~CPU ();

		constexpr const CPUArchitecture& architecture () const
							{ return (_architecture); }

		constexpr const Registers& internalRegisters () const
							{ return (_registers); }
		bool existsInternalRegister (size_t nR) const
							{ return (nR < _registers.size ()); }
		constexpr const Register& internalRegister (size_t nR) const
							{ return (existsInternalRegister (nR) ? _registers [nR] : NoRegister); }
		Register& internalRegister (size_t nR)
							{ return (existsInternalRegister (nR) ? _registers [nR] : TrashRegister); }
		void setInternalRegister (size_t nR, UBytes v)
							{ if (existsInternalRegister (nR) && internalRegister (nR).accept (v)) internalRegister (nR).set (v); }

		constexpr const ProgramCounter& programCounter () const
							{ return (_programCounter); }
		ProgramCounter& programCounter ()
							{ return (_programCounter); }

		constexpr const StatusRegister& statusRegister () const
							{ return (_statusRegister); }
		StatusRegister& statusRegister ()
							{ return (_statusRegister); }

		/** The CPU is not the owner of the memory, but the computer (just to keep all in th same place)
			A reference is here given to simplify the execution of transactions. */
		void setMemoryRef (Memory* m)
							{ _memory = m; }
		const Memory* memoryRef () const
							{ return (_memory); }
		Memory* memoryRef () 
							{ return (_memory); }

		constexpr unsigned int clockCycles () const
							{ return (_clockCycles); }

		/** To initialize the CPU. It could be overloaded later. \n
			By default it just initialize registers and program counter to 0. \n
			Returns true if everything was ok and false in any other case. */
		virtual bool initialize ();

		/** To execute the next transaction,
			Return true if everything was ok and false in any other case. */
		bool executeNextTransaction ();

		/** To get the last error happend (after initialize or simulate methods). */
		constexpr unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const CPU& c);

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Modified at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Modified at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;

		Memory* _memory; // A reference...

		// Implementation
		unsigned int _lastError;
		unsigned int _clockCycles;
	};
}

#endif
  
// End of the file
/*@}*/

