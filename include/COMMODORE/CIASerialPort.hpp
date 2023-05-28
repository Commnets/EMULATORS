/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIASerialPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2023 \n
 *	Description: The CIA includes a Serial Port inside
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIASERIALPORTPORT__
#define __COMMODORE_CIASERIALPORTPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class CIATimer;

	/** The serial port inside the CIA is quite complex. \n
		It read and send bits sequentially. \n
		To read bits the CNT and SP signals are used: \n
		When the CNT is flow low to high (raising edge) the value of the line SP is taken into account. 
		The CIASerialPort has to be configured in OUTPUT mode.
		To write bits the Timer A plays a key role. This is the reason to needed at construction time:
		When the TimerA is half its value and it is running in continuous mode,  */
	class CIASerialPort : public MCHEmul::InfoClass, public MCHEmul::Notifier
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
		void setStatus (Status s)
							{ _status = s; }

		// To manage the value of the port...
		/** The value is always set when there is an intention to transmit it. ºn
			This is done within the method simulate. \n
			The value can be only set when the port has been configured for ouput purpouses. */
		void setValue (const MCHEmul::UByte& v)
							{ _value = v;
							  if (_status == Status::_SAVING)
								{ _bufferValue = v; _toTransmit = true; _numberBitsTransmitted = 0; } }
		const MCHEmul::UByte& value () const
							{ return (_value); }

		/** When the CNTsignal is received 
			it is needed to determine whether a raising edge is generated or not and also a pulse.
			The first will be used in reading operations while the second will be used on reading ones. */
		void setCNTSignal (bool v)
							{ _CNTRaisingEdge = !_CNTPin && (v);
							  _CNTPulse = _CNTPin && !v; 
							  _CNTPin = v; }
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

		/** To know whether an interruption related with the Clock should be launched. \n
			That happen when there is a interruption requested and also they are allowed. */
		bool launchInterruption () const
							{ return (_interruptRequested && _interruptEnabled); }

		void initialize ();

		void simulate (MCHEmul::CPU* cpu, CIATimer* t);

		/**
		  *	The name of the fields are: \n
		  *	Status			= Attribute with Read when the statis of the serial port is reading and write in other case.
		  *	Buffer			= Attribute with the current buffer value (read or to be written).
		  */
		MCHEmul::InfoStructure getInfoStructure () const;

		private:
		// Implementation
		/** Adds a bit to the value stored. \n
			If the maximum bits expected is reached a true is returned, false in other case. 
			In that case an interrupt should be generated if active. */
		inline bool addBit (bool b);

		/** Removes a bit from the value stored.
			Returns true when the maximum bits to extract has been reached and false in other case. 
			The variable passed as reference will hold the bt removed. */
		bool removeBit (bool& b)
							{ b = _bufferValue.shiftLeftC (); return (++_numberBitsTransmitted >= 8); }

		/** To know whether a raising edge or a pulse are still pending to be processed. */
		bool CNTRaisingEdge () const
							{ bool r = _CNTRaisingEdge; _CNTRaisingEdge = false; return (r); }
		bool CNTPulse () const
							{ bool r = _CNTPulse; _CNTPulse = false; return (r); }

		private:
		int _id;
		unsigned int _interruptId;
		Status _status;
		MCHEmul::UByte _value;
		bool _CNTPin, _SPPin;

		// Implementation
		bool _toTransmit;
		unsigned int _numberBitsTransmitted;
		bool _interruptEnabled;
		mutable bool _interruptRequested;
		/** used for receiving and sending info. */
		MCHEmul::UByte _bufferValue;
		mutable bool _CNTPulse, _CNTRaisingEdge;
	};

	// ---
	inline bool CIASerialPort::addBit (bool b)
	{ 
		bool r = false;
		_bufferValue.shiftLeftC (b); 
		if (r = (++_numberBitsTransmitted >= 8))
			_value = _bufferValue; // The value in the buffer is moved into the real one...
		return (r); 
	}
}

#endif
  
// End of the file
/*@}*/
