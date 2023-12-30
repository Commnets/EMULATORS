#include <CORE/MBElement.hpp>
#include <CORE/Bus.hpp>
#include <CORE/Wire.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::MotherboardElement::getInfoStructure () const
{ 
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ATTRS", _attributes);
	
	return (result);
}

// ---
void MCHEmul::MotherboardElement::connectToBus (MCHEmul::Bus* b)
{
	if (!connectedToBus (b -> id ())) 
		_buses.insert (std::pair <int, Bus*> (b -> id (), b)); 
}

// ---
void MCHEmul::MotherboardElement::disconnectFromBus (MCHEmul::Bus* b)
{
	_buses.erase (b -> id ());
}

// ---
void MCHEmul::MotherboardElement::connectToWire (MCHEmul::Wire* w)
{
	if (!connectedToWire (w -> id ())) 
		_wires.insert (std::pair <int, Wire*> (w -> id (), w)); 
}

// ---
void MCHEmul::MotherboardElement::disconnectFromWire (MCHEmul::Wire* w)
{
	_buses.erase (w -> id ());
}
