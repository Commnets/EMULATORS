/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: IO6510PortRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/03/2024 \n
 *	Description: The 6510 chip uses a port. \n
 *				 That port is located at address 0 (data direction) and 1 (port value) of the RAM. \n
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_IO6150PORTREGISTERS__
#define __F6500_IO6150PORTREGISTERS__

#include <CORE/incs.hpp>

namespace F6500
{
	class IO6510PortRegisters : public MCHEmul::ChipRegisters
	{
		public:
		// Different events
		static const unsigned int _PORTVALUECHANGED = 160;

		/** bU = Byte with the bits used. 
			This is because in 6510 there is only 6 bits used whilst in 7501 (used in 264 series) 
			there are 8 bit used. */
		IO6510PortRegisters (int id, MCHEmul::PhysicalStorage* ps, 
			const MCHEmul::UByte& bU = 0b00111111 /** 6 LSB bits by default. */);

		virtual size_t numberRegisters () const override
							{ return (2); }

		// Managing the port value...
		/** To get the value of the port. */
		const MCHEmul::UByte& portValue () const
							{ return (_portValue); }
		/** To set the value of the port externally. 
			Only the bit defines as input can be changed, the rest stays at the same output value than before,
			because they are force from the 6510 tristate TTL diodes. */
		void setPortValue (const MCHEmul::UByte& pV)
							{ _portValue = (~_dirValue & (pV | _mask)) | (_dirValue & _portValue); }
		void setBitPortValue (size_t p, bool v)
							{ MCHEmul::UByte pV = _portValue; pV.setBit (p, v); setPortValue (pV); }

		virtual void initialize () override;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		/** Has to be defined if needed, 
			by default all pins are output. */
		virtual void initializeInternalValues ()
							{ _dirValue = _outputValue = _portValue = MCHEmul::UByte::_FF; }
		
		/** Method to notify changes in the port value. \n
			Every bit of the parameter c defines whether the correspondent port pin has changed (true) or not (false). \n
			What is notified (and the events used) finally should take this into account. \n
			By default the change is communicate in a generic way, if there were any change! */
		virtual void notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v)
							{ if (c != MCHEmul::UByte::_0) 
								notify (MCHEmul::Event (_PORTVALUECHANGED, v.value ())); }
		
		protected:
		// Values of the registers...
		/** The direction of the info. */
		MCHEmul::UByte _dirValue;
		/** The value in the output register. */
		MCHEmul::UByte _outputValue;
		/** The value in the port. */
		MCHEmul::UByte _portValue;

		// Implementation
		MCHEmul::UByte _mask;
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
