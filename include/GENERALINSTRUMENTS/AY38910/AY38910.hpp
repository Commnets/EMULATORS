/** \ingroup GENERALINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: AY38910.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/04/2025 \n
 *	Description: This class manages different I/O Ports that are connected to Keyboard, Casette and Slot selection.
 *				 So this is a very important processor within any GENERALINSTRUMENTS computer.
 *	Versions: 1.0 Initial
 *  Based on: https://theoddys.com/acorn/semiconductor_datasheets/P8255A.pdf
 *			  https://www.angelfire.com/art2/unicorndreams/msx/RR-PPI.html
 */

#ifndef __GENERALINSTRUMENTS_AY38910__
#define __GENERALINSTRUMENTS_AY38910__

#include <CORE/incs.hpp>
#include <GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp>

namespace GENERALINSTRUMENTS
{
	class AY38910 final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 100;

		AY38910 (AY38910Registers* reg);

		~AY38910 ();

		// The access to the chip can be throught out the pins connected to the outside...
		// ...What this actions do, will depend on the internal situation of the chip
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (_AY38910Registers -> readRegister (rId)); }
		MCHEmul::UByte peekRegister (unsigned char rId) const // Same that previous but not changind the content...
							{ return (_AY38910Registers-> peekRegister (rId)); }
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ _AY38910Registers -> setRegister (rId, v); }

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
		void debugAY38910Cycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		private:
		/** A reference to the 8255 PPI Registers. */
		AY38910Registers* _AY38910Registers;

		/** The number of cycles the CPU was executed once the simulated method finishes. */
		unsigned int _lastCPUCycles;

		// Implementation
		/** When the Registers are created internally because they will be accesed always through out the chip
			and this one froma port and not like a memory address (that belongs to the computer) this
			variable is set and must be deleted. */
		AY38910Registers* _internalRegisters;

	};
}

#endif
  
// End of the file
/*@}*/