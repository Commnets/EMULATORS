/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
#include <CORE/Bus.hpp>
#include <CORE/Wire.hpp>
#include <CORE/MBElement.hpp>
#include <CORE/Register.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/IO.hpp>
#include <CORE/Screen.hpp>
#include <CORE/Sound.hpp>
#include <CORE/OSIO.hpp>
#include <CORE/DebugFile.hpp>
#include <chrono>

namespace MCHEmul
{
	/** The computer links many different elements: \n
		CPU, Memory, Specialized Chips and IODevices. */
	class Computer : public InfoClass, public Notifier, public Observer
	{
		public:
		/** Types of actions than can be executed on a compter. 
			They can be overloaded in extensions of the computer. */
		static const unsigned int _ACTIONNOTHING = 0; // Meaning working normally...
		static const unsigned int _ACTIONSTOP = 1; // Meaning to stop the execution of the program...
		static const unsigned int _ACTIONCONTINUE = 2; // Menaing to continue the execution until any other stop action is found...
		static const unsigned int _ACTIONNEXT = 3; // Meaning to execute just only the next instruction...

		/** The types of status that a computer can have. */
		static const unsigned int _STATUSRUNNING = 0;
		static const unsigned int _STATUSSTOPPED = 1;

		/** The computer might execute different actions when reaching a specific position. */
		class Action
		{
			public:
			// No default constructors are needed...
			Action (unsigned int id)
				: _id (id)
							{ }

			unsigned int id () const
							{ return (_id); }

			virtual ~Action () 
							{ /** Just in case an extension is needed. */ }

			/** Returns true when the next CPU cycle has to be executed after this action,
				and false in any other circusntance. */
			virtual bool execute (Computer*) = 0;

			protected:
			unsigned int _id;
		};

		using Actions = std::vector <Action*>;

		/** Made up of a set of actions. */
		class CompositeAction : public Action
		{
			public:
			CompositeAction (unsigned int id, const Actions& a)
				: Action (id),
				  _actions (a)
							{ }

			/** It will return true when all elements return true, 
				and false in other circunstance. */
			virtual inline bool execute (Computer* c) override;

			protected:
			Actions _actions;
		};

		/** No action at all. 
			Usually it is not needed. It is defined just in case. */
		class NoAction final : public Action
		{
			public:
			NoAction ()
				: Action (Computer::_ACTIONNOTHING)
							{ }
			
			virtual inline bool execute (Computer*) override;
		};

		/** A clear and simple action is just to stop the execution. */
		class StopAction final : public Action
		{
			public:
			StopAction ()
				: Action (Computer::_ACTIONSTOP)
							{ }

			virtual inline bool execute (Computer* c) override;
		};

		/** In the opposite side, the action is to continue with the execution. */
		class ContinueAction final : public Action
		{
			public:
			ContinueAction ()
				: Action (Computer::_ACTIONCONTINUE)
							{ }

			virtual inline bool execute (Computer* c) override;
		};

		/** Move to the next instrction. */
		class NextCommandAction final : public Action
		{
			public:
			NextCommandAction ()
				: Action (Computer::_ACTIONNEXT)
							{ }

			virtual inline bool execute (Computer* c) override;
		};

		using TemplateOfActions = std::map <unsigned int, Action*>;
		using MapOfActions = std::map <MCHEmul::Address, unsigned int>;

