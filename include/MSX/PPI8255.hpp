/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: PPI8255.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/04/2025 \n
 *	Description: This class manages different I/O Ports that are connected to Keyboard, Casette and Slot selection.
 *				 So this is a very important processor within any MSX computer.
 *	Versions: 1.0 Initial
 *  Based on: https://theoddys.com/acorn/semiconductor_datasheets/P8255A.pdf
 *			  https://www.angelfire.com/art2/unicorndreams/msx/RR-PPI.html
 */

#ifndef __MSX_PPI8255__
#define __MSX_PPI8255__

#include <CORE/incs.hpp>
#include <MSX/PPI8255Registers.hpp>

namespace MSX
{
	class PPI8255 final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 100;

		PPI8255 (PPI8255Registers* reg);

		~PPI8255 ();

		// The access to the chip can be throught out the pins connected to the outside...
		// ...What this actions do, will depend on the internal situation of the chip
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (_PPI8255Registers -> readRegister (rId)); }
		MCHEmul::UByte peekRegister (unsigned char rId) const // Same that previous but not changind the content...
							{ return (_PPI8255Registers-> peekRegister (rId)); }
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ _PPI8255Registers -> setRegister (rId, v); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones from the parent class +:
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugPPI8255Cycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		private:
		/** A reference to the 8255 PPI Registers. */
		PPI8255Registers* _PPI8255Registers;

		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;

		// Implementation
		/** When the Registers are created internally because they will be accesed always through out the chip
			and this one froma port and not like a memory address (that belongs to the computer) this
			variable is set and must be deleted. */
		PPI8255Registers* _internalRegisters;

	};
}

#endif
  
// End of the file
/*@}*/