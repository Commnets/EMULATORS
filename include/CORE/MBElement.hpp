/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: MBElement.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/12/2023 \n
 *	Description: Every element being part of the mother board of a computer should inherit from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_MBELEMENT__
#define __MCHEMUL_MBELEMENT__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/NotifyObserver.hpp>
#include <CORE/Ticks.hpp>

namespace MCHEmul
{
	class Bus;
	using Buses = std::map <int, Bus*>;
	class Wire;
	using Wires = std::map <int, Wire*>;

	/** Any element connected to the mother board. \n
		It can be a CPU, a chip, a memory element or a device. 
		The elements can be connected to busses or wires and the y can also have 
		a external clock cobbected to it (defoned by a ticksDelayed element. */
	class MotherboardElement : public InfoClass, public Observer
	{
		public:
		friend Bus;
		friend Wire;

		/** Every motherboard element is defined by a class name a set of attributes (optional). \n
			The motherboard element can be linked to both buses ans wires. \n
			The class admits also a reference to a tick counter. */
		MotherboardElement (int id, const std::string& n, const Attributes& attrs =  { })
			: InfoClass (n),
			  _id (id),
			  _attributes (attrs),
			  _buses (), _wires (),
			  _ticksCounter (nullptr) // By default, but it can be changed
							{ }

		// Just in case...
		virtual ~MotherboardElement ()
								{ }

		int id () const
							{ return (_id); }

		// Accesing to the attributes...
		const Attributes& attributes () const
							{ return (_attributes); }
		inline const std::string& attribute (const std::string& aN) const;

		// Accesing to the buses connected...
		const Buses& buses () const
							{ return (_buses); }
		bool connectedToBus (int id) const
							{ return (_buses.find (id) != _buses.end ()); }
		inline const Bus* bus (int id) const; // The outcome will nullptr if the requested bus were not connected...
		inline Bus* bus (int id);

		// Accesing to the wires connected
		const Wires& wires () const
							{ return (_wires); }
		bool connectedToWire (int id) const
							{ return (_wires.find (id) != _wires.end ()); }
		inline const Wire* wire (int id) const; // The outcome will nullptr if the requested wire were not connected... 
		inline Wire* wires (int id); 

		// Assigning a tick counter...
		/** It can be nullptr. */
		void assignTicksCounter (TicksCounterDelayed& tc)
							{ _ticksCounter = &tc; }
		const TicksCounterDelayed* ticksCounter () const
							{ return (_ticksCounter); }
		TicksCounterDelayed* ticksCounter ()
							{ return (_ticksCounter); }

		virtual bool initialize ()
							{ return (true); } // By default it does nothing...

		/** To get just the name of the class and the attributes. */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		void connectToBus (Bus* b);
		void disconnectFromBus (Bus* b);
		void connectToWire (Wire* w);
		void disconnectFromWire (Wire* w);

		protected:
		int _id;
		Attributes _attributes;		
		Buses _buses;
		Wires _wires;
		/** This can be nullptr, take care. */
		TicksCounterDelayed* _ticksCounter;
	};

	// To simplify the use of list of elements...
	using MotherboardElements = std::vector <MotherboardElement*>;

	// ---
	inline const std::string& MotherboardElement::attribute (const std::string& aN) const
	{ 
		Attributes::const_iterator i = _attributes.find (aN); 
		
		return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); 
	}

	// ---
	inline const Bus* MotherboardElement::bus (int id) const
	{ 
		Buses::const_iterator i = _buses.find (id); 
		
		return ((i != _buses.end ()) ? (*i).second : nullptr); 
	}

	// ---
	inline Bus* MotherboardElement::bus (int id)
	{ 
		Buses::const_iterator i = _buses.find (id); 
		
		return ((i != _buses.end ()) ? (*i).second : nullptr); 
	}

	// ---
	inline const Wire* MotherboardElement::wire (int id) const
	{ 
		Wires::const_iterator i = _wires.find (id); 
		
		return ((i != _wires.end ()) ? (*i).second : nullptr); 
	}

	// ---
	inline Wire* MotherboardElement::wires (int id) 
	{ 
		Wires::const_iterator i = _wires.find (id); 
		
		return ((i != _wires.end ()) ? (*i).second : nullptr); 
	}
}

#endif
  
// End of the file
/*@}*/