		/** 
		  *	The computer owns the different elements.
		  *	The devices mandatory are the screen and the InputOSDevice. 
		  *	This is verified at construction level. \n
		  *	@param cpu		A reference to the cpu used by the computer.
		  *	@param c		The list of the chips of the computer.
		  *	@param m		The memory accesible to the computer. \n
		  *					All blocks. Whether they are or not active is something to configure in the Memory itself. 
		  *	@param d		The devices connected to the computer. \n
		  *					There are two that are always mandatory: An screen and a Keyboard. \n
		  *					The chips and the devices are connected at construction time too, 
		  *					invoking the method "linkChips" of every Device.
		  *	@param cs		the number of cycles per second of the clock.
		  *	@param bs		Buses being part of the motherboard of the computer.
		  *	@param ws		Wires being part of the motherboard of the computer.
		  *	@param attrs	Optional attributes defining the computer. 
		  *	@param sL		Number of stabilization loops before starting to execute CPU instructions.
		  * CPU, Memory, Chips and Devices are Motherboard Elements.
		  * All of them are, somehow and depending on the computer, linked among them. \n
		  * They can be linked directly or through the buses and wires of the computer. \n
		  * The constructor invokes the method linkToChips to any IODevice present in the computer,
		  * passing the list of Chips defined for the computer. \n
		  * This is the most common way to connect chips and devices. \n
		  * However they can also be connectes through the buses and wires if defined.
		  * So the constructor also invokes the method connectElements to any bus and wire defined for the system.
		  * Use one, the other or both. It is your choice.
		  */
		Computer (CPU* cpu,
				  const Chips& c, 
				  Memory* m, 
				  const IODevices& d,
				  unsigned int cs,
				  const Buses& bs = { },
				  const Wires& ws = { },
				  const Attributes& attrs = { }, unsigned short sL = 10);

		Computer (const Computer&) = delete;

		Computer& operator = (const Computer&) = delete;

		virtual ~Computer () override; 

		Computer (Computer&&) = delete;

		Computer& operator = (Computer&&) = delete;

		const Clock& clock () const
							{ return (_clock); }
		Clock& clock ()
							{ return (_clock); }

		const CPU* cpu () const
							{ return (_cpu); }
		CPU* cpu ()
							{ return (_cpu); }

		// Managing the chips...
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

		// Managing the devices...
		const IODevices& devices () const
							{ return (_devices); }
		bool existsDevice (int id) const
							{ return (_devices.find (id) != _devices.end ()); }
		const IODevice* device (int id) const
							{ return (existsDevice (id) ? (*_devices.find (id)).second : nullptr); }
		IODevice* device (int id)
							{ return (existsDevice (id) ? (*_devices.find (id)).second : nullptr); }

		// Managing the buses...
		const Buses& buses () const
							{ return (_buses); }
		bool existsBus (int id) const
							{ return (_buses.find (id) != _buses.end ()); }
		const Bus* bus (int id) const
							{ return (existsBus (id) ? (*_buses.find (id)).second : nullptr); }
		Bus* bus (int id)
							{ return (existsBus (id) ? (*_buses.find (id)).second : nullptr); }

		// Managing the wires...
		const Wires& wires () const
							{ return (_wires); }
		bool existsWire (int id) const
							{ return (_wires.find (id) != _wires.end ()); }
		const Wire* wire (int id) const
							{ return (existsWire (id) ? (*_wires.find (id)).second : nullptr); }
		Wire* wire (int id)
							{ return (existsWire (id) ? (*_wires.find (id)).second : nullptr); }

		// Managing the peripherals
		IOPeripherals peripherals () const;
		bool existsPeripheral (int id) const
							{ IOPeripherals prh = std::move (peripherals ()); return (prh.find (id) != prh.end ()); }
		const IOPeripheral* peripheral (int id) const;
		IOPeripheral* peripheral (int id)
							{ return ((IOPeripheral*) (((const Computer*) this) -> peripheral (id))); }
		/** To connect a peripheral to a device on the computer. \n
			It returns true when it is ok, and false in other circunstance. ºn
			The way the peripheral is connected will depend on the computer, on the device and on the peripheral itself. */
		bool connectPeripheral (IOPeripheral* p, IODevice* d)
							{ assert (p != nullptr && d != nullptr); return (d -> connectPeripheral (p)); }
		/** To connect a peripheral in the right device. 
			Returns true whether the device to connect the peripheral was found and the connection was right, 
			and false in any other circunstance. */
		bool connectPeripheral (IOPeripheral* p);
		/** To disconnect a peripheral from a device.
			If the device doesn't manage the peripheral or the peripheral is not found, nothing happens. */
		bool disconnectPeripheral (int id, IODevice* d)
							{ assert (d != nullptr); return (d -> disconnectPeripheral (id)); }
		/** To disconnect a peripheral from the right device. \n
			It will return false if an error in the deconnection happened. */
		bool disconnectPeripheral (int id);
		/** To disconnect al peripherals.
			It will return false if an error in the deconnection of at leat one peripheral happened. */
		bool disconnectAllPeripherals ();
		/** To connect data to a peripheral of the computer. \n
			It returns true when it is ok, and false in other circunstance. */
		bool connectPeripheralData (IOPeripheral* p, FileData* dt)
							{ assert (p != nullptr && dt != nullptr); return (p -> connectData (dt)); }
	
