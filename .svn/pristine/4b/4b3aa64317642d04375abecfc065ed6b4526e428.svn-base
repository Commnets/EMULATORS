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
			This is verified at construction level. */
		Computer (CPU* cpu, const Chips& c, Memory* m, const IODevices& d, const Attributes& attrs = { });

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

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/** To initialize the Computer, when the "power is set up". \n 
			It could be defined per computer. By default it initializes the chips. \n
			Returns true, when verything was ok, and false in any other circusntance. \n
			The last error happend could then be checked in _lastEerror. */
		virtual bool initialize ();

		/** To simulate how the computer works. 
			cE indicates whether the machine is or not controlled from outside. */
		bool run (bool cE = false);
		/** To simulate how the computer works from an speecific memory location. */
		bool runFrom (const Address& a, bool cE = false);
		/** Just to start a program from an address. No initialization is done. */
		bool startProgramAt (const Address& a, bool cE = false);

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const Computer& c);

		protected:
		void setExit (bool e)
							{ _exit = e; }

		// Implementation (used from the several runs)
		/** It can be overloaded later. */
		virtual bool runImpl (bool cE);

		protected:
		CPU* _cpu;
		Chips _chips; 
		Memory* _memory;
		IODevices _devices;
		const Attributes _attributes = { }; // Maybe modified at construction level

		/** It is useful to track how the program is working from an external location. */
		bool _controlFromOutside;
		/** Used to finish the main loop. */
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
