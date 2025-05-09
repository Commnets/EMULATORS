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
	class CPU : public MotherboardElement, public Notifier, public DebugableClass
	{
		public:
		/** Structure for notification.
			There are many things that can be notified, so the data is very generic. 
			One thing is when it is about to execute an instruction. */
		struct EventData final : public Event::Data
		{
			EventData (void* d)
				: _data (d)
							{ }

			// It will be one thing ot another attending to the type of event launched!
			void* _data;
		};

		/** An event when the system is about to execute a instruction.
			Sometimes the executiom of a specific instruction could affect other parts of the computer,
			like chips (specifically or devices. */
		static const unsigned int _CPUTOEXECUTEINSTRUCTION = 130;

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

		/** The Stop Status is very special in the CPU.
			It is needed to keep a lot of information about it, 
			that can be managed in many different ways depending on the implementation of the CPU. */
		struct StopStatusData final
		{
			StopStatusData ()
				: _typeCyclesStopped (0),
				  _cyclesAtStop (0),
				  _cyclesStopped (0),
				  _cyclesAtCPU (0),
				  _cyclesLastInstructionExecutedStopRequest (0),
				  _cyclesLastInstructionOverlappedStopRequest (0),
				  _counterCyclesStopped (0)
							{ }

			/** The parameters are:
				@param tC	: Type of cycle where to stop. @see InstructionDefined. \n
				@param cC	: Cycle when the stop request came. \n
				@param nC	: For how many cycles must the CPU stop? -1 will mean forever. \n
							  Only either positive values or -1 are allowed!
				@param c	: Current cycle of the CPU, usually after having ececuted the last instruction. \n
				@param nLI	: Number of cycles of the last instruction. 
				The last two parameters are to calculate the overlap of the stop request with the last instruction. */
			StopStatusData (unsigned int tC, unsigned int cC, int nC, 
				unsigned int c, unsigned int nLI)
							{ initialize (tC, cC, nC, c, nLI); }

			/** To know whether the status is valid for certain type of cycles. */
			bool validForCycles (unsigned int tC) const
							{ return ((_typeCyclesStopped & tC) != 0); }

			/** Put all attributes to 0. */
			inline void reset ();

			/** Same effect that the constructor but without creating a new object.
				It is even invoked from the constructor. */
			inline void initialize (unsigned int tC, unsigned int cC, int nC,
				unsigned int c, unsigned int nLI);

			// Partial execution...
			/** Simnilar than the previous but not actualizing the information about the overlap. */
			inline void reset (unsigned tC, unsigned int cC, int nC, unsigned int c);
			/** To recalculate the overlap info. */
			inline void calcOverlapInfo (unsigned int nLI);

			// When running...
			/** Just running, 
				but is there still to maintain this state? 
				This method must be executed after being inialized and always the stop state remains. 
				Returns true if the status is still valid and false if it isn't. */
			bool run () const
							{ return (_cyclesStopped == -1 ||
								(_cyclesStopped != -1 &&
								 (int (++_counterCyclesStopped) < _cyclesStopped))); }
			/** To get the number of cycles still stopped.
				It returns -1 meaning forever. */
			int cyclesStillValid () const
							{ return (_cyclesStopped == -1 
								? _cyclesStopped // Meaning forever...
								: ((_cyclesStopped >= (int) _counterCyclesStopped) // Only if it is bigger...
									? (_cyclesStopped - (int) _counterCyclesStopped) : 0)); }

			/** To get the attributes. */
			Attributes attributes () const;
			/** To get information about the status in a string. */
			std::string asString () const;
			/** To get only the core info about the status. */
			std::string asStringCore () const;

			// It is not recommened to access directly to the attributes, but just in case...

			/** Type of cycle where to stop. */
			unsigned int _typeCyclesStopped;
			/** Number of cycles of the processor when the stop was requested. */
			unsigned int _cyclesAtStop;
			/** Number of cycles to be stopped. -1 means forever... */
			int _cyclesStopped; 
			/** The number of cycles at the CPU. */
			unsigned int _cyclesAtCPU;

			/** The stop request usually comes externally to the CPU.
				The way this simulation is done implies that the last instruction had been already executed 
				when the stop request same. So part (or all) of the last instruction shouldn't have vbeen executed.
				This structure calculate how many cycles were overlap of the last instruction were overlaped 
				and how many of them were really executed. */
			/** Number of cycles that the last Instruction overlapped when the stop was request, if any. */
			unsigned int _cyclesLastInstructionOverlappedStopRequest;
			/** Number of cycles of the last instruction that were executed before the stop was requested. */
			unsigned int _cyclesLastInstructionExecutedStopRequest;

			// Implementation
			/** Current number of cycles already stopped, 
				obviously when it is the situation and it is nor forever. 
				It is put to 0 when the stop is requested back. */
			mutable unsigned int _counterCyclesStopped;
		};

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

		/** To get the current state. */
		unsigned int state () const
							{ return (_state); }
		/** To get the one before the current one. */
		unsigned int lastState () const
							{ return (_lastState); }

		// Related with the state stopped...
		/** To get a reference to the stop status. */
		const StopStatusData& stopStatusData () const
							{ return (_stopStatusData); }
		StopStatusData& stopStatusData ()
							{ return (_stopStatusData); }
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
		/** Is the CPU stopped for any reason? 
			The type of cycles why the CPU is stopped for is optional (all as default). */
		bool stopped (unsigned int tC = InstructionDefined::_CYCLEALL) const
							{ return (_state == _STOPPED && _stopStatusData.validForCycles (tC)); }
		/** To know how many cycles the cpu will remain still stopped.
			= 0 if nothing ot if the state is = _RUNNING. */
		unsigned int cyclesRemainStopped () const
							{ return ((_state == _STOPPED) ? _stopStatusData.cyclesStillValid () : 0); }

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

		// Managing the interrupt system...
		/** To get the CPU Interrupt System. @see createInterruptSystem method. \n
			The system has to be created in the method createInterruptSystem that must be overloaded 
			for this purpose when defining every CPU in detail (by default it is virtual). 
			The rest of the methods are Facade of the the interruptSystem. */
		const CPUInterruptSystem* interruptSystem () const
							{ return ((_interruptSystem == nullptr) 
								? (_interruptSystem = createInterruptSystem ()) : _interruptSystem); }
		CPUInterruptSystem* interruptSystem ()
							{ return (const_cast <CPUInterruptSystem*> ((const_cast <const CPU*> (this)) -> interruptSystem ())); }
		bool existsInterrupt (int id) const
							{ return (interruptSystem () -> existsInterrupt (id)); }
		const CPUInterrupts& interrupts () const
							{ return (interruptSystem () -> interrupts ()); }
		const CPUInterrupt* interrupt (int id) const
							{ return (interruptSystem () -> interrupt (id)); }
		CPUInterrupt* interrupt (int id)
							{ return (interruptSystem () -> interrupt (id)); }

		// Managing the requests of interrutps...
		// This block delegates everything in the interrupt system acting as a Facade design pattern...
		void requestInterrupt (int id, unsigned int nC, Chip* src = nullptr, int cR = -1)
							{ requestInterrupt (CPUInterruptRequest (id, nC, src, cR)); }
		void requestInterrupt (const CPUInterruptRequest& iR)
							{ if (interruptSystem () -> requestInterrupt (iR)) 
								_IFDEBUG debugInterruptRequest (iR); }
		const CPUInterruptRequests& interruptsRequested () const
							{ return (interruptSystem () -> interruptsRequested ()); }
		const CPUInterruptRequest& getNextInterruptRequest ()
							{ return (interruptSystem () -> getNextInterruptRequest ()); }
		const CPUInterrupt* getInterruptForRequest (const CPUInterruptRequest& iR) const
							{ return (interruptSystem () -> getInterruptForRequest (iR)); }
		CPUInterrupt* getInterruptForRequest (const CPUInterruptRequest& iR)
							{ return (interruptSystem () -> getInterruptForRequest (iR)); }
		void removeInterruptRequest (const CPUInterruptRequest& iR)
							{ interruptSystem () -> removeInterruptRequest (iR); }

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
		  * INTERRUPTS	= InfoStructure: Information about the interrupts of the CPU
		  * ARCHITECURE = InfoStructure: Architecture info. \n
		  *	REGS		= InfoStructure: Registers info. \n
		  * PC			= Attribute: Value of the program counter. \n
		  *	SR			= Attribute: Value of the status register. \n
		  * CLK			= Attribute: Value of the internal clock cycle (could be o if it is external). \n
		  * STATE		= Attribute: State of execution. \n
		  * CLKSTOP		= Attribute: Number of cycles pending in stop situation
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** Fix the limits to debug info. */
		void setDeepDebugLimits (const Address& d1, const Address& d2)
		{
			_debugLimitsInit = d1;
			_debugLimitsEnd = d2;
		}

		protected:
		// Internal methods to simplify the understanding of the code.
		// Most of them can be overloaded...take care!

		// Invoked from executeNextCycle
		/** The first instruction of the method is to free the buffered commands (if any). \n
			This method must answer true when that instruction can be run and false in other circunstance. */
		virtual bool unbufferCommands ()
							{ return (true); }

		// Invoked from executeNextInstruction
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
		/** To recognize the execution interrupt. \n 
		 	By default, it does nothing, but it can be overloaded. \n
			This method is invoked from the methods above. */
		virtual void aknowledgeInterrupt ()	{ }
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

		/** To create the interrupt system. \n
			It must be redefined depeing on the type of CPU. */
		virtual CPUInterruptSystem* createInterruptSystem () const = 0;

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStopRequest () const; // tC = type of stop cycles, nC = number cycles to stop (== -1 forever)
		void debugInterruptRequest (const CPUInterruptRequest& iR) const;
		void debugLastExecutionData () const; // Using information in _lastInstruction...
		void debugStopSituation () const; // Using information in CPU abour stop situation (@see below)
		void debugAlreadyStopped () const; // That situation is not ususal...
		void debugInterruptLaunched () const; // Using the information in _currentInterrupt...
		void debugInterruptFails () const; // Using the information in _currentInterrupt...
		void debugInterruptRequestNotAllowed (const CPUInterruptRequest& iR) const;
		void debugInterruptRequestToWait (const CPUInterruptRequest& iR) const;
		void debugInstructionFails () const; // Using the information in _currentInstruction...
		void debugInstructionExecuted (const std::string& sdd) const; // ...plus then info in _currentInstruction...
		void debugInstructionWaiting () const; // Using the information in _currentInstruction...
		void debugInstructionNoExists (unsigned int nI);
		void debugHookInfo (CPUHook* hk);
		// -----

		protected:
		const CPUArchitecture _architecture = 
			CPUArchitecture (2 /** 2 bytes arch. */, 1 /** 1 byte for instruction. */); // Adjusted at construction level
		Registers _registers;
		const Instructions _instructions = { }; // Adjusted at construction level
		ProgramCounter _programCounter;
		StatusRegister _statusRegister;
		Memory* _memory; // A reference...
		CPUHooks _hooks;
		/** The Interrupt System. 
			Defined as mutable because it could be created in a const method. */
		mutable CPUInterruptSystem* _interruptSystem;

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

		/** Limits in the memory to debug. */
		Address _debugLimitsInit, _debugLimitsEnd;

		// Implementation
		/** Just what was the error in the execution if any. */
		unsigned int _error;

		// When things are executed by cycle...
		// In full instructions this is also used to avoid more interim variables, 
		// but makes no much more sense!
		/** The request under execution. */
		CPUInterruptRequest _currentInterruptRequest;
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
		StopStatusData _stopStatusData;

		/** The instructions will be moved into an array at construction time,
			to speed up their access in the executeNextInstruction method. */
		ListOfInstructions _rowInstructions;
	};

	// ---
	inline void CPU::StopStatusData::reset ()
	{ 
		reset (0, 0, 0, 0);  

		_cyclesLastInstructionOverlappedStopRequest =
		_cyclesLastInstructionExecutedStopRequest = 0;
	}

	// ---
	inline void CPU::StopStatusData::initialize 
		(unsigned int tC, unsigned int cC, int nC, unsigned int c, unsigned int nLI)
	{
		reset (tC, cC, nC, c);

		calcOverlapInfo (nLI);
	}

	// ---
	inline void CPU::StopStatusData::reset (unsigned tC, unsigned int cC, int nC, unsigned int c)
	{
		_typeCyclesStopped = tC;
		_cyclesAtStop = cC;
		// The values allowed are either positive or -1 meaning forever!
		_cyclesStopped = (nC < 0) ? -1 : nC; 
		_cyclesAtCPU = c;

		// Notice that the overlap info is not updated...

		_counterCyclesStopped = 0;
	}

	// ---
	inline void CPU::StopStatusData::calcOverlapInfo (unsigned int nLI)
	{

		// Actualize the info related with the overlapping...
		// c should be always bigger (or equal) that cC. If it wasn't no overlap happened!
		if (_cyclesAtCPU < _cyclesAtStop)
		{
			_cyclesLastInstructionOverlappedStopRequest = 0; // No overlapped could happen ener...
			_cyclesLastInstructionExecutedStopRequest = nLI; // ...so the last instruction was exected fully!
		}
		else
		{
			_cyclesLastInstructionOverlappedStopRequest = _cyclesAtCPU - _cyclesAtStop;
			_cyclesLastInstructionExecutedStopRequest = 
				(nLI >= _cyclesLastInstructionOverlappedStopRequest) 
					? nLI - _cyclesLastInstructionOverlappedStopRequest
					: 0; // If this situation an error has happened.... 
		}
	}

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
	inline CPUHook* CPU::executeHookIfAny ()
	{
		CPUHook* result = nullptr;

		CPUHooks::const_iterator i = 
			_hooks.find (_programCounter.internalRepresentation ());
		if (i != _hooks.end ())
			(result = (*i).second) -> hook (this);

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/

