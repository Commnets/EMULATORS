/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: MBElement.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 29/12/2023 \n
 *	Description: Every element being part of the mother board of a computer should inherit from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_MBELEMENT__
#define __MCHEMUL_MBELEMENT__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/NotifyObserver.hpp>

namespace MCHEmul
{
	class Bus;
	using Buses = std::map <int, Bus*>;
	class Wire;
	using Wires = std::map <int, Wire*>;

	class MotherboardElement : public InfoClass, public Observer
	{
		public:
		friend Bus;
		friend Wire;

		/** Every motherboard element is defined by a class name a set of attributes (optional). \n
			The motherboard element can be linked to both buses ans wires. */
		MotherboardElement (int id, const std::string& n, const Attributes& attrs =  { })
			: InfoClass (n),
			  _id (id),
			  _attributes (attrs),
			  _buses (), _wires ()
							{ }

		// Just in case...
		virtual ~MotherboardElement ()
								{ }

		int id () const
							{ return (_id); }

		// Accesing to the attributes...
		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		// Accesing to the buses connected...
		const Buses& buses () const
							{ return (_buses); }
		bool connectedToBus (int id) const
							{ return (_buses.find (id) != _buses.end ()); }
		const Bus* bus (int id) const // The outcome will nullptr if the requested bus were not connected...
							{ Buses::const_iterator i = _buses.find (id); return ((i != _buses.end ()) ? (*i).second : nullptr); }
		Bus* bus (int id)
							{ Buses::const_iterator i = _buses.find (id); return ((i != _buses.end ()) ? (*i).second : nullptr); }

		// Accesing to the wires connected
		const Wires& wires () const
							{ return (_wires); }
		bool connectedToWire (int id) const
							{ return (_wires.find (id) != _wires.end ()); }
		const Wire* wire (int id) const // The outcome will nullptr if the requested wire were not connected... 
							{ Wires::const_iterator i = _wires.find (id); return ((i != _wires.end ()) ? (*i).second : nullptr); }
		Wire* wires (int id) 
							{ Wires::const_iterator i = _wires.find (id); return ((i != _wires.end ()) ? (*i).second : nullptr); }

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
	};

	// To simplify the use of list of elements...
	using MotherboardElements = std::vector <MotherboardElement*>;
}

#endif
  
// End of the file
/*@}*/