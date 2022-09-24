/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: Chip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: Defines common things to any chip.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CHIP__
#define __MCHEMUL_CHIP__

#include <global.hpp>
#include <core/Memory.hpp>

namespace MCHEmul
{
	class CPU;

	/** A chip is a specialized element within the computer (different that the CPU). \n
		All chips are set with the full memory accesibl when the computer is initialized,
		unless something specific is said initializing the chip itself!. */
	class Chip
	{
		public:
		Chip () = delete;

		Chip (int id, const Attributes& attrs = { })
			: _id (id), _memory (nullptr), _attributes (attrs), 
			  _lastError (_NOERROR) // Memory accessed can be null, take care...
							{ }

		Chip (const Chip&) = delete;

		Chip& operator = (const Chip&) = delete;

		/** The chip is not owner of the memory. */
		virtual ~Chip ()
							{ } 

		int id () const
							{ return (_id); }

		/** The memory the chip can access to. */
		void setMemoryRef (Memory* m)
							{ _memory = m; }
		const Memory* memoryRef () const
							{ return (_memory); }
		Memory* memoryRef ()
							{ return (_memory); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/** To initialize the chip, when "the power is set up". \n 
			It could be defined per chip. By default it does nothing. \n
			Returns true, when verything was ok, and false in any other circusntance. */
		virtual bool initialize ()
							{ return (true); }

		/** To simulate th behaviour of the chip. It has to be defined per chip. \n
			Returns true if everything was ok, and false in any other circunstance. \n 
			The last error could be recovered from the variable _lastError in that case. */
		virtual bool simulate (CPU*) = 0;

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const Chip& c);

		protected:
		const int _id = -1; // Modified at construction level
		Memory* _memory;
		const Attributes _attributes = { }; // Maybe modified at construction level

		// Implementation
		unsigned int _lastError;
	};

	using Chips = std::map <int, Chip*>;

	/** A no chip. */
	class NoChip final : public Chip
	{
		public:
		NoChip ()
			: Chip (0)
							{ }

		virtual bool simulate (CPU*) override
							{ return (true); }
	};

	/** Two chips used as an answer when the one requested doesn't exist. */
	static const Chip* ChipNotValid = new NoChip;
	static Chip* TrashChip = new NoChip;
}

#endif
  
// End of the file
/*@}*/

