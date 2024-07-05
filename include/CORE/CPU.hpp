/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPU.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Defines common elements to any CPU.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPU__
#define __MCHEMUL_CPU__

#include <CORE/global.hpp>
#include <CORE/CPUHook.hpp>
#include <CORE/MBElement.hpp>
#include <CORE/DebugFile.hpp>
#include <CORE/CPUArchitecture.hpp>
#include <CORE/Bus.hpp>
#include <CORE/Chip.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Register.hpp>
#include <CORE/CPUInterrupt.hpp>
#include <CORE/ProgramCounter.hpp>
#include <CORE/StatusRegister.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/CPUInterrupt.hpp>
#include <fstream>
#include <vector>

namespace MCHEmul
{
	/** 
	  * The center of any Machine. \n
	  * This is maybe the most important class in the emulator framework. \n
	  * As any other mother board element (@see MCHEmul::MotherboardElement) its execution is controlled by a clock.
	  * But in the case of the CPU the clock can also be internal. \n
	  * When the clock is internal (_ticksCounter == nullptr), 
	  * the whole computer (@see Computer) speed is then driven by the CPU instruction execution speed,
	  * it is, that the CPU execute full instructions and computer clock ticks are updated by the last instruction cycles executed. \n
	  * Whereas when the clock is external (_ticksCounter != nullptr) the computer speed is updated by 1 in each cycle. \n
	  * In computers like C64, VIC20 or ZX81, all elements share the same clock, 
	  * so it might considered that the clock is CPU internal. \n
	  * In others, like C264, the TED register (video chip) is the one leading the computer 
	  *	and the CPU clock is derivated from the TED's clock. \n
	  * Any CPU, as this library understood them, is made up of:
	  * Registers:			The number of these can vary and also the length in bits of each. \n
	  *						Usually this length is never longer that the one defined by the architecture. \n
	  * PC Counter:			An special register which length in bits is like the architecture,
	  *						It will define the number of positions reachable from the CPU. \n
	  * Status Register:	An special register that defines flags to indicate the result 
	  *						of the operations executed from the CPU. \n
	  * Instructions:		A set of instructions with the length defined by the architecture each. \n
	  * Interrupts:			A set of interrupts type that can be active or desactive from the CPU.
	  * The method to execute one instruction is: executeNextCycle (@see method executeNextCycle), 
	  * than can be overlaoded (@see method).
	  */
	class CPU : public MotherboardElement
	{
		public:
		/** When a interrupt type is requested, 
			this is the info to be provided from any external element. 
			The struct can be initialised usign intilizator list. \n
			None of the are mandatory. So it is an open structure. */
		struct InterruptRequest
		{
			/** Type. 
				Must match the type of instructions (see @_rowInterrupts) 
				but no double check is done when used (for performance reasons). */
			int _type;
			/** The machine cycles when the interrupt was requested. */
			unsigned int _cycles;
			/** Who is launching the request. */
			MCHEmul::Chip* _from;
			/** Reason to invoke it. */
			int _reason;

			std::string toString () const
							{ return (std::to_string (_type)
								+ " from:" + ((_from == nullptr) ? "-" : std::to_string (_from -> id ())) 
								+ " reason: " + std::to_string (_reason)); }

			friend std::ostream& operator << (std::ostream& o, const InterruptRequest& iR)
							{ return (o << iR.toString ()); }
		};

		// To simplify the use of a list of requests...
		using InterruptRequests = std::vector <InterruptRequest>;

		/** This static const member is to refer to a no interrupt request. */
		static const InterruptRequest _NOINTREQUEST;

		// States of the CPU
		/** The possible different states of the CPU. 
			As the CPU can be overloaded, additional states could be added later. */
		/** When a instruction is being executed... \n
			Every instruction is made up of different internal steps \n
			So the system will be in this state while a cycle of a instruction was still pending. */
		static const unsigned int _EXECUTINGINSTRUCTION = 0; 
		/** When the CPU is stopped because it has been requested (i.e) from other chip of the motherboard.
			The CPU can be stopped for a number of cycles or "forever" (until someone unstops it). */
		static const unsigned int _STOPPED = 1;
		// Many other states can be added but these a re the default ones
		// These states are managed in the method @see executeNextCycle, that is the core of the CPU
		// So if more states were added the method would have to be overloaded too...

		/** The most important attribute is maybe the "set of instructions". \n
			Define them very carefully. They are the CORE of the processor. 
			Notice that no list of interrupt is given at construction time. 
			They must be added (if any) in the body of the inheriting constructor. */
		CPU (int id, const CPUArchitecture& a, 
			 const Registers& r, const StatusRegister& sR, const Instructions& ins,
			 const Attributes& = { });

