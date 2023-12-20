/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VIA Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIAREGISTERS__
#define __COMMODORE_VIAREGISTERS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class VIA;

	/** In the VIA Memory, there are a couple of records that behave different
		when they are read that when they are written. \n
		Read this document for better understanding: \n
		https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf \n
		One of the most important characteristics of the VIA is the port management. \n
		The VIA manages two ports independently: Port A (PA) and Port B (PB). \n
		*/
	class VIARegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend VIA;

		// Some events.
		/** When the ports are actualized. \n
			The parameter is the value (1 or 0) of the actualization. */
		static const unsigned int _PORTAACTUALIZED = 210;
		static const unsigned int _PORTBACTUALIZED = 211;

		VIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		// managing the ports...
		/** Read the value. */
		MCHEmul::UByte portA () const
							{ return (MCHEmul::UByte (_portA)); }
		/** Put a new value into the port. \n
			It can be done "forcing" the value into the port or taking into account the _dataPortDir value. \n
			Forcing can happen when something with a mandatory value is connected to the port. \n
			e.g if a joystick is connected to the pin, when moving, the right switch is grounded and the mandatory value is 0. \n
			In the other hand a keyboard matrix is not connected to anything, so the new values can not be forced. \n
			When not forced: \n
			The values at 1 of the port are maintained at the pins where the dataPortDir is in read (0) mode. \n
			The values selected are pushed to the port at the pins where the dataPortDir is in write (1) mode. \n
			The truth table is: \n
			9120 9121 (0 = input; 1 output)	Port effect \n
			0		0							1 See that the effect in the ouput is by default nothing, then Hi - level!
			0		1							0 \n
			1		0							1 See that the effect in the otput is by default nothing, then Hi - level!
			1		1							1 \n
			Port effect = !(!9120 (_outputRegA) & 9122 (_dataPortADir)) = 9120 | !9122 \n
			The final value of the port is returned. \n
			By default the value is not forced. */
		MCHEmul::UByte setPortA (const MCHEmul::UByte& v, bool f = false)
							{ notify (MCHEmul::Event (_PORTAACTUALIZED, 
								_portA = f ? v.value () : (_outputRegA | ~_dataPortADir) & v.value ()));
							  return (MCHEmul::UByte (_portA)); }
		/** Same than port A. */
		MCHEmul::UByte portB () const
							{ return (MCHEmul::UByte (_portB)); }
		MCHEmul::UByte setPortB (const MCHEmul::UByte& v, bool f = false)
							{ notify (MCHEmul::Event (_PORTBACTUALIZED, 
								_portB = f ? v.value () : (_outputRegB | ~_dataPortBDir) & v.value ()));
							  return (MCHEmul::UByte (_portB)); }

		// The output registers...
		/** Their values are used internally to operate ports. */
		unsigned char outputRegisterA () const
							{ return (_outputRegA); }
		unsigned char outputRegisterB () const
							{ return (_outputRegB); }

		// All these method are invoked from VIA emulation
		/** To know the value of the data direction registers. */
		unsigned char dataPortADir () const
							{ return (_dataPortADir); }
		unsigned char dataPortBDir () const
							{ return (_dataPortBDir); }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		protected:
		/** Ports used by the VIA. */
		mutable unsigned char _portA, _portB;
		/** The output registers that are not neccesary iqual to the ports. */
		mutable unsigned char _outputRegA, _outputRegB;
		/** The direction of the different ports used by the VIA. */
		unsigned char _dataPortADir, _dataPortBDir;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
