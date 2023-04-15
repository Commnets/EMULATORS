/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: MicroBus.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/04/2023 \n
 *	Description: A MicroBus represents all together the set of common signals used
				 in a microprocessor to control how the whole system works: \n
				 A data bus, an address bus a signal to indicate whether both are or not active (or in high impedance)
				 and a signal to indicate whether the indo in the buses are for reading or writting.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_MICROBUS__
#define __MCHEMUL_MICROBUS__

#include <CORE/global.hpp>
#include <CORE/Wire.hpp>
#include <CORE/Bus.hpp>

namespace MCHEmul
{
	class MicroprocessorBus;

	class MicroprocessorBusElement
	{
		public:
		friend MicroprocessorBus;

		virtual ~MicroprocessorBusElement ()
							{ /** nothing else. */ }

		protected:
		/** The notification always ends up in this method. */
		virtual void notified (const UBytes& aV, const UBytes& dV)
							{ /** nothing by default. */ }
	};

	using MicroprocessorBusElements = std::vector <MicroprocessorBusElement*>;

	class MicroprocessorBus final
	{
		public:
		MicroprocessorBus (size_t dS, size_t aS)
			: _active (false),
			  _dataBus (dS),
			  _addressBus (aS),
			  _readActive (true) // Makes no sense when no active...
							{ }
		
		/** The bus doesn't own ever none of the elements connected. */

		/** connected a new element only if it was not connected before. */
		void connectElement (BusElement* wE)
								{ _addressBus.connectElement (wE); _dataBus.connectElement (wE); }
		void disconnectElement (BusElement* wE)
								{ _addressBus.disconnectElement (wE); _dataBus.disconnectElement (wE); }

		// To know whether the micro bus is or not active...
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }

		// To know whether the micro bus is or not active for reading
		// If it is not active for reading it is active for writting!
		bool readActive () const
							{ return (_readActive); }
		void setReadActive (bool rA)
							{ _readActive = rA; }

		/** To write a value to the buses if active. Otherwise it doesn't do anything. */
		void write (const UBytes& aV, const UBytes& dV)
							{ if (_active && !_readActive) 
								{ _addressBus = aV; _dataBus = dV; } }
		/** To read a value from the buses if active. Otherwise it doesn't do anything. */
		void read (UBytes& aV, UBytes dV)
							{ if (_active && _readActive)
								{ aV = _addressBus.value (); dV = _dataBus.value ();  } }
						
		private:
		bool _active;
		Bus _dataBus;
		Bus _addressBus;
		bool _readActive;
	};
}

#endif
  
// End of the file