		CPU (const CPU&) = delete;

		CPU& operator = (const CPU&) = delete; 

		virtual ~CPU ();

		CPU (CPU&&) = delete;

		CPU& operator = (CPU&&) = delete; 

		const CPUArchitecture& architecture () const
							{ return (_architecture); }

		/** Is the CPU stopped for any reason? 
			The type of cycles why the CPU is stopped for is optional (all as default). */
		bool stopped (unsigned int tC = InstructionDefined::_CYCLEALL) const
							{ return (_state == _STOPPED && ((_typeCycleStopped & tC) != 0)); }
		/** 
		  *	By default: \n
		  *	Only possible to stop (s = true) if it was either executing an instruction or starting an interruption. \n
		  *	Only possible to restart back (s = false) when it was previouly stopped. \n
		  * The stop can be partial.
		  * It can be for a specific type of cycle (of the transaction under execution), and number of those (nC).
		  * By default the CPU is stopped forever! \n
		  * This behaviour can be overloaded later.
		  */
		virtual void setStop (bool s /** true when stop, 0 when run. */, 
			unsigned int tC, /** Type of cycle affected when stop. 0 means none, than might be contradictory with s value. */
			unsigned int cC, /** Number of cycles of the microprocessor when the the stop was requested. */
			int nC = -1 /** how many cycles when s = true. -1 will mean forever. */);

		// Info about the registers...
		const Registers& internalRegisters () const
							{ return (_registers); }
		Registers& internalRegisters ()
							{ return (_registers); }
		bool existsInternalRegister (size_t nR) const
							{ return (nR < _registers.size ()); }
		/** In commands this instruction can be usefull. \
			Never use it in main loop because it is really slow. */
		bool existsInternalRegister (const std::string& rn) const
							{ return (std::find_if (_registers.begin (), _registers.end (), 
								[&](const Register& r) -> bool { return (r.name () == rn); }) != _registers.end ()); }
		const Register& internalRegister (size_t nR) const
							{ return (existsInternalRegister (nR) ? _registers [nR] : NoRegister); }
		/** To get it by name. \n
			Never used it in the main loop, because it is very slow. */
		const Register& internalRegister (const std::string& rn) const
							{ return (existsInternalRegister (rn)
								? *(std::find_if (_registers.begin (), _registers.end (), 
									[&](const Register& r) -> bool { return (r.name () == rn); }))
								: TrashRegister); }
		Register& internalRegister (size_t nR)
							{ return (existsInternalRegister (nR) ? _registers [nR] : TrashRegister); }
		Register& internalRegister (const std::string& rn)
							{ return (existsInternalRegister (rn)
								? *(std::find_if (_registers.begin (), _registers.end (), 
									[&](Register r) -> bool { return (r.name () == rn); }))
								: TrashRegister); }
		void setInternalRegister (size_t nR, UBytes v)
							{ if (existsInternalRegister (nR) && internalRegister (nR).accept (v)) 
								internalRegister (nR).set (v); }

		/** About the memory and the address bus...
			The address and the data bus can be programmed as busses or wires,
			but in a very basic version of the code, it is possbile to maintain the 
			address bus and the data bus. 
			The instructions has to actualize this content if it is decides to follow this way. */
		const Address& lastINOUTAddress () const
							{ return (_lastINOUTAddress); }
		void setLastINOUTAddress (const Address& a)
							{ _lastINOUTAddress = a; }
		const UBytes& lastINOUTData () const
							{ return (_lastINOUTData); }
		void setLastINOUTData (const MCHEmul::UBytes& d)
							{ _lastINOUTData = d; }

		// Info about the program counter...
		const ProgramCounter& programCounter () const
							{ return (_programCounter); }
		ProgramCounter& programCounter ()
							{ return (_programCounter); }

		// Info about the ststus register...
		const StatusRegister& statusRegister () const
							{ return (_statusRegister); }
		StatusRegister& statusRegister ()
							{ return (_statusRegister); }

		// To access instructions...
		bool existsInstruction (unsigned int i) const
							{ return (_instructions.find (i) != _instructions.end ()); }
		const Instructions& instructions () const
							{ return (_instructions); }
		const Instruction* instruction (unsigned int i) const
							{ return ((*_instructions.find (i)).second); }
		Instruction* instruction (unsigned int i)
							{ return ((*_instructions.find (i)).second); }

