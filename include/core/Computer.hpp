/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 03/04/2021 \n
 *	Description: Groups many elements to define a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __CPUEMUL_COMPUTER__
#define __CPUEMUL_COMPUTER__

#include <assert.h>
#include <ostream>

#include <global.hpp>
#include <core/CPU.hpp>
#include <core/Chip.hpp>
#include <core/Memory.hpp>
#include <core/Register.hpp>
#include <language/Instruction.hpp>

namespace MCHEmul
{
	/** The computer links many different elements. */
	class Computer
	{
		public:
		Computer () = delete;

		/** The computer owns the different elements. */
		Computer (CPU* cpu, const Chips& c, Memory* m, const Attributes& attrs = { });

		Computer (const Computer&) = delete;

		virtual ~Computer (); 

		Computer& operator = (const Computer&) = delete;

		constexpr const CPU* cpu () const
							{ return (_cpu); }
		CPU* cpu ()
							{ return (_cpu); }

		constexpr const Chips& chips () const
							{ return (_chips); }
		bool existsChip (int id) const
							{ return (_chips.find (id) != _chips.end ()); }
		constexpr const Chip* chip (int id) const
							{ return (existsChip (id) ? (*_chips.find (id)).second : ChipNotValid); }
		Chip* chip (int id)
							{ return (existsChip (id) ? (*_chips.find (id)).second : TrashChip); }

		constexpr const Memory* memory () const
							{ return (_memory); }
		Memory* memory ()
							{ return (_memory); }

		constexpr const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/** To initialize the Computer, when the power is set up. \n 
			It could be defined per chip. By default it initialize the chips. \n
			Returns true, when verything was ok, and false in any other circusntance. */
		virtual bool initialize ();

		/** To simulate how the computer works. Can be overloaded. */
		virtual bool run ();

		/** To get the last error happend (after initialize or simulate methods). */
		constexpr unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const Computer& c);

		protected:
		void setExit (bool e)
							{ _exit = e; }

		protected:
		CPU* _cpu;
		Chips _chips; 
		Memory* _memory;
		const Attributes _attributes = { }; // Maybe modified at construction level

		/** It is useful to track how the program is working. */
		bool _debugMode;
		/** Used to finish the main loop. */
		bool _exit;

		// Implementation
		unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/