		const Screen* screen () const
							{ return (_screen); }
		Screen* screen ()
							{ return (_screen); }
		const InputOSSystem* inputOSSystem () const
							{ return (_inputOSSystem); }
		InputOSSystem* inputOSSytem ()
							{ return (_inputOSSystem); }
		const SoundSystem* sound () const // Can be nullptr...
							{ return (_sound); }
		SoundSystem* sound () // Can be nullptr...
							{ return (_sound); }

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
			By default the level 0 just invoke the CPU restart method, 
			level 1 call initialize (Computer) but it won't restart the memory,
			and any other value will initialize including the memory. \n
			Return true when everything ok and false in other case. */
		virtual bool restart ();

		/** To simulate how the computer works. 
			Returns true when finalizing the run ok, and false with error. \n
			The parameter is the log level. \n
			This method used the other tow defined behind and it can be simulated from outside. */
		bool run ();

		// Managing the clock to accelerate or desacelerate the execution...
		/** To indicate that the speed clock status. */
		void startsSpeedClock ()
							{ _clock.start (); }

		// Managing the cycles...
		/** 
		  * Execute one computer cycle (cpu + chips). \n
		  * This method is the very core one. \n
		  * This is where the clock is managed. \n
		  * The number of cycles to wait (to maintain the speed)
		  * will depend on whether the clock of the CPU is either internal or external. \n
		  * If it is internal, the number of cycles of the last instruction will be taken into account,
		  * If it is external, only 1 cycle will be taken into consideration.
		  */
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

		// Important when restarting...
		/** To set a restart address different tha the estandard defined by the CPU. */
		void setStartingAddress (const Address& sA)
							{ _startAtSpecificAddress = true; _startingAddress = sA; }
		/** To set the restart stopped. \n
			This method can be invoked after computer initialization, so the act*/
		void setNextStartStopped (bool s)
							{ _startStopped = s; }

		// Managing the deep debugging...
		/** To knwow whether the debug file is or not active. */
		bool deepDebug () const
							{ return (_deepDebug.active ()); }
		/** To activate and desactive the deep debug to a file. \n
			Returns true when ok, and false when not possible. \n
			When activated, the only mandatory parameter needed is the 
			name of the file where to store the info. \n
			The deep debug is activated at CPU level minimum if activated, 
			but optionally a list of chips id can be passed as parameter (a single parameter = -1 will mean all). \n
			Also a list of devices where to trace can be passed as parameter (a single parameter = -1 will al so mean all). \n
			Also a list of memory physical storages to trace if any. The standard physical storage don't trace anything.
			The user should redefine their own ones, to trace what wanted. \n
			Finally another optional parameter is whether the info generated has to be added to the file if it exists
			or a new one should be created. */
		bool activateDeepDebug (const std::string& fN,
			bool cpud = true, // Meaning that the cpu info must be activated...
			const std::vector <int>& cId = { -1 }, // Menaing all chips included...
			const std::vector <int>& iId = { -1 }, // Meaning all devices included...
			const std::vector <int>& mId = { -1 }, // Meaning all physical storages included...
			bool a = false /** meaning not adding the info at the end, but new file. */);
		bool desactivateDeepDebug ();
		const DebugFile* deepDebugFile () const
							{ return (&_deepDebug); }
		DebugFile* deepDebugFile ()
							{ return (&_deepDebug); }

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
		/** To run something specific in a computer cycle. \n 
			Very rare, but just to allow an exit way per computer cycle. 
			By default it does nothing. */
		virtual void specificComputerCycle ()
							{ }