		/** To get the next instruction (if any) attending to the location of the Program Counter. 
			This method is not invoked often but maybe from debugging situations. */
		const Instruction* nextInstruction () const
							{ return (CPU::instructionAt 
								(_memory, _programCounter.asAddress (), const_cast <CPU*> (this))); }
		/** To get the last instruction fully executed. */
		const Instruction* lastInstruction () const
							{ return (_lastInstruction); }
		/** To get the instruction under executuion. 
			When the clock is internal and a instruction is in each loop the value of this variable 
			is always the same than the previous one, except when a interrupt happens. */
		const Instruction* currentInstruction () const
							{ return (_currentInstruction); }

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
		virtual bool initialize () override;

		/** Restart the PC, by default it does nothing, 
			but it can be overloaded. */
		virtual void restartPC ()
						{ programCounter ().initialize (); }

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

		// Managing the requests of interrutps...
		/** To request an interrupt. \n
			Receives the id of the interruption requested, the clockCycle where it has happened,
			the sender (optional), and a code for the reason (also optional, -1 = not defined. */
		void requestInterrupt (int id, unsigned int nC, Chip* src = nullptr, int cR = -1)
							{ requestInterrupt (InterruptRequest ({ id, nC, src, cR })); }
		/** Same than previous, but receiving a InterruptRequest as parameter. \n
			This one can be overloaded. \n
			By default a unique element is allowed in the list of pendings. */
		virtual void requestInterrupt (const InterruptRequest& iR);
		/** To know whether the interrupt requests pending. Empty object if none. */
		const InterruptRequests& interruptsRequested () const
							{ return (_interruptsRequested); }
		/** To get the next interrupt in the list of those. \n 
			The way the interrupt is choosen can be changed by the user. \n
			The default behaviour is to keep just one, so the first is brought back!. 
			If there weren't be any, _NOINTREQUEST const object reference is returned. */
		virtual const InterruptRequest& getNextInterruptRequest ()
							{ return (_interruptsRequested.empty () 
								? _NOINTREQUEST : *_interruptsRequested.begin ()); }
		/** Once a interrupt request is procesed must be removed from the list of the pending ones. \n
			As the default behaviour is keeping just one, all list is deleted (quicker). */
		virtual void removeInterruptRequest (const InterruptRequest& iR)
							{ _interruptsRequested = { }; }
		/** To recognize the interrupt. \n 
		 	By default, it does nothing, but it can be overloaded. \n
			This method is invoked from the previous one. */
		virtual void aknowledgeInterrupt ()	{ }

		// Managing hooks...
		/** Adding a hook/hooks */
		inline void addHook (unsigned long a, CPUHook* h);
		inline void addHooks (const CPUHooks& hs);

		/** 
		  *	The real CORE of the class. \n
		  *	The behaviour can be changed and adapted to different types of CPUs, overloading this method. \n
		  *	However the definition has always to be by cycle, as the CPU manages cycles.
		  *	The CPU can be in different states. By default 2 are supported: \n
		  *	1.- _EXECUTINGINSTRUCTION:	To execute a new instruction. \n
		  *								Incrementes the _clockCycles variable in the number of clocks of the 
		  *								instruction/interrupt executed. \n
		  *	2.-	_STOPPED:				Just passes a cycle until the maximum declared is reached. \n
		  *								Increments the _clokcCycles variable is just 1.
		  * However new states can be supported extending the class.
		  */
		virtual bool executeNextCycle ();

		// Controlling the INTERNAL clock
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
		  *	SR			= Attribute: Value of the status register. \n
		  * CLK			= Attribute: Value of the internal clock cycle (could be o if it is external).
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
		/** To make a row data with the interruptions. \n
			This method is called from the constructor and instantiate the variable _rowInterrupts,
			and also at any time an interrupt is added or removed. */
		void makeInterruptionRowData ();

		// Internal methods to simplify the comprension of the code.
		// However they can be overloaded...
		// Invoke from executeNextInstruction
		/** 
		  * When the state is: _EXECUTINGINSTRUCTION.
		  * The method can be overloaded. \n
		  *	Execute the next either instruction or interrupt. \n
		  *	The execution can be done either by cycle or by full instruction/instruct. \n
		  * Per cycle: \n
		  *	If a instruction takes e.g. 2 cycles, 
		  *	only when the CPU has passed cycle 2 after loading the instruction this is executed. \n
		  *	The computer's clock is incremented in one cycle only each loop (@see MCHEmul::Computer::runComputerCycle method). \n
		  * In this case invokes the methods @see executeNextInterruptRequest_PerCycle 
		  *	and @executeNextInstruction_PerCycle. \n
		  * Full: \n
		  *	Execute the full instruction/interrupt launch. \n
		  *	The computer's clock is incremented in the number of cycles the instruction took. \n
		  * In this case invokes the methods @see executeNextInterruptRequest_Full and @executeNextInstruction_Full. \n
		  * \n
		  * By default, the interrupt has preference over the instruction.
		  * but the instruction under execution has to finish before launching the execution of a interrupt
		  * and it is only executed when it is possible (@see MCHEmul::CPUInterrupt::canBeExecuted method).
		  * NOTE: The method returns true if everything was ok, and false when wasn't.
		  */
		virtual bool when_ExecutingInstruction ();
		/** When the state is: _STOPPED. \n
			NOTE: The method returns true if everything was ok, and false when not. */
		virtual bool when_Stopped ();

