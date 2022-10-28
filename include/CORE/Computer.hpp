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

		Computer () = delete;

		/** The computer owns the different elements.
			The devices mandatory are the screen and the InputOSDevice. 
			This is verified at construction level. \n
			@param	spd		the number of cycles per second of the clock. */
		Computer (CPU* cpu, const Chips& c, Memory* m, const IODevices& d, unsigned int cs, const Attributes& attrs = { });

		Computer (const Computer&) = delete;

		virtual ~Computer (); 

		Computer& operator = (const Computer&) = delete;

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
		virtual bool connect (IOPeripheral* p, IODevice* d) = 0;

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

		// To load data files...
		bool loadInto (const std::string& fN, const Address& a)
							{ return (memory () -> loadInto (fN, a)); }

		/** To initialize the Computer, when the "power is set up". \n 
			It could be defined per computer. By default it initializes the chips. \n
			Returns true, when verything was ok, and false in any other circusntance. \n
			The last error happend could then be checked in _lastEerror. */
		virtual bool initialize ();

		/** To simulate how the computer works. 
			Returns true when finalizing the run ok, and false with error. \n
			The parameter is the log level. \n
			This method used the other tow defined behind and it can be simulated from outside. */
		bool run ();
		
		/** To indicate that the cpu clock status. */
		void startsComputerClock ()
							{ _clock.start (_cpu -> clockCycles ()); }
		/** Execute one computer cycle (cpu + chips). */
		bool runComputerCycle (unsigned int a = 0 /** Meaning no action. */);
		/** Execute the IO Cycle.
			Returns true when ok, and false when no ok. */
		bool runIOCycle ();
		/** To indicate that the cycle finishes. */
		void finishCycle ()
							{ _clock.waitFor (_cpu -> clockCycles ()); }

		bool exit () const
							{ return (_exit); }
		void setExit (bool e)
							{ _exit = e; }

		unsigned int debugLevel () const
							{ return (_debugLevel); }
		void setDebugLevel (unsigned int dL)
							{ _debugLevel = dL; }

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

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
							{ for (const auto i : at) removeAction (i.first); }
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
		/** Used internally to align the speed to the microprocessor 
			to the speed of the code in this machine. */
		class Clock final
		{
			public:
			Clock () = delete;

			Clock (unsigned int cS)
				: _cyclesPerSecond (cS),
				  _realCyclesPerSecond (0),
				  _initialClockCycles (0), _iClock ()
							{ assert (_cyclesPerSecond > 0); }

			Clock (const Clock&) = default;

			Clock& operator = (const Clock&) = default;

			unsigned int cyclesPerSecond () const
							{ return (_cyclesPerSecond); }
			unsigned int realCyclesPerSecond () const
							{ return (_realCyclesPerSecond); }

			void start (unsigned int cC);
			void waitFor (unsigned int cC);

			private:
			unsigned int _cyclesPerSecond;
			unsigned _realCyclesPerSecond;

			// Implementation
			unsigned int _initialClockCycles;
			std::chrono::time_point <std::chrono::steady_clock> _iClock;
		};
			
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

		/** The level of the debug info. */
		unsigned int _debugLevel;

		// Implementation
		mutable unsigned int _lastError;
		Screen* _screen;
		InputOSSystem* _inputOSSystem;
		GraphicalChip* _graphicalChip;
		Clock _clock; // To maintain the sped of the compute...
		unsigned int _lastAction;
	};
}

#endif
  
// End of the file
/*@}*/
