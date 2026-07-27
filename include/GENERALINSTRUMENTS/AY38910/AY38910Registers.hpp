/** \ingroup GENERALINSTRUMENTS */
/*@{*/

/**	
 *	@file	
 *	File: AY38910Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/04/2024 \n
 *	Description: The registers of the AY38910 Chip.
 *	Versions: 1.0 Initial
 *  Based on: https://theoddys.com/acorn/semiconductor_datasheets/P8255A.pdf
 *			  https://www.angelfire.com/art2/unicorndreams/msx/RR-PPI.html
 */

#ifndef __GENERALINSTRUMENTS_AY38910REGISTERS__
#define __GENERALINSTRUMENTS_AY38910REGISTERS__

#include <CORE/incs.hpp>
#include <GENERALINSTRUMENTS/AY38910/AY38910LibWrapper.hpp>

namespace GENERALINSTRUMENTS
{
	class AY38910Registers : public MCHEmul::ChipRegisters
	{
		friend class AY38910;

		public:
		static const unsigned int _ID = 1200;

		enum class IOPort
		{
			_A = 0,
			_B
		};

		// Constructors...
		/** This constructor is used when the registers will be used directly from the Chip
			and this one from the port manager. \n
			That is when the chip is working under a processor which devices are managed by ports. \n
			A temporal internal phisical memory is created and it is deleted when detroyed. \n
			@see attribute _internalPhysicalMemory */
		AY38910Registers ();
		/** This constructor is used when the registers are used as an extension of the memory
			and they are accessed from outside. \n
			The physical memory is not owned by this class but the computer (memory class). */
		AY38910Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s); 

		/** The chip is accessed through register selection, data write and data read. */
		virtual size_t numberRegisters () const override
							{ return (0x03); }

		/** To know the internal control register currently selected. */
		unsigned char selectedControlRegister () const
							{ return (_selectedControlRegister); }

		/** To get and set the values driven by the external devices connected to the I/O ports. */
		const MCHEmul::UByte& ioPortInputValue (IOPort p) const
							{ return (_IOPortInputValues [(size_t) p]); }
		void setIOPortInputValue (IOPort p, const MCHEmul::UByte& v)
							{ _IOPortInputValues [(size_t) p] = v; }

		/** To get the value latched for output at an I/O port. */
		const MCHEmul::UByte& ioPortOutputLatch (IOPort p) const
							{ return (_IOPortOutputLatches [(size_t) p]); }

		/** To know the direction and resolved value of an I/O port. */
		bool ioPortIsOutput (IOPort p) const;
		MCHEmul::UByte ioPortValue (IOPort p) const;

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  * The attributes and infostructures of the parent class, plus: \n
		  * SELECTEDREGISTER	= Attribute: Internal AY register selected through the address port. \n
		  * CONTROLREGISTERS	= Attribute: Effective values of the 16 internal AY registers. \n
		  * PORTAINPUT		= Attribute: Value driven externally at I/O port A. \n
		  * PORTAOUTPUTLATCH	= Attribute: Last value written to the output latch of I/O port A. \n
		  * PORTAVALUE		= Attribute: Value resolved at the pins of I/O port A. \n
		  * PORTAOUTPUT		= Attribute: Whether I/O port A is configured as output. \n
		  * PORTBINPUT		= Attribute: Value driven externally at I/O port B. \n
		  * PORTBOUTPUTLATCH	= Attribute: Last value written to the output latch of I/O port B. \n
		  * PORTBVALUE		= Attribute: Value resolved at the pins of I/O port B. \n
		  * PORTBOUTPUT		= Attribute: Whether I/O port B is configured as output. \n
		  * LASTVALUEREAD	= Attribute: Last value read through the data port.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// This methods are directly invoked when the registers are accesible from a memory position
		// They are also invoked from other methods defined above...
		// ....when the registers are accesible from ports...
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		void initializeInternalValues ();

		/** This method is to establish the wrapper used. */
		void setAY38910LibWrapper (AY38910LibWrapper* w)
							{ _AY38910Wrapper = w; }

		protected:
		/** The control register selected that will be accessed by default. */
		unsigned char _selectedControlRegister;
		/** A reference to AY38910 Lib Wrapper. */
		AY38910LibWrapper* _AY38910Wrapper;

		// Implementation		
		/** Values driven at the pins by the devices connected to ports A and B. */
		MCHEmul::UByte _IOPortInputValues [2];
		/** Values written to the output latches of ports A and B. */
		MCHEmul::UByte _IOPortOutputLatches [2];
		/** The last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
