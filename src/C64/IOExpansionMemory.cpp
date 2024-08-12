#include <C64/IOExpansionMemory.hpp>

// ---
void C64::IOExpansionMemory::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::PhysicalStorageSubset::setValue (p, v); 
							  
	notify (MCHEmul::Event (_SET, 0, std::shared_ptr <EventData> (new EventData (p, v)))); 
}

// ---
const MCHEmul::UByte& C64::IOExpansionMemory::readValue (size_t p) const
{ 
	const MCHEmul::UByte& r = MCHEmul::PhysicalStorageSubset::readValue (p); 
							  
	(const_cast <IOExpansionMemory*> (this)) -> 
		notify (MCHEmul::Event (_READ, 0, std::shared_ptr <EventData> (new EventData (p, r))));
							  
	return (r); 
}
