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
	class Instruction;
	struct InstructionContextEventData;
	struct InterruptContextEventData;

	/** A chip is a specialized element within the computer (different that the CPU). \n
		All chips are set with the full memory accesible when the computer is initialized,
		unless something specific is said initializing the chip itself!. \n
		The chip is able to notify event to other element subscribed or receive event from tehm. */
	class Chip : public MotherboardElement, public Notifier, public DebugableClass
	{
		public:
		Chip (int id, const Attributes& attrs = { })
			: MotherboardElement (id, "Chip", attrs),
			  _memory (nullptr),
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

		// Related with the simulation
		/** Notifies the chip that an instruction has been selected but has not yet
			produced any side effect. \n
			The context contains non-owning pointers to the instruction, CPU and memory,
			and a copy of the instruction starting address. \n
			The event is processed synchronously and the context pointer must not be retained. */
		virtual void CPUAboutToExecute (const InstructionContextEventData*)
							{ /** Nothing is done by default. */ }
		/** Notifies the chip that an accepted interrupt is about to start its
			launch bus transaction. \n
			The context is processed synchronously and must not be retained. */
		virtual void CPUAboutToExecute (const InterruptContextEventData*)
							{ /** Nothing is done by default. */ }
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

		protected:
		Memory* _memory;

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

