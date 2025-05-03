/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: PPI8255Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/04/2024 \n
 *	Description: The registers of the PPI8255 Chip.
 *	Versions: 1.0 Initial
 *  Based on: https://theoddys.com/acorn/semiconductor_datasheets/P8255A.pdf
 *			  https://www.angelfire.com/art2/unicorndreams/msx/RR-PPI.html
 */

#ifndef __MSX_PPI8255REGISTERS__
#define __MSX_PPI8255REGISTERS__

#include <CORE/incs.hpp>

namespace MSX
{
	class PPI8255Registers : public MCHEmul::ChipRegisters
	{
		friend class PPI8255;

		public:
		static const unsigned int _ID = 1200;

		// Constructors...
		/** This constructor is used when the registers will be used directly from the Chip
			and this one from the port manager. \n
			That is when the chip is working under a processor which devices are managed by ports. \n
			A temporal internal phisical memory is created and it is deleted when detroyed. \n
			@see attribute _internalPhysicalMemory */
		PPI8255Registers ();
		/** This constructor is used when the registers are used as an extension of the memory
			and they are accessed from outside. \n
			The physical memory is not owned by this class but the computer (memory class). */
		PPI8255Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s); 

		/** This chip has only 4 registers valid: 0 & 1 & 2 & 4 */
		virtual size_t numberRegisters () const override
							{ return (4); }

		/** To know whether the slot configuration has changed. 
			Once the variable is read, it comes back to false. */
		bool slotChanged () const
							{ return (_slotChanged); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The ones from the parent class plus:
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// These methods are directly invoked when the registers are accesible from a memory position
		// They are also invoked from other methods defined above...
		// ....when the registers are accesible from ports...
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		// These methods reflects the key that is pressed...
		// This method is used only from the chip. No boundary check is done at all...
		void setKeyboardStatusMatrix (unsigned short row, unsigned short bit, bool st)
							{ _keyboardStatusMatrix [(size_t) row].setBit ((size_t) bit, st); }

		void initializeInternalValues ();

		protected:
		/** The different slots slected by bank. 
			When one of them changed, the _slotChanged variable is set and
			used from the simulation method of the processor to raise an event 
			that it would change finally the configuration. */
		unsigned char _slotBank0; // Primary slot selected for bank 0: 0x0000 - 0x4000
		unsigned char _slotBank1; // Primary slot selected for bank 1: 0x4000 - 0x7fff
		unsigned char _slotBank2; // Primary slot selected for bank 2: 0x8000 - 0xbfff
		unsigned char _slotBank3; // Primary slot selected for bank 3: 0xc000 - 0xffff
		/** When any of the previous values change. */
		MCHEmul::OBool _slotChanged;

		// Managing the ketboard...
		/** The information about the key pressed. 
			With all FF means nothing pressed. When something is pressed the right bit becomes false. 
			There are 10 of this elements, equivalent to the number of rows managed in the MSX standard. 
			This number of elements is initialized at construction time. */
		std::vector <MCHEmul::UByte> _keyboardStatusMatrix;
		unsigned char _keyboardRowSelected;
		bool _keyboardCAPSLED; // true when off, false when on...
		bool _keyClickSoundOuput; // true when high, false when low (disconnected)...

		// Mamaging the cassette...
		bool _casetteMotorControl; // true when off, false when on...
		bool _casetteWriteSignal;  // true = high (used when saving info)...


		// Implementation
		/** The last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