		// Implementation
		/** To execute the next interrupt. \n 
		 	NOTE: It returns true if a interrupt was finally "executed" and false if not. \n 
			The variable "e" holds whether there were or nor an error executing the interrupt (if any).  \n
			Methods invoked from @see when_ExecutingInstruction. */
		virtual bool executeNextInterruptRequest_PerCycle (unsigned int& e);
		virtual bool executeNextInterruptRequest_Full (unsigned int& e);
		/** ...and finally execute the instruction.
		 	NOTE: It returns true if a instruction was finally "executed" and false if not. \n 
			The variable "e" holds whether there were or nor an error executing the instruction (if any). \n
			Methods invoked from @see when_ExecutingInstruction. */
		virtual bool executeNextInstruction_PerCycle (unsigned int &e);
		virtual bool executeNextInstruction_Full (unsigned int &e);

		/** To get the instruction from a specific position of the memory. 
			If the instruction can not be created, a nullptr is returned. */
		static Instruction* instructionAt (Memory* m, const Address& addr, CPU* c);

		/** To execute the hook if any at the position where the PC is now. */
		inline CPUHook* executeHookIfAny ();

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Adjusted at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Adjusted at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;
		Memory* _memory; // A reference...
		CPUInterrupts _interrupts;
		CPUHooks _hooks;

		// The current situation of the CPU...
		/** Last INOUT data used. 
			It is not mandatory, but if used, instructions have to set the values. */
		Address _lastINOUTAddress;
		UBytes _lastINOUTData;
		/** The state. */
		unsigned int _state;
		/** The cycles that the CPU has executed since it started. 
			This counter is put back to 0 when the CPU is restarted in any way. */
		unsigned int _clockCycles;
		/** The cycles that the CPU took last time it executed. 
			Usually they are the cycles of a instruction except when interruptions
			happens that the time to set up the interruption is also taken into account. */
		unsigned int _lastCPUClockCycles;
		/** The last state. */
		unsigned int _lastState;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		unsigned int _error;

		// When things are executed by cycle...
		// In full instructions this is also used to avoid more interim variables, 
		// but makes no much more sense!
		/** The request under execution. */
		InterruptRequest _currentInterruptRequest;
		/** The interruption under exection. nullptr when nothing. */
		CPUInterrupt* _currentInterrupt;
		/** The instruction under execution. nullptr when nothing. */
		Instruction* _currentInstruction;
		/** Cycles pending to be executed from either 
			the current instruction or the current interruption. */
		unsigned int _cyclesPendingExecution; 

		// Parameters related with the differet states of the CPU...
		// When _EXECUTINGINST:
		/** Last instruction executed. */
		Instruction* _lastInstruction;

		// When _STOPPED:
		/** Type of cycle stopped. */
		unsigned int _typeCycleStopped;
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
		InterruptRequests _interruptsRequested;

		/** The instructions will be moved into an array at construction time,
			to speed up their access in the executeNextInstruction method. */
		ListOfInstructions _rowInstructions;
		/** Also a row vector with the interruptions. */
		CPUListOfInterrupts _rowInterrupts;
	};

	// ---
	inline void CPU::addHook (unsigned long a, CPUHook* h)
	{
		MCHEmul::CPUHooks::const_iterator i = _hooks.find (a);
		if (i == _hooks.end ())
			_hooks.insert (MCHEmul::CPUHooks::value_type (a, h));
	}

	// ---
	inline void CPU::addHooks (const CPUHooks& hs)
	{
		for (const auto& i : _hooks)
			delete (i.second);

		_hooks = hs;
	}

	// ---
	inline MCHEmul::CPUHook* CPU::executeHookIfAny ()
	{
		MCHEmul::CPUHook* result = nullptr;

		MCHEmul::CPUHooks::const_iterator i = 
			_hooks.find (_programCounter.internalRepresentation ());
		if (i != _hooks.end ())
			(result = (*i).second) -> hook (this);

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/

