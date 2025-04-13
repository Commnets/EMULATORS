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
		friend class PPI8855;

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

		/** Destroy the internal physycal memory when created. */
		virtual ~PPI8255Registers ();

		/** This chip has only 4 registers valid: 0 & 1 & 2 & 4 */
		virtual size_t numberRegisters () const override
							{ return (0x04); }

		// The class is designed to be accessed from wither a port or from a memory position.
		// When it is accessed from a port the following methods must be used...
		// ...and they invoke the ones used when the positions are accesed from a memory...
		// ...readValue, peekValue & setValue defined below)
		/** To read a register. 
			When reading a register the internal situation could be modified. */
		MCHEmul::UByte readRegister (unsigned char rId) const
							{ return (readValue ((size_t) rId)); }
		/** Like the previous one but not changing tthe internal situation of the registers. */
		MCHEmul::UByte peekRegister (unsigned char rId) const
							{ return (peekValue ((size_t) rId)); }
		/** To change the value of a register. \n
			The internal situation is also change. */
		void setRegister (unsigned char rId, const MCHEmul::UByte& v)
							{ setValue ((size_t) rId, v); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The ones from the parent class plus:
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// This methods are directly invoked when the registers are accesible from a memory position
		// They are also invoked from other methods defined above...
		// ....when the registers are accesible from ports...
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		void initializeInternalValues ();

		protected:
		// Implementation		
		/** This variable is created when the register is created 
			to be accessed fromports and not from internal memory. */
		MCHEmul::PhysicalStorage* _internalPhysicalMemory;
		/** The last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
