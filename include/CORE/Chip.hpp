/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Chip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: Defines common things to any chip.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CHIP__
#define __MCHEMUL_CHIP__

#include <CORE/global.hpp>
#include <CORE/MBElement.hpp>
#include <CORE/Memory.hpp>
#include <CORE/DebugFile.hpp>

namespace MCHEmul
{
	class CPU;

	/** A chip is a specialized element within the computer (different that the CPU). \n
		All chips are set with the full memory accesible when the computer is initialized,
		unless something specific is said initializing the chip itself!. \n
		The chip is able to notify event to other element subscribed or receive event from tehm. */
	class Chip : public MotherboardElement, public Notifier
	{
		public:
		Chip (int id, const Attributes& attrs = { })
			: MotherboardElement (id, "Chip", attrs),
			  _memory (nullptr),
			  _deepDebugFile (nullptr),
			  _error (_NOERROR) // Memory accessed can be null, take care...
							{ }

		Chip (const Chip&) = delete;

		Chip& operator = (const Chip&) = delete;

		/** The chip is not owner of the memory. */
		virtual ~Chip () override
							{ } 

		Chip (Chip&&) = delete;

		Chip& operator = (Chip&&) = delete;

		/** The memory that the chip can access to. \n
			This method is invoked from the Computer constructor. */
		void setMemoryRef (Memory* m)
							{ _memory = m; }
		const Memory* memoryRef () const
							{ return (_memory); }
		Memory* memoryRef ()
							{ return (_memory); }

		/** To simulate th behaviour of the chip. It has to be defined per chip. \n
			Returns true if everything was ok, and false in any other circunstance. \n 
			The last error could be recovered from the variable _error in that case. \n
			The clock cyclees is added. */
		virtual bool simulate (CPU*) = 0;

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int error () const
							{ return (_error); }
		void resetErrors ()
							{ _error = _NOERROR; }

		/**
		  *	The name of the fields are: \n
		  *	ID		= Attribute: Id of the Chip. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Chip. \n
		  * MEMORY	= InfoStructure: Memory info addrressed by the Chip.
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** Manages the deep debug file. \n
			Take care it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF)
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		Memory* _memory;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		mutable unsigned int _error;
	};

	using Chips = std::map <int, Chip*>;

	/** A no chip. */
	class NoChip final : public Chip
	{
		public:
		NoChip ()
			: Chip (0)
							{ }

		virtual bool initialize () override
							{ return (true); }

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

