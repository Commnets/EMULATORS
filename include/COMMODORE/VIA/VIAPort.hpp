/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIAPorts.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/01/2024 \n
 *	Description: The "Ports" of the VIA Chip (@see VIA for better understanding).
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIAPORTS__
#define __VIC20_VIAPORTS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class VIAControlLineType1;
	class VIAControlLineType2;
	class VIATimer1;
	class VIA;

	/** 
	  *	The "Port" reflects the behaviour of a set of things in VIA.
	  *	A "Port" is made up of basically 4 pieces: \n
	  *	1.-	A set of pins (8) "_port", 
	  *		where any device can be connected to move in/out data into/from the computer. \n
	  *	2.-	A Out-Register (_OR),
	  *		with the info that the computer would like to move out. \n
	  *	3.-	A In-Register (_IR),
	  *		with the info to be moved into the computer. \n
	  *	4.-	A Direction Data Register (_DDR) that defines which bits of the _OR register will be moved into the pin,
	  *		and which bits of the _IR refister will be moved from the pin into the computer (data bus). \n
	  * When the "Port" is read, the value in the _IR is read. \n
	  * A collateral action might happen in the associated "ControlLines". 
	  * What this action is about will depend on the mode in which the "ControlLine" is working (@see VIAControlLine). \n
	  * That value might be latched if the configuration defines that possiblity (@see method setLanch, and VIARegisters). \n
	  * The latch operation happens (if allowed) when the interrupt flag of the "ControlLine" linked happens (@see VIAControlLine). \n
	  * A "Timer" might be also linked to the "Port" (Timer1 usually). \n
	  * In some modes of the timer (Timer1 when ACR7 = true) anytime reaches0 = true, P7 changes its value.
	  */
	class VIAPort : public MCHEmul::InfoClass, public MCHEmul::Notifier
	{
		public:
		friend VIA;

		// Events...
		/** Event sent when the value of the port has changed. */
		static const unsigned int _VIAPORTIOBITSACTUALIZED = 240;

		VIAPort (int id);

		/** To get access to the bus to connect the VIA (if needed) to other elements. */
		const MCHEmul::Bus& bus () const
							{ return (_port); }
		MCHEmul::Bus& bus ()
							{ return (_port); }

		bool latchIR () const
							{ return (_latch); }
		void setLatchIR (bool l)
							{ _latch = l; }
		void latchValue ()
							{ _valueLatched = portValue (); }

		/** To fix the value of the port pins. \n
			This routine can be used from externally the class, 
			but it also used from the method simulate. \n
			A notification will be sent. */
		void setPortValue (const MCHEmul::UByte& v)
							{ _port.setValue (MCHEmul::UBytes ({ v })); } // A notification will be issued...
		const MCHEmul::UByte& portValue () const
							{ return (_port.value ()[0]); }

		/** To get the value present in the "Port" (potentially to be moved into the data bus). \n
			The value = pins when no latching. \n
			Any time this method is executed the "ControlLines" interrupt flag is cleared only if r = true (default). */
		virtual const MCHEmul::UByte& value (bool r = true) const;
		/** To set the value to be sent (initially) to the port pins. */
		virtual void setValue (const MCHEmul::UByte& v, bool r = true);

		/** To read/set the direction of the data to the port pins. \n
			A bit to 0 in the value means input, while a bit to 1 means output. */
		const MCHEmul::UByte& DDR () const
							{ return (_DDR); }
		void setDDR (const MCHEmul::UByte& v)
							{ _DDR = v; }

		/** Internal registers. 
			Not used very often. */
		const MCHEmul::UByte& OR () const
							{ return (_OR); }
		const MCHEmul::UByte& IR () const
							{ return (_IR); }

		virtual void initialize ();

		/** Returns true with everything ok, and false in any other circunstance. 
			The simulation behaviour is a bit different depending on the type of port. */
		virtual bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  *	OR		: Attribute: The Output Register.
		  *	IR		: Attribute: The Input Register.
		  *	DDR		: Atrribute: The Direction Register.
		  *	Port"	: Attribute: The value present at the port.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		/** To get the info of the port like an string. \n
			It is used something by the debug pieces. \n 
			It can be overloaded. */
		virtual std::string asString () const
							{ return (_OR.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
									  _IR.asString (MCHEmul::UByte::OutputFormat::_HEXA) + "," +
									  _DDR.asString (MCHEmul::UByte::OutputFormat::_HEXA)); }

		protected:
		/** Link to "ControlLines". 
			Could be overloaded e.g. to control the type of control lines linked. */
		virtual void linkAtControlLines (VIAControlLineType1* cl1, VIAControlLineType2* cl2)
							{ _CL1 = cl1; _CL2 = cl2;  }

		/** Notify changes in the value of the port.
			The first parameter represents the bit that changed, and the second is the new value. */
		virtual void notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v);

		protected:
		int _id;
		/** Latch or not the value in _IR. */
		bool _latch;
		/** The value latched, if the option is activ. */
		MCHEmul::UByte _valueLatched;
		/** The value present at the pins of the VIC. 
			It is represented using a Bus of 8 bits, to detect changes e.g in it. */
		MCHEmul::Bus _port;
		/** The Data Direction Register. 
			A 0 in the bit means the correspondant pin in the port will act as input, 
			whilst a 1 means that the pin will be output. */
		MCHEmul::UByte _DDR;
		/** The Output Register:
			Per bit, this would be the true table for the port value:
			_DDR		_OR		OLDOUT	NEWOUTPUT
			0(input)	0		0		0
			0			0		1		1
			0			1		0		0
			0			1		1		1
			1(output)	0		0		0
			1			0		1		0
			1			1		0		1
			1			1		1		1 */
		MCHEmul::UByte _OR;
		mutable MCHEmul::UByte _IR;

		// Elements related with this one...
		VIAControlLineType1 *_CL1;
		VIAControlLineType2 *_CL2;

		// Implementation
		MCHEmul::UByte _lastPortValue;
	};

	/** The Port A follows pretty much the standard behaviour. */
	class VIAPortA final : public VIAPort
	{
		public:
		VIAPortA (int id)
			: VIAPort (id)
							{ /** Nothing else. */ }
	};

	/** The Port B has a different behaviour in some bits. \n
		Bit 6 is used to detect pulses, and bit 7 is used to reflect the timer2 status. */
	class VIAPortB final : public VIAPort
	{
		public:
		friend VIA;

		// Events...
		/** When the value of the PIN3 has changed. 
			This is used to notify write actions in the datsette. */
		static const unsigned int _VIAPORTN3ACTUALIZED = 241;

		VIAPortB (int id);

		/** To know whether there is a pulse happen in bit 6. */
		bool p6Pulse () const // Bear in mind that when this method is invoked "transition" becomes again false...
							{ return (_p6.transition ()); }
		bool peekP6negativeEdge () const // When this one, the "transition" status is not cleared...
							{ return (_p6.peekNegativeEdge ()); }
		/** To set the value to be sent (initially) to the port pins. \n
			The value that finally will be present in the port pins might be slighly different
			depending on the Direction Register (DDR) value and others (reflection of timer2 in bit 7 of output register). \n
			Any time this method is executed the "ControlLines" interrupt flag is cleared only if r = true. */
		bool p7 () const
							{ return (_p7); }
		/** Set the value of the bit 7. \n
			Usually this is called from "Timer" simulation in some modes, when reaches0 = true. */
		void setP7 (bool v)
							{ _p7 = v; }
		/** Just to change the value of the bit 7. \n
			Same comments than above. */
		void changeP7 ()
							{ _p7 = !_p7; }

		/** In the PortB the value returned finally for the PINS declared as output
			is the value in the OR Register. \n
			This is because a Darlington Transistor can be connected to that port and, 
			when a big current is demanded the value in that port pint coould bring down up to 0, 
			reading then as a 0 instead a 1. */
		virtual const MCHEmul::UByte& value (bool r = true) const override;

		virtual void initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		/** Link to "Timer" 
			Could be overloaded e.g. to control the type of control lines linked. */
		void linkAtTimer (VIATimer1* t)
							{ _T = t; }

		virtual void notifyPortChanges (const MCHEmul::UByte& c, const MCHEmul::UByte& v) override;

		private:
		// Elements related with this one...
		VIATimer1* _T;

		bool _p7;
		MCHEmul::Pulse _p6;
	};
}

#endif
  
// End of the file
/*@}*/
