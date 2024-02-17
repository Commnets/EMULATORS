/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIAControlLine.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
	class VIAPort;
	class VIA;

	/** Every "ControlLine" is defined basically by an id and a wire (@see MCHEmul::Wire). 
		The wire is basically for the chip to be connected to others. */
	class VIAControlLine : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		VIAControlLine (int id);

		int id () const
							{ return (_wire.id ()); }
			
		const MCHEmul::Wire& wire () const
							{ return (_wire); }
		MCHEmul::Wire& wire ()
							{ return (_wire); }

		bool positiveEdge () const
							{ return (_wire.positiveEdge ()); }
		bool peekPositiveEdge () const
							{ return (_wire.peekPositiveEdge ()); }
		bool negativeEdge () const
							{ return (_wire.negativeEdge ()); }
		bool peekNegativeEdge () const
							{ return (_wire.peekNegativeEdge ()); }
		bool transition () const
							{ return (_wire.transition ()); }
		bool peekTransition () const
							{ return (_wire.peekTransition ()); }

		/** To set and know the value in the "ControlLine" */
		bool value () const
							{ return (_wire.value ()); }
		void setValue (bool v)
							{ _wire.setValue (v); }
		void chageValue () 
							{ setValue (!value ()); }

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
		bool interruptRequested () const // Bear in mind that, after this method, the variables becomes false...
							{ return (_interruptRequested); }
		/** Same than the previous method but without modifying the value. */
		bool peekInterruptRequested () const
							{ return (_interruptRequested.peekValue ()); }
		bool launchInterrupt () const
							{ return (_interruptRequested.peekValue () && _interruptEnabled); }

		/** When the a read/write action is done in the linked port (@see VIAPort)
			there might be a colateral effect in the "ControlLine" linked to that.
			This method is invoked from the port linked. */
		virtual void whenReadWritePort (bool r /** true if read action, false if write action. */) = 0;

		virtual void initialize ();

		virtual bool simulate (MCHEmul::CPU* cpu) = 0;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * VALUE		: Attribute: The value in the wire. 1 = true, 0 = false \n
		  * MODE		: Attribute: The mode under which the "ControlLine" is working. \n
		  *							 The interpretation will depend on the type of "ControLine". \n
		  * POSEDGE		: Attribute: Last edge positive? \n
		  * NEGEDGE		: Attribute: Last edge negative?
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** In some modes, when the control line changes the status, 
			the value in a port related might be latched. \n
			The link is not always needed. */
		void linkToPort (VIAPort* p)
							{ _P = p; }
		/** In some type, in some modes, 
			the result might affect another "ControlLine". (pulse mdoes). \n
			The link is not always needeed. */
		void linkToControlLine (VIAControlLine* c)
							{ _CL = c;}

		protected:
		MCHEmul::Wire _wire;
		unsigned char _mode;
		/** When interrupts are enabled. */
		bool _interruptEnabled;

		// Other elements this "ControlLine" has relation with...
		VIAPort* _P;
		VIAControlLine* _CL;

		// Implementation
		/** When a interrupt has been requested for this object. */
		MCHEmul::OBool _interruptRequested;
	};

	/** Type 1 of "ControlLine".
		In this type read/write actions on linked port (@see VIAPort) will always clear the interrupt flag. \n
		There are only two available modes for this type.
		0 : Interrupt is flagged when the control line value goes from 1 to 0 = negative edge.
			Flag is cleared when read/write action in port linked happen (not in register 0x0f). \n
			When flag is set, the value at the port linked is lached. \n
		1 : Interrupt is flagged when the control line value goes from 0 to 1 = positive edge.
			Flag is NOT cleared when read/write action in the port linked happen (not in register 0x0f). \n
			When the flag is set, the value at the port linked is latched. \n */
	class VIAControlLineType1 final : public VIAControlLine
	{
		public:
		VIAControlLineType1 (int id)
			: VIAControlLine (id)
							{ }

		/** The collateral effect when read/write action in the port
			is always to clear the interrupt flag. */
		virtual void whenReadWritePort (bool) override
							{ interruptRequested (); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;
	};

	/** Type 2 of "ControlLine".
		In this type read/write actions on linked port (@see VIAPort) have several collateral effects 
		depending on the _mode: From nothing to impact in the another "ControlLine" linked. \n
		There are 8 modes available for this type.
		0 : Interrupt is flagged when the control line value goes from 1 to 0 = negative edge.
			In the port linked (mandatory) the value in the _IR is latched. \n
			Flag is cleared when read/write action in the port linked happen (not in register 0x0f). \n
		1 : Interrupt is flagged when the control line value goes from 0 to 1 = positive edge.
			In the port linked (mandatory) the value in the _IR is latched. \n
			Flag is NOT cleared when read/write action in the port linked happen (not in register 0x0f). \n
		2 : Like 0, but interrupt is flagged when contro line value goes from 0 to 1 = positive edge. \n
		3 : Like 1, but interrupt is flagged when contro line value goes from 0 to 1 = positive edge. \n
		4 : Handshake mode: \n
			Value goes low = 0, when write operation in linked port happen.  \n
			Value goes back to high = 1, when transition happens in th elinked "ControlLine". \n
			Interrupt flag not affected. \n
		5 : Pulse output mode: \n
			Value goes low for a cycle only. \n
			Interrupt flag not affected. \n
		6 : Value is maintained always low. \n
			Interrupt flag not affected. \n
		7 : Value is maintained always high. \n
			Interrupt flag not affected. \n
		To this type belongs _CA2 and _CB2. */
	class VIAControlLineType2 final : public VIAControlLine
	{
		public:
		VIAControlLineType2 (int id)
			: VIAControlLine (id),
			  _justMovedToFalse (false)
							{ }

		virtual void whenReadWritePort (bool r) override;

		virtual void initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		/** In handshake mode (0x05) a pulse is generate in the control line
			for just one cycle. */
		MCHEmul::OBool _justMovedToFalse;
	};
}

#endif
  
// End of the file
/*@}*/

