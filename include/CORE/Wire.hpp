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
#include <CORE/Pulse.hpp>
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
			  _pulse (false)
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
								{ return (_pulse.value ()); }
		inline bool setValue (bool b);

		/** This methods will move the status of the ede back to false = 0. */
		bool positiveEdge () const
								{ return (_pulse.positiveEdge ()); }
		bool negativeEdge () const
								{ return (_pulse.negativeEdge ()); }
		bool transition () const
								{ return (_pulse.transition ()); }
		/** Same, but not changing its values. */
		bool peekPositiveEdge () const
								{ return (_pulse.peekPositiveEdge ()); }
		bool peekNegativeEdge () const
								{ return (_pulse.peekNegativeEdge ()); }
		bool peekTransition () const
								{ return (_pulse.transition ()); }
	
		protected:
		/** An identification for the wire. */
		int _id;
		/** To control the pulse. */
		Pulse _pulse;
	};

	// ---
	inline bool Wire::setValue (bool b)
	{ 
		bool result = _pulse.set (b);
		if (result)
			notify (Event (_WIREDCHANGED)); 

		return (result);
	} 
}

#endif
  
// End of the file
/*@}*/