/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Groups the different elements needed to define a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHMUL_COMPUTER__
#define __MCHMUL_COMPUTER__

#include <CORE/global.hpp>
#include <CORE/Clock.hpp>
#include <CORE/CPU.hpp>
#include <CORE/Chip.hpp>
#include <CORE/GraphicalChip.hpp>
#include <CORE/Memory.hpp>
#include <CORE/Register.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/IO.hpp>
#include <CORE/Screen.hpp>
#include <CORE/OSIO.hpp>
#include <chrono>

namespace MCHEmul
{
	/** The computer links many different elements: \n
		CPU, Memory, Specialized Chips and IODevices. */
	class Computer : public InfoClass
	{
		public:
		/** Types of actions than can be executed on a compter. 
			They can be overloaded in extensions of the computer. */
		static const unsigned int _ACTIONNOTHING = 0; // Meaning working normally...
		static const unsigned int _ACTIONSTOP = 1; // Meaning to stop the execution of the program...
		static const unsigned int _ACTIONCONTINUE = 2; // Menaing to continue the execution until any other stop action is found...
		static const unsigned int _ACTIONNEXT = 3; // Meaning to execute just only the next instruction...

		using MapOfActions = std::map <MCHEmul::Address, unsigned int>;

		/** The types of status that a computer can have. */
		static const unsigned int _STATUSRUNNING = 0;
		static const unsigned int _STATUSSTOPPED = 1;

		/** The computer owns the different elements.
			The devices mandatory are the screen and the InputOSDevice. 
			This is verified at construction level. \n
			@param cpu		A reference to the cpu used by the computer.
			@param c		The list of the chips of the computer.
			@param m		The memory accesible to the computer. \n
							All blocks. Whether they are or not active is something to configure in the Memory itself. 
			@param d		The devices connected to the computer. \n
							There are two that are always mandatory: An screen and a Keyboard. \n
							The chips and the devices are connected at construction time too, 
							invoking the method "linkChips" of every Device.
			@param cs		the number of cycles per second of the clock. 
			@param attrs	Optional attributes defining the computer. */
		Computer (CPU* cpu, const Chips& c, Memory* m, const IODevices& d, unsigned int cs, const Attributes& attrs = { });

		Computer (const Computer&) = delete;

		Computer& operator = (const Computer&) = delete;

		virtual ~Computer (); 

		Computer (Computer&&) = delete;

		Computer& operator = (Computer&&) = delete;

		const CPU* cpu () const
							{ return (_cpu); }
		CPU* cpu ()
							{ return (_cpu); }

		const Chips& chips () const
							{ return (_chips); }
		bool existsChip (int id) const
							{ return (_chips.find (id) != _chips.end ()); }
		const Chip* chip (int id) const
							{ return (existsChip (id) ? (*_chips.find (id)).second : ChipNotValid); }
		Chip* chip (int id)
							{ return (existsChip (id) ? (*_chips.find (id)).second : TrashChip); }
		/** This is specifically choosen at construction time. */
		const GraphicalChip* graphicalChip () const
							{ return (_graphicalChip); }
		GraphicalChip* graphicalChip ()
							{ return (_graphicalChip); }

		const Memory* memory () const
							{ return (_memory); }
		Memory* memory ()
							{ return (_memory); }

		const IODevices& devices () const
							{ return (_devices); }
		bool existsDevice (int id) const
							{ return (_devices.find (id) != _devices.end ()); }
		const IODevice* device (int id) const
							{ return (existsDevice (id) ? (*_devices.find (id)).second : nullptr); }
		IODevice* device (int id)
							{ return (existsDevice (id) ? (*_devices.find (id)).second : nullptr); }

		/** To connect a peripheral to a device on the computer. \n
			It returns true when it is ok, and false in other circunstance. 
			The way the peripheral is connected will depend on the computer, on the device and on the peripheral itself. */
		virtual bool connect (IOPeripheral* p, IODevice* d)
							{ return ((p == nullptr || d == nullptr) ? false : d -> connectPeripheral (p)); }

		const Screen* screen () const
							{ return (_screen); }
		Screen* screen ()
							{ return (_screen); }
		const InputOSSystem* inputOSSystem () const
							{ return (_inputOSSystem); }
		InputOSSystem* inputOSSytem ()
							{ return (_inputOSSystem); }

