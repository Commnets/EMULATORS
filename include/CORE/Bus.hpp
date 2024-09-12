/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Bus.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/04/2023 \n
 *	Description: Trying the emulate a "bus" of data connected to two different computer elements within the mother board.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_BUS__
#define __MCHEMUL_BUS__

#include <CORE/global.hpp>
#include <CORE/NotifyObserver.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class MotherboardElement;
	using MotherboardElements = std::vector <MotherboardElement*>;

	/** Replica of the physical Bus that connects several elements within a computer. \n
		Connected to the Bus there are several Motherboard Elements, and UBytes are transmited throught it. \n
		When a value in set in the bus, the different Motherboard elements connected are notified. */
	class Bus final : public Notifier
	{
		public:
		// Different events
		/** When the bus is actualized. */
		static const unsigned int _BUSACTUALIZED = 110;

		Bus (int id, size_t nE)
			: _id (id),
			  _value (UBytes (std::vector <UByte> (nE, UByte::_0)))
								{ }
		
		/** The bus doesn't own ever none of the elements connected. */

		int id () const
							{ return (_id); }
		
		const MCHEmul::UBytes& value () const
								{ return (_value); }
		inline void setValue (const UBytes& v);

		/** Connect a new element only if it was not connected before. */
		void connectElement (MotherboardElement* mE);
		/** Connect a set of motherboard elements to this bus. \n
			The method can be overloaded because not all elements received might make sense to be connected to this bus. \n
			Any overload of this method must take this into account. \n
			By default all are connected. */
		virtual void connectElements (const MotherboardElements& mbE)
							{ for (const auto& i : mbE) connectElement (i); }
		void disconnectElement (MotherboardElement* mE);

		Bus& operator = (const UBytes& v)
								{ setValue (v); return (*this); }
	
		protected:
		int _id;
		/** The valus kept in the bus. */
		UBytes _value;
	};

	// ---
	inline void Bus::setValue (const UBytes& v)
	{ 
		if (v != _value && v.size () == _value.size ()) /** Always of the same size... */
		{ 
			_value = v;
			
			notify (Event (_BUSACTUALIZED)); 
		} 
	}
}

#endif
  
// End of the file
/*@}*/