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
#include <CORE/CPUInterrupt.hpp>
#include <fstream>

namespace MCHEmul
{
	/** The center of any Machine. */
	class CPU : public InfoClass
	{
		public:
		/** The possible different states of the CPU. 
			As the CPU can be overloaded, additional states could be added later. */
		/** When a instruction is being executed... \n
			Every instruction is made up of different internal steps \n
			So the system will be in this state while a cycle of a instruction was still pending. */
		static const unsigned int _EXECUTINGINSTRUCTION = 0; 
		/** When the CPU is stopped because it has been requested (i.e) from other chip of the motherboard.
			The CPU can be stopped for a number of cycles or "forever" (until someone unstops it). */
		static const unsigned int _STOPPED = 1;

		/** The most important attribute is maybe the "set of instructions". \n
			Define them very carefully. They are the CORE of the processor. */
		CPU (const CPUArchitecture& a, 
			const Registers& r, const StatusRegister& sR, const Instructions& ins);

		CPU (const CPU&) = delete;

		CPU& operator = (const CPU&) = delete; 

		virtual ~CPU ();

		CPU (CPU&&) = delete;

		CPU& operator = (CPU&&) = delete; 

		bool stopped () const
							{ return (_state == _STOPPED); }
		/** 
		  *	By default: \n
		  *	Only possible to stop (s = true) if it was either executing an instruction or starting an interruption. \n
		  *	Only possible to restart back (s = false) when it was previouly stopped.
		  * Once the CPU resumes the execution of what it was doing previoulsy.
		  * The stop can be partial. It is for a specific type of cycle (of the transaction under execution).
		  * This behaviour can be overloaded later.
		  */
		virtual void setStop (bool s /** true when stop, 0 when run. */, 
			unsigned int tC, /** Type of cycle affected when stop. 9999 means all. */
			unsigned int cC, /** Number of cycles of the microprocessor when the the stop was requested. */
			int nC = -1 /** how many cycles when s = true. -1 will mean forever. */);

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
		/** To get the last instruction fully executed. */
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

		// Managing interruptions
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
		/** And to request an interrupt. \n
			Receives the id of the interruption requested, the clockCycle where it has happened,
			and the sender (optional). \n
			It can be overloaded, but by default just only one interruption at the same time can be invoked. */
		virtual void requestInterrupt (int id, unsigned int nC, Chip* src = nullptr)
							{ if (_interruptRequested == -1)
								{ _interruptRequested = id; _clyclesAtInterruption = nC; } }

		/** 
		  *	The real CORE of the class. \n
		  *	The behaviour can be changed and adapted to different types of CPUs, overloading this method. \n
		  *	However the definition has always to be by cycle, as the CPU manages cycles.
		  *	The CPU can be in different states. By default 2 are supported: \n
		  *	1.- _EXECUTINGINSTRUCTION:	To execute a new instruction. \n
		  *								Increments the _clockCycles variable un the number of cycles that the last instruction finally took. \n
		  *	2.-	_STOPPED:				Just passes a cycle until the maximum declared is reached. \n
		  *								Increments the _clokcCycles variable is just 1.
		  * The CPU by default is always executing the first state unless it was stopped for a number of cycles. \n
		  * An interruption might be invoked to the CPU during the simulation of any other chip. \n
		  * If it was the case, before executing a new transaction, this situation will be taken into account.
		  */
		virtual bool executeNextCycle ();

		// Controlling the clock
		/** The number of clockcycles since restarting. */
		unsigned int clockCycles () const
							{ return (_clockCycles); }
		/** Add or subtract clock cycles to the total. 
			By careful using these two methods. */
		void addClockCycles (unsigned int cC)
							{ _clockCycles += cC; }
		void subtractClockCycles (unsigned int cC)
							{ _clockCycles -= cC; }
		/** To know the cycles of the last CPU instruction/interuption launching execution. */
		unsigned int lastCPUClockCycles () const
							{ return (_lastCPUClockCycles); }

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
			Take care: it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF)
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		/** To make a row data with the interruptions. */
		void makeInterruptionRowData ();

		// Internal methods to simplify the comprension of the code.
		// Invoke from executeNextInstruction
		/** When the state is: _EXECUTINGINST. */
		virtual bool when_ExecutingInstruction ();
		/** When the state is: _STOPPED. */
		virtual bool when_Stopped ();

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Adjusted at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Adjusted at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;
		Memory* _memory; // A reference...
		CPUInterrupts _interrupts;
		/** The state. */
		unsigned int _state;

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
		/** The last state. */
		unsigned int _lastState;

		// Parameters related with the differet states of the CPU...
		// When _EXECUTINGINST:
		/** Last instruction executed. */
		Instruction* _lastInstruction;

		// When _STOPPED:
		/** Number of cycles to be stopped. -1 when it is forever... */
		int _cyclesStopped; 
		/** Number of cycles of the processor when the stop was requested. */
		unsigned int _cyclesAtStop;
		/** Current number of cycles already stopped, 
			obviously when it is the situation and it is nor forever. */
		unsigned int _counterCyclesStopped;

		/** When the CPU is executing a transaction, and a interruption is requested,
			the current instruction has to finish first, and the the interruption is invoked. \n
			It the CPU were stopped, first of all the CPU would have to run back 
			and then finish the instruction that could be running when it was stopped. \n
			The parameter has the id of the interrupt requested. -1 means none. */
		int _interruptRequested;
		/** In some CPU is important to know there the clock was when the interruption was requested. */
		unsigned int _clyclesAtInterruption;

		/** The instructions will be moved into an array at construction time,
			to speed up their access in the executeNextInstruction method. */
		std::vector <Instruction*> _rowInstructions;
		/** Also a row vector with the interruptions. */
		std::vector <CPUInterrupt*> _rowInterrupts;
	};
}

#endif
  
// End of the file
/*@}*/

