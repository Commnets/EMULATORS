/** \ingroup CPU */
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
#include <CORE/Memory.hpp>
#include <CORE/Register.hpp>
#include <CORE/Instruction.hpp>
#include <CORE/IO.hpp>
#include <CORE/Screen.hpp>
#include <CORE/OSIO.hpp>

namespace MCHEmul
{
	/** The computer links many different elements. \n 
		The computer owns all the elements linked. */
	class Computer
	{
		public:
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

		const Screen* screen () const
							{ return (_screen); }
		Screen* screen ()
							{ return (_screen); }
		const InputOSSystem* inputOSSystem () const
							{ return (_inputOSSystem); }
		InputOSSystem* inputOSSytem ()
							{ return (_inputOSSystem); }

		unsigned int specyclesPerSecond () const
							{ return (_cyclesPerSecond); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		// To load data files...
		/** Two types of files: \n
		  * A file with the x initial bytes represeting an address (depending on the architecture of the cpu),
		  * Or a simpl data file, but the address where to load rom has to be passed as parameter! */
		bool load (const std::string& fN)
							{ return (memory () -> load (fN)); }
		bool loadInto (const std::string& fN, const Address& a)
							{ return (memory () -> loadInto (fN, a)); }
		bool loadInto (const std::string& fN)
							{ return (memory () -> loadInto (fN)); }

		/** To initialize the Computer, when the "power is set up". \n 
			It could be defined per computer. By default it initializes the chips. \n
			Returns true, when verything was ok, and false in any other circusntance. \n
			The last error happend could then be checked in _lastEerror. */
		virtual bool initialize ();

		/** To simulate how the computer works. 
			Returns true when finalizing the run ok, and false with error. \n
			The parameter is the log level. \n
			This method used the other tow defined behind and it can be simulated from outside. */
		bool run (unsigned lL);
		
		/** Execute one computer cycle (cpu + chips). 
			Returns true when ok, and false when no ok. */
		bool runComputerCycle (unsigned int lL);
		/** Execute the IO Cycle.
			Returns true when ok, and false when no ok. */
		bool runIOCycle (unsigned int lL);

		/** To know the value of the exit. */
		bool exit () const
							{ return (_exit); }
		/** Set the exit variable from the outside. */
		void setExit (bool e)
							{ _exit = e; }

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const Computer& c);

		protected:
		CPU* _cpu;
		Chips _chips; 
		Memory* _memory;
		IODevices _devices;
		unsigned int _cyclesPerSecond;
		const Attributes _attributes = { }; // Maybe modified at construction level

		/** Used to to indicate the execution must finishes.
			There could have been an error or not. */
		bool _exit;

		// Implementation
		unsigned int _lastError;
		Screen* _screen;
		InputOSSystem* _inputOSSystem;
	};
}

#endif
  
// End of the file
/*@}*/
