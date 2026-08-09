/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIASerialPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2023 \n
 *	Description: The CIA includes a Serial Port inside
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIASERIALPORTPORT__
#define __COMMODORE_CIASERIALPORTPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** The serial port inside the CIA reads and sends bits sequentially. \n
		Input samples SP on CNT rising edges. \n
		Output uses Timer A underflows to generate the CNT clock. */
	class CIASerialPort final : public MCHEmul::InfoClass, public MCHEmul::Notifier
	{
		public:
		/** The different status that this SerialPort can be in. \n
			At creating the element is _STOPEED. */
		enum class Status
		{
			_READING = 0,
			_SAVING = 1
		};

		CIASerialPort (int id /** unique id in the CIA Chip. */, unsigned int iId)
			: MCHEmul::InfoClass ("CIASerialPort"),
			  _id (id), 
			  _interruptId (iId)
							{ initialize (); }

		Status status () const
							{ return (_status); }
		inline void setStatus (Status s);

		// To manage the value of the port...
		/** Stores a byte in SDR for a future or active transmission. \n
			A pending byte is loaded into the shift register when Timer A is running. */
		inline void setValue (const MCHEmul::UByte& v);
		/** Loads a pending SDR byte when Timer A is running. */
		inline void tryStartTransmission (bool timerARunning);
		const MCHEmul::UByte& value () const
							{ return (_value); }

		bool SPSignal () const
							{ return (_SPPin); }
		void setSPSignal (bool b)
							{ _SPPin = b; }

		/** To know whether the interrupts on the Serial Port are or not allowed. */
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }

		/** To know whether the interrupt condition for the Serial Port has or not been reached. \n
			When this variable is read, the value comes back to false. */
		bool interruptRequested () const
							{ bool r = _interruptRequested; _interruptRequested = false; return (r); }
		/** Same than the previous method but without modifying the value. */
		bool peekInterruptRequested () const
							{ return (_interruptRequested); }

		/** To know whether an interruption related with the Clock should be launched. \n
			That happen when there is a interruption requested and also they are allowed. */
		bool launchInterruption () const
							{ return (_interruptRequested && _interruptEnabled); }

		void initialize ();

		void simulate (bool CNTRisingEdge, bool CNTFallingEdge, bool timerAUnderflow);

		/**
		  *	The name of the fields are: \n
		  *	Status			= Attribute with Read when the statis of the serial port is reading and write in other case.
		  *	Buffer			= Attribute with the current buffer value (read or to be written).
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Implementation
		/** Adds a bit to the input shift register. \n
			Returns true when a complete byte has been received. */
		inline bool addBit (bool b);

		/** Removes one bit from the output shift register. \n
			Returns true when a complete byte has been transmitted. */
		bool removeBit (bool& b)
							{ b = _shiftRegister.shiftLeftC (); return (++_numberBitsTransmitted >= 8); }

		/** Loads the pending SDR byte into the active shift register. */
		inline void startTransmission ();


		private:
		int _id;
		unsigned int _interruptId;
		Status _status;
		MCHEmul::UByte _value;
		bool _SPPin;

		// Implementation
		unsigned int _numberBitsTransmitted;
		bool _interruptEnabled;
		mutable bool _interruptRequested;
		/** Active input/output shift register. */
		MCHEmul::UByte _shiftRegister;
		/** Buffered SDR value waiting to be transmitted. */
		MCHEmul::UByte _transmitBuffer;
		bool _transmitBufferFull;
		bool _transmissionActive;
		bool _generatedCNTSignal;
	};

	// ---
	inline void CIASerialPort::setStatus (Status s)
	{
		if (_status == s)
			return;

		_status = s;

		// Changing the serial direction aborts the active shift operation,
		// but does not discard a byte already written to SDR.
		_transmissionActive = false;
		_numberBitsTransmitted = 0;

		if (_status == Status::_SAVING)
			_generatedCNTSignal = true;
	}

	// ---
	inline void CIASerialPort::setValue (const MCHEmul::UByte& v)
	{ 
		// SDR and the active shift register are separate hardware stages.
		// A new SDR write must not overwrite or restart an active transfer.
		_value = _transmitBuffer = v;
		_transmitBufferFull = true;
	}

	// ---
	inline void CIASerialPort::tryStartTransmission (bool timerARunning)
	{
		if (timerARunning &&
			!_transmissionActive &&
			_transmitBufferFull &&
			_status == Status::_SAVING)
			startTransmission ();
	}

	// ---
	inline void CIASerialPort::startTransmission ()
	{
		_shiftRegister = _transmitBuffer;
		_transmitBufferFull = false;
		_transmissionActive = true;
		_numberBitsTransmitted = 0;
	}

	// ---
	inline bool CIASerialPort::addBit (bool b)
	{ 
		_shiftRegister.shiftLeftC (b);

		if (++_numberBitsTransmitted < 8)
			return (false);

		_value = _shiftRegister;
		_numberBitsTransmitted = 0;

		return (true);
	}
}

#endif
  
// End of the file
/*@}*/
