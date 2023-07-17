/** \ingroup CORE */
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
#include <CORE/DebugFile.hpp>
#include <CORE/CPUArchitecture.hpp>
#include <CORE/Chip.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Register.hpp>
#include <CORE/CPUInterrupt.hpp>
#include <CORE/ProgramCounter.hpp>
#include <CORE/StatusRegister.hpp>
#include <CORE/Instruction.hpp>
#include <fstream>

namespace MCHEmul
{
	/** The center of any Machine. */
	class CPU : public InfoClass
	{
		public:
		CPU (const CPUArchitecture& a, const Registers& r, const StatusRegister& sR, const Instructions& ins);

		CPU (const CPU&) = delete;

		CPU& operator = (const CPU&) = delete; 

		virtual ~CPU ();

		CPU (CPU&&) = delete;

		CPU& operator = (CPU&&) = delete; 

		bool stopped () const
							{ return (_stopped); }
		void setStop (bool s)
							{ _stopped = s; }

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

		/** To get the next instruction (if any) attending to the location of the Program Counter. 
			This method is not invoked but only from debugging reasons. */
		const Instruction* nextInstruction () const;
		const Instruction* lastInstruction () const
							{ return (_lastInstruction); }

		/** The CPU is not the owner of the memory, but the computer (just to keep all in the same place)
			A reference is here given to simplify the execution of transactions. */
		void setMemoryRef (Memory* m)
							{ _memory = m; }
		const Memory* memoryRef () const
							{ return (_memory); }
		Memory* memoryRef () 
							{ return (_memory); }

		/** To initialize the CPU. It could be overloaded later. \n
			By default it just initialize registers and program counter to 0. \n
			Returns true if everything was ok and false in any other case. */
		virtual bool initialize ();

		/** There is a possibility to restart the CPU. \n
			Be really carefully when using this possibility. \n
			By default it does nothing. \n
			Return true when evrything is ok, and false in other case. */
		virtual bool restart ()
							{ return (true); }

		/** To add and remove interrupts. */
		const CPUInterrupts& interrupts () const
							{ return (_interrupts); }
		bool existsInterrupt (int id) const
							{ return (_interrupts.find (id) != _interrupts.end ()); }
		const CPUInterrupt* interrupt (int id) const
							{ return ((*_interrupts.find (id)).second); }
		CPUInterrupt* interrupt (int id)
							{ return ((*_interrupts.find (id)).second); }
		void addInterrupt (CPUInterrupt* in);
		void removeInterrrupt (int id);

		/** The core:
			To execute the next instruction. \n
			The execution of the instruction can be debugged. \n
			The clock is affected. @see method lastClockCycles. */
		bool executeNextInstruction ();

		// The CPU generates the clock cycles,
		// than can be transmited to the rest of chips in the computer.
		/** The number of clockcycles since restarting. */
		unsigned int clockCycles () const
							{ return (_clockCycles); }
		/** Add or subtract clock cycles. */
		void addClockCycles (unsigned int cC)
							{ _clockCycles += cC; }
		void subtractClockCycles (unsigned int cC)
							{ _clockCycles -= cC; }
		/** To know the cycles of the last CPU execution. */
		unsigned int lastCPUClockCycles () const
							{ return (_lastCPUClockCycles); }
		/** To know the cycles of the last reading activities. */
		unsigned int lastCPUReadingClockCycles () const
							{ return (_lastCPUReadingClockActivities); }

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int error () const
							{ return (_error); }
		void resetErrors ()
							{ _error = _NOERROR; }

		/**
		  *	The name of the fields are: \n
		  * ARCHITECURE = InfoStructure: Architecture info. \n
		  *	REGS		= InfoStructure: Registers info. \n
		  * PC			= Attribute: Value of the program counter. \n
		  *	SR			= Attribute: Value of the status register.
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** Manages the deep debug file. \n
			Take care it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF)
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Adjusted at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Adjusted at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;
		Memory* _memory; // A reference...
		CPUInterrupts _interrupts;

		Instruction* _lastInstruction;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		unsigned int _error;
		/** The cycles that the CPU has executed since it started. 
			This counter is put back to 0 when the CPU is restarted in any way. */
		unsigned int _clockCycles;
		/** The cycles that the CPU took last time it executed. 
			Usually they are the cycles of a instruction except when interruptions
			happens that the time to set up the interruption is also taken into account. */
		unsigned int _lastCPUClockCycles;
		/** THe number of _lastCPUClockCycles spend in reading activities. */
		unsigned int _lastCPUReadingClockActivities;
		bool _stopped; // When the CPU is stopped and no runCycle is executed...
		/** The instructions will be moved into an array at construction time,
			to speed up their access in the executeNextInstruction method. */
		std::vector <Instruction*> _rowInstructions;
	};
}

#endif
  
// End of the file
/*@}*/

