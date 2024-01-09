/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIAControlLine.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/01/2023 \n
 *	Description: The "ControlLines" of the VIA Chip (@see VIA for better understanding).
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIACONTROLLINE__
#define __COMMODORE_VIACONTROLLINE__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** Every "ControlLine" is defined basically by an id and a wire (@see MCHEmul::Wire). 
		The wire is basically to the chip to connect to others. */
	class VIAControlLine final : public MCHEmul::InfoClass
	{
		public:
		VIAControlLine (int id)
			: MCHEmul::InfoClass ("VIAControlLine"),
			  _wire (id),
			  _mode (0), // Not defined yet...
			  _interruptEnabled (false),
			  _interruptRequested (false)
							{ initialize (); }

		int id () const
							{ return (_wire.id ()); }
			
		const MCHEmul::Wire& wire () const
							{ return (_wire); }
		MCHEmul::Wire& wire ()
							{ return (_wire); }

		/** To set and know the value in the "ControlLine" */
		bool value () const
							{ return (_wire.value ()); }
		void setValue (bool v)
							{ _wire.setValue (v); }

		/** The "ControlLine" can affect the VIA Chip in different ways.
			Every way is defined by a number (0 - 255). */
		unsigned char mode () const
							{ return (_mode); }
		void setMode (unsigned char m)
							{ _mode = m; }

		// Managing interrupt related data...
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }
		bool interruptRequested () const // Bear in mind that, after this method the variables becomes false...
							{ return (_interruptRequested); }
		/** Same than the previous method but without modifying the value. */
		bool peekInterruptRequested () const
							{ return (_interruptRequested.peekValue ()); }
		bool launchInterrupt () const
							{ return (_interruptRequested.peekValue () && _interruptEnabled); }

		inline void initialize ();

		inline bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * VALUE		: Attribute: The value of the wire \n
		  * MODE		: Attribute: The mode under which the "ControlLine" is working. \n
		  * POSEDGE		: Attribute: Last positive edge? \n
		  * NEGEDGE		: Attribute: Las negative edge?
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		MCHEmul::Wire _wire;
		unsigned char _mode;
		/** When interrupts are enabled. */
		bool _interruptEnabled;

		// Implementation
		/** When a interrupt has been requested for this object. */
		MCHEmul::OBool _interruptRequested;
	};

	// ---
	inline void VIAControlLine::initialize ()
	{ 
		_wire.setValue (false); 
		
		_interruptRequested = false; 
	}

	// ---
	inline bool VIAControlLine::simulate (MCHEmul::CPU* cpu)
	{
		if (_wire.transition ())
			_interruptRequested = true;

		return (true);
	}
}

#endif
  
// End of the file
/*@}*/

