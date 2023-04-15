/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Wire.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/04/2023 \n
 *	Description: Trying the emulate a "wire" connected two or pins in different elements (usually chips).
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_WIRE__
#define __MCHEMUL_WIRE__

#include <CORE/global.hpp>

namespace MCHEmul
{
	class Wire;

	/** The element connected throught out a wire. */
	class WiredElement
	{
		public:
		friend Wire;
	
		virtual ~WiredElement () 
								{ }
							
		protected:
		/** The notification always ends up in this method.
			It receives the value of the wire, and whether that value is rfelated with a climbing or falling edge. */ 
		virtual void notified (bool b, bool uE, bool dE)
								{ /** nothing by default. */ }
	};

	// Just to simplify the use of a set of those elements...
	using WiredElements = std::vector <WiredElement*>;

	/** Replica of the physical wire connecting elements,
		The different elements are pluged into the wire and any communication in it
		is transmitted to all elements connected. */
	class Wire final
	{
		public:
		Wire ()
			: _value (false),
			  _downEdge (false), _upEdge (false),
			  _elements ()
								{ }
		
		/** The wire doesn't own ever none of the elements connected. */

		/** connected a new element only if it was not connected before. */
		void connectElement (WiredElement* wE)
								{ if (std::find_if (_elements.begin (), _elements.end (), 
									[=](WiredElement* e) { return (e == wE); }) == _elements.end ())
									_elements.emplace_back (wE); }
		void disconnectElement (WiredElement* wE)
								{ _elements.erase (std::remove_if (
									_elements.begin(), _elements.end(), 
										[=](WiredElement* e) { return (e == wE); }), 
									_elements.end()); }
		
		bool value () const
								{ return (_value); }
		void setValue (bool b)
								{ if (b != _value) 
									{ _downEdge = _value && !b; _upEdge = !_value && b; _value = b;
									  notify (); } } // the edges might be important in the behaviour of some wire elements...
		bool downEdge () const
								{ return (_downEdge); }
		bool upEdge () const
								{ return (_upEdge); }
	
		private:
		/** Just setting a value a notification is transmitted to any element. */
		void notify ()
								{ for (auto i : _elements) 
									i -> notified (_value, _upEdge, _downEdge); }
	
		protected:
		bool _value;
		WiredElements _elements;
	
		// Implementation
		bool _downEdge;
		bool _upEdge;
	};

	// Just to simplify the use of a set of wires
	using Wires = std::vector <Wire*>;
}

#endif
  
// End of the file
/*@}*/