		unsigned int cyclesPerSecond () const
							{ return (_clock.cyclesPerSecond ()); }
		unsigned int realCyclesPerSecond () const
							{ return (_clock.realCyclesPerSecond ()); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/** To load data files. */
		bool loadInto (const std::string& fN, const Address& a)
							{ return (memory () -> loadInto (fN, a)); }

		/** To save data files. */
		bool saveFrom (const std::string& fN, size_t nB, const Address& a)
							{ return (memory () -> saveFrom (fN, nB, a)); }

		/** To initialize the Computer, when the "power is set up". \n 
			It could be defined per computer. By default it initializes the chips. \n
			Returns true, when verything was ok, and false in any other circusntance. \n
			The last error happend could then be checked in _lastEerror. \n
			The parameter indicates wether the memory is or not initialized. */
		virtual bool initialize (bool iM = true);
		/** To restart the computer. \n
			It should take into account the "level". By default it will by like an initialization. \n 
			It should return true when everything is ok, and false in any other circunstance. \n
			By default the level 1 won't restart the memory,
			and any other value will restart the full computer (= initialize ()). */
		virtual bool restart ();

		/** To simulate how the computer works. 
			Returns true when finalizing the run ok, and false with error. \n
			The parameter is the log level. \n
			This method used the other tow defined behind and it can be simulated from outside. */
		bool run ();

		// Managing the clock...
		/** To indicate that the cpu clock status. */
		void startsComputerClock ()
							{ _clock.start (); }
		unsigned int clockCycles () const
							{ return (cpu () -> clockCycles ()); }
		unsigned int lastClockCycles () const
							{ return (cpu () -> lastClockCycles ()); }
		/** To count cycles. */
		void countClockCycles (unsigned int cC)
							{ _clock.countCycles (cC); }

		// Managing the cycles...
		/** Execute one computer cycle (cpu + chips). */
		bool runComputerCycle (unsigned int a = 0 /** Meaning no action. */);
		/** Execute the IO Cycle.
			Returns true when ok, and false when no ok. */
		bool runIOCycle ();

		bool exit () const
							{ return (_exit); }
		void setExit (bool e)
							{ _exit = e; }
		bool restartAfterExit () const
							{ return (_restartAfterExit); }
		unsigned int restartLevel () const
							{ return (_restartLevel); }
		void setRestartAfterExit (bool r, unsigned int l = 0 /** The meaning could be determine by the computer itself. */)
							{ _restartAfterExit = r; _restartLevel = l;  }

		unsigned int debugLevel () const
							{ return (_debugLevel); }
		void setDebugLevel (unsigned int dL)
							{ _debugLevel = dL; }

		void activateDeepDebug (const std::string& fN)
							{ cpu () -> activateDeepDebug (fN); }
		void desactivateDeepDebug ()
							{ cpu () -> desactivateDeepDebug (); }

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int error () const
							{ return (_error); }
		void resetErrors ()
							{ _error = _NOERROR; }

		/** To know whether the computer is running. 
			It can be overloaded later, but the default implementation asks by the status RUNNING. */
		virtual bool isRunning () const
							{ return (_status == _STATUSRUNNING); }

		// To manage actions at address level...
		/** To know the list of all actions defined. */
		const MapOfActions& actions () const
							{ return (_actionsAt); }
		/** To know the action at an specific address. */
		unsigned int action (const Address& a) const
							{ MapOfActions::const_iterator i = _actionsAt.find (a); 
							  return ((i == _actionsAt.end ()) ? _ACTIONNOTHING : (*i).second); }
		/** To manage them individually. */
		void addAction (const Address& at, unsigned int a);
		/** Adds a couple of them. */
		void addActions (const MapOfActions& at)
							{ for (const auto& i : at) _actionsAt [i.first] = i.second; }
		void removeAction (const Address& at);
		/** Remove a set of them. */
		void removeActions (const MapOfActions& at)
							{ for (const auto& i : at) removeAction (i.first); }
		/** All of them at the same time. */
		void removeAllActions () 
							{ _actionsAt = { }; }
		/** To remove just the ones blonging to a specific type.*/
		void removeAllActions (unsigned int a);
		/** Action pending to be executed in the next cycle. */
		void setActionForNextCycle (unsigned int a)
							{ _actionForNextCycle = a; }
		unsigned int lastAction () const
							{ return (_lastAction); }

		/**
		  *	The name of the fields are: \n
		  *	ATTRS		= InfoStructure: attributes defining the computer.
		  * CPU			= InfoStructure: Info about the CPU.
		  * CHIPS		= InfoStructure: Info about the Chips.
		  * DEVICES		= InfoStructure: Info about the Devices connected.
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		// To be used when e.g debugging...
		/** In the method runComputerCycle, before executing the cycle related to the computer (cpu + chips), this method is invoked. \n
			The parameters passed through are: "lA" is the last action executed (if any), 
			"at" is the action associated to the point where the program counter is now at, 
			and "a" is the action parameter received by the method itself. \n
			A potential "like a" parameter received through the communication system (if active and if any) is 
			also taken into account. However the parameter "a" received by the method has priority.
			With these three/four variables this method should do whatever is requires and decide whether execute the 
			cycle of the computer (true) or not (return false). \n
			It can be overloaded for specific pruposes. \n
			During the execution of the method the firt parameter (lastAction) can be modified. */
		virtual bool executeAction (unsigned int& lA, unsigned int at, unsigned int a);

		protected:
		CPU* _cpu;
		Chips _chips; 
		Memory* _memory;
		IODevices _devices;
		const Attributes _attributes = { }; // Maybe modified at construction level
		MapOfActions _actionsAt;
		unsigned int _status;
		unsigned int _actionForNextCycle; // The action to be executed in the next cycle...

		/** Used to to indicate the execution must finishes.
			There could have been an error or not. */
		bool _exit;
		/** Used to indicate whether the execution has to be restarted after exiting the main loop. 
			This variable can only be set from a command. */
		bool _restartAfterExit;
		/** There could be different types of restart: total, just memory, etc...
			The type of restarting has to be determine by the computer. 
			By default it will be in the same way that a full initialization. */
		unsigned int _restartLevel;

		/** The level of the debug info. */
		unsigned int _debugLevel;

		// Implementation
		mutable unsigned int _error;
		Screen* _screen;
		InputOSSystem* _inputOSSystem;
		GraphicalChip* _graphicalChip;
		Clock _clock; // To maintain the sped of the computer...
		unsigned int _lastAction;
	};
}

#endif
  
// End of the file
/*@}*/
