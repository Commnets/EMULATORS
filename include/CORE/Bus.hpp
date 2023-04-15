/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Bus.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/04/2023 \n
 *	Description: Trying the emulate a "bus" of data connected to two different elements (usually chips).
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_BUS__
#define __MCHEMUL_BUS__

#include <CORE/global.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class Bus;

	/** The element connected throught out a bus. */
	class BusElement
	{
		public:
		friend Bus;
	
		virtual ~BusElement () 
								{ }
							
		protected:
		/** The notification always ends up in this method.
			It receives the value of the wire, and whether that value is rfelated with a climbing or falling edge. */ 
		virtual void notified (const UBytes& b)
								{ /** nothing by default. */ }
	};

	// Just to simplify the use of a set of those elements...
	using BusElements = std::vector <BusElement*>;

	/** Replica of the physical bus connecting elements. */
	class Bus final
	{
		public:
		Bus (size_t nE)
			: _value (UBytes (std::vector <UByte> (nE, UByte::_0))),
			  _elements ()
								{ }
		
		/** The bus doesn't own ever none of the elements connected. */

		/** connected a new element only if it was not connected before. */
		void connectElement (BusElement* wE)
								{ if (std::find_if (_elements.begin (), _elements.end (), 
									[=](BusElement* e) { return (e == wE); }) == _elements.end ())
									_elements.emplace_back (wE); }
		void disconnectElement (BusElement* wE)
								{ _elements.erase (std::remove_if (
									_elements.begin(), _elements.end(), 
										[=](BusElement* e) { return (e == wE); }), 
									_elements.end()); }
		
		const MCHEmul::UBytes& value () const
								{ return (_value); }
		void setValue (const UBytes& v)
								{ if (v != _value && v.size () == _value.size ()) /** Always of the same size... */
									{ _value = v;
									  notify (); } }

		Bus& operator = (const UBytes& v)
								{ setValue (v); return (*this); }
	
		private:
		/** Just setting a value a notification is transmitted to any element. */
		void notify ()
								{ for (auto i : _elements) 
									i -> notified (_value); }
	
		protected:
		UBytes _value;
		BusElements _elements;
	};
}

#endif
  
// End of the file
/*@}*/