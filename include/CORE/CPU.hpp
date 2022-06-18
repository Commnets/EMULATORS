/** \ingroup CPU */
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

#include <CORE/global.hpp>
#include <CORE/CPUArchitecture.hpp>
#include <CORE/Chip.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Register.hpp>
#include <CORE/CPUInterrupt.hpp>
#include <CORE/ProgramCounter.hpp>
#include <CORE/StatusRegister.hpp>
#include <CORE/Instruction.hpp>

namespace MCHEmul
{
	/** The center of any Machine. */
	class CPU
	{
		public:
		CPU () = delete;

		CPU (const CPUArchitecture& a, const Registers& r, const StatusRegister& sR, const Instructions& ins)
			: _architecture (a), _registers (r), _statusRegister (sR), _instructions (ins),
			  _programCounter (a.numberBytes ()), _memory (nullptr), _interrupts (),
			  _lastError (_NOERROR), _clockCycles (0) 
							{ assert (_registers.size () > 0 && _instructions.size () > 0); }

		CPU (const CPU&) = delete;

		CPU& operator = (const CPU&) = delete; 

		virtual ~CPU ();

		const CPUArchitecture& architecture () const
							{ return (_architecture); }

		const Registers& internalRegisters () const
							{ return (_registers); }
		bool existsInternalRegister (size_t nR) const
							{ return (nR < _registers.size ()); }
		const Register& internalRegister (size_t nR) const
							{ return (existsInternalRegister (nR) ? _registers [nR] : NoRegister); }
		Register& internalRegister (size_t nR)
							{ return (existsInternalRegister (nR) ? _registers [nR] : TrashRegister); }
		void setInternalRegister (size_t nR, UBytes v)
							{ if (existsInternalRegister (nR) && internalRegister (nR).accept (v)) internalRegister (nR).set (v); }

		const ProgramCounter& programCounter () const
							{ return (_programCounter); }
		ProgramCounter& programCounter ()
							{ return (_programCounter); }

		const StatusRegister& statusRegister () const
							{ return (_statusRegister); }
		StatusRegister& statusRegister ()
							{ return (_statusRegister); }

		bool existsInstruction (unsigned int i) const
							{ return (_instructions.find (i) != _instructions.end ()); }
		const Instructions& instructions () const
							{ return (_instructions); }
		const Instruction* instruction (unsigned int i) const
							{ return ((*_instructions.find (i)).second); }
		Instruction* instruction (unsigned int i)
							{ return ((*_instructions.find (i)).second); }

		/** The CPU is not the owner of the memory, but the computer (just to keep all in the same place)
			A reference is here given to simplify the execution of transactions. */
		void setMemoryRef (Memory* m)
							{ _memory = m; }
		const Memory* memoryRef () const
							{ return (_memory); }
		Memory* memoryRef () 
							{ return (_memory); }

		unsigned int clockCycles () const
							{ return (_clockCycles); }
		void addClockCycles (unsigned int cC)
							{ _clockCycles += cC; }

		/** To initialize the CPU. It could be overloaded later. \n
			By default it just initialize registers and program counter to 0. \n
			Returns true if everything was ok and false in any other case. */
		virtual bool initialize ();

		/** To add and remove interrupts. */
		bool existsInterrupt (int id) const
							{ return (_interrupts.find (id) != _interrupts.end ()); }
		const CPUInterrupt* interrupt (int id) const
							{ return ((*_interrupts.find (id)).second); }
		CPUInterrupt* interrupt (int id)
							{ return ((*_interrupts.find (id)).second); }
		void addInterrupt (CPUInterrupt* in);
		void removeInterrrupt (int id);

		/** To execute the next transaction,
			Return true if everything was ok and false in any other case. */
		bool executeNextInstruction ();

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const CPU& c);

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Adjusted at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Adjusted at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;

		Memory* _memory; // A reference...

		CPUInterrups _interrupts;

		// Implementation
		unsigned int _lastError;
		unsigned int _clockCycles;
	};
}

#endif
  
// End of the file
/*@}*/

