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
#include <CORE/NotifyObserver.hpp>

namespace MCHEmul
{
	class MotherboardElement;
	using MotherboardElements = std::vector <MotherboardElement*>;

	/** Replica of the physical wire connecting elements,
		The different elements are pluged into the wire and any communication in it
		is transmitted to all elements connected. */
	class Wire final : public Notifier
	{
		public:
		static const unsigned int _WIREDCHANGED = 111;

		Wire (int id)
			: _id (id),
			  _value (false),
			  _downEdge (false), _upEdge (false)
								{ }
		
		/** The wire doesn't own ever none of the elements connected. */

		int id () const
								{ return (_id); }

		/** Connect a new element only if it was not connected before. */
		virtual void connectElement (MotherboardElement* mE);
		/** Connect a set of motherboard elements to this wire. \n
			The method can be overloaded because not all elements received might make sense to be connected to this wire. \n
			Any overload of this method must take this into account. \n
			By default all are connected. */
		virtual void connectElements (const MotherboardElements& mbE)
							{ for (auto& i : mbE) connectElement (i); }
		virtual void disconnectElement (MotherboardElement* mE);
		
		bool value () const
								{ return (_value); }
		inline void setValue (bool b);

		bool downEdge () const
								{ return (_downEdge); }
		bool upEdge () const
								{ return (_upEdge); }
	
		protected:
		/** An identification for the wire. */
		int _id;
		/** The current value of the wire. */
		bool _value;
		/** When the signal comes from up to down. */
		bool _downEdge;
		/** When the signal comes from ddown to up. */
		bool _upEdge;
	};

	// ---
	inline void Wire::setValue (bool b)
	{ 
		if (b != _value) 
		{ 
			_downEdge = _value && !b; 
			_upEdge = !_value && b; 
			
			_value = b;
			
			notify (Event (_WIREDCHANGED)); 
		} 
	} 
}

#endif
  
// End of the file
/*@}*/