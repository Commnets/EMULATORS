/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIAControlLines.hpp \n
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
			The concrete mode ranges are 0..1 for CA1/CB1 and 0..7 for CA2/CB2. */
		unsigned char mode () const
							{ return (_mode); }
		virtual void setMode (unsigned char m)
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
		virtual void whenReadWritePort (bool r /** true if read action, false if write action. */, bool portA) = 0;

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

		protected:
		MCHEmul::Wire _wire;
		unsigned char _mode;
		/** When interrupts are enabled. */
		bool _interruptEnabled;

		// Implementation
		/** When a interrupt has been requested for this object. */
		MCHEmul::OBool _interruptRequested;
	};

	/** Type 1 of "ControlLine".
		In this type read/write actions on linked port (@see VIAPort) will always clear the interrupt flag. \n
		There are only two available modes for this type.
		0 : Interrupt is flagged when the control line value goes from 1 to 0 = negative edge.
			Flag is cleared when read/write action in port linked happen (not in register 0x0f). \n
			Every active edge captures the linked port, even if the flag was already set. \n
		1 : Interrupt is flagged when the control line value goes from 0 to 1 = positive edge.
			Flag is cleared when a read/write action in the linked port happens (not in register 0x0f). \n
			Every active edge captures the linked port; ACR selects whether reads use that capture. \n */
	class VIAControlLineType1 final : public VIAControlLine
	{
		public:
		friend VIA;

		VIAControlLineType1 (int id)
			: VIAControlLine (id),
			  _P (nullptr) // it must be associated later...
							{ }

		/** The collateral effect when read/write action in the port
			is always to clear the interrupt flag. */
		virtual void whenReadWritePort (bool, bool) override
							{ interruptRequested (); }
		bool activeEdgeDetected () const
							{ return (_mode == 0 ? peekNegativeEdge () : peekPositiveEdge ()); }

		virtual void initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		/** A transition in this type of line it is used to latch a value in a associated port. \n
			The value will or not be used depending on the configuration of the port. \n
			This method is to link the port. */
		void linkToPort (VIAPort* p)
							{ _P = p; }

		private:
		VIAPort* _P;
	};

	/** Type 2 of "ControlLine".
		In this type read/write actions on linked port (@see VIAPort) have several collateral effects 
		depending on the _mode: From nothing to impact in the another "ControlLine" linked. \n
		There are 8 modes available for this type.
		0 : Interrupt is flagged when the control line value goes from 1 to 0 = negative edge.
			C2 does not capture port inputs; capture belongs to CA1/CB1. \n
			Flag is cleared when read/write action in the port linked happen (not in register 0x0f). \n
		1 : Independent interrupt input on a negative edge. Port accesses do not clear its flag. \n
		2 : Interrupt input on a positive edge. Port accesses clear its flag. \n
		3 : Independent interrupt input on a positive edge. Port accesses do not clear its flag. \n
		4 : Handshake mode: \n
			CA2 goes low on a linked PA read/write; CB2 goes low only on a linked PB write.  \n
			Value returns high on the linked CA1/CB1 active edge, selected by PCR. \n
			Interrupt flag not affected. \n
		5 : Pulse output mode: \n
			The same PA/PB access rules as handshake mode start a one-cycle low pulse. \n
			Interrupt flag not affected. \n
		6 : Value is maintained always low. \n
			Interrupt flag not affected. \n
		7 : Value is maintained always high. \n
			Interrupt flag not affected. \n
		To this type belongs _CA2 and _CB2. */
	class VIAControlLineType2 final : public VIAControlLine
	{
		public:
		friend VIA;

		/** Selects the pin owner without changing the stored PCR mode. \n
			Serial operation suppresses peripheral CB2 drive/acknowledgement behavior;
			disabling SR restores the latest PCR configuration. */
		enum class Function
		{
			_PERIPHERAL,
			_SERIALINPUT,
			_SERIALOUTPUT,
			_SERIALOUTPUTIDLE
		};

		VIAControlLineType2 (int id)
			: VIAControlLine (id),
			  _function (Function::_PERIPHERAL),
			  _justMovedToFalse (false),
			  _CL (nullptr)
							{ }

		Function function () const
							{ return (_function); }
		void setFunction (Function f);

		virtual void whenReadWritePort (bool r, bool portA) override;
		virtual void setMode (unsigned char m) override;
		virtual void initialize () override;
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		void applyOutputLevel ();
		/** The linked CB1/CA1 edge can complete a peripheral handshake. */
		void linkToControlLine (VIAControlLine* c)
							{ _CL = c; }

		private:
		Function _function;
		/** Keeps a peripheral pulse low for one simulated cycle. */
		MCHEmul::OBool _justMovedToFalse;
		VIAControlLine* _CL;
	};
}

#endif
  
// End of the file
/*@}*/