		/** In the method runComputerCycle, 
			before executing the cycle related to the computer (cpu + chips), this method is invoked
			just in case an action should be executed. \n
			The method receive the id of an action to be executed. \n
			The default behaviour:
			A parameter received through the communication system (if active and if any) is 
			taken into account. However the " received by the method has priority over it.
			And if the parameter received is _ACTIONNOTHING and also the one received throught comms
			it will be executed the one defined at PC (if any). \n
			It can be overloaded for specific purposes. \n
			During the execution of the method the firt parameter (lastAction) can be modified. */
		virtual bool executeActionAtPC (unsigned int a);

		protected:
		CPU* _cpu;
		Chips _chips; 
		Memory* _memory;
		IODevices _devices;
		Buses _buses;
		Wires _wires;
		const Attributes _attributes = { }; // Maybe modified at construction level
		TemplateOfActions _templateActions; // The templates are used to acclerate what to do!
		MapOfActions _actionsAt;
		unsigned int _status;
		unsigned int _actionForNextCycle; // The action to be executed in the next cycle...
		DebugFile _deepDebug; // When the deep debug is requested...

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

		/** Sometimes, when the computer starts a stabilization is required in the devices and chips.
			It implies that no transaction is executed in the meantime for a couple of loops.
			The number of loops to wait is defined at construction time. */
		unsigned short _stabilizationLoops;

		/** When the computer has to start a specific address 
			it has been usually defined when the emulation starts. */
		bool _startAtSpecificAddress;
		Address _startingAddress;
		/** When the computer starts stopped. */
		bool _startStopped;

		// Implementation
		mutable unsigned int _error;
		Screen* _screen; // it cann't be nullptr ever...
		SoundSystem* _sound; // it can be nullptr...
		InputOSSystem* _inputOSSystem;
		GraphicalChip* _graphicalChip;
		// The following arrays are used just only within the methods runXXX to speed up execution!
		// To go through out and array of elements if always quicker than to go through a map of those
		// even when the number of total elements is small as it is usually the case in a computer simulation!
		Chip** _plainChips; // The same content than _chips but in an array of those (quicker)...
		IODevice** _plainDevices; // The same content than _devices but in array of those (quicker)...
		Clock _clock; // To maintain the speed of the computer...
		unsigned int _lastAction;
		mutable bool _stabilized;
		mutable unsigned short _currentStabilizationLoops;
		std::vector <Action*> _templateListActions; // The same the template of actions but in a vector to speed up access...
	};

	// ---
	inline bool Computer::CompositeAction::execute (Computer* c)
	{
		bool result = true;
		for (auto& i : _actions)
			result &= i -> execute (c);
		return (result);
	}

	// ---
	inline bool Computer::NoAction::execute (Computer* c)
	{
		// What to do now will depend on was the last action was...
		switch (c -> _lastAction)
		{
			case Computer::_ACTIONNOTHING:
				// The status doesn't change as there is nothing else to do...
				break;

			case Computer::_ACTIONSTOP:
			case Computer::_ACTIONNEXT:
				c -> _status = Computer::_STATUSSTOPPED;
				break;

			case Computer::_ACTIONCONTINUE:
				c -> _status = Computer::_STATUSRUNNING;
				break;
		}

		return (c -> _status == Computer::_STATUSRUNNING);
	}

	// ---
	inline bool Computer::StopAction::execute (Computer* c)
	{
		c -> _status = Computer::_STATUSSTOPPED;

		return (false);
	}

	// ---
	inline bool Computer::ContinueAction::execute (Computer* c)
	{
		c -> _status = Computer::_STATUSRUNNING;

		return (true);
	}

	// ---
	inline bool Computer::NextCommandAction::execute (Computer* c)
	{
		c -> _status = Computer::_STATUSRUNNING;

		// Flush the memory, if needed...
		c -> memory () -> configuration ().executeMemorySetCommandsBuffered ();

		return (true);
	}
}

#endif
  
// End of the file
/*@}*/
