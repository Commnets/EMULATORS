#include <CORE/ChipRegisters.hpp>

// ---
MCHEmul::ChipRegisters::ChipRegisters (int id, size_t nB, size_t nR)
	: MCHEmul::PhysicalStorageSubset
		(id, new MCHEmul::PhysicalStorage (id, MCHEmul::PhysicalStorage::Type::_RAM,
			nR), 0, MCHEmul::Address (nB, 0), nR),
	  _internalStorage (nullptr)
{
	_internalStorage = physicalStorage ();
}

// ---
MCHEmul::ChipRegisters::~ChipRegisters ()
{
	delete (_internalStorage);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::ChipRegisters::valueRegisters () const
{
	std::vector <MCHEmul::UByte> result; 
	for (size_t i = 0; i < numberRegisters (); result.emplace_back (peekValue (i++))); 
	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::ChipRegisters::getInfoStructure () const
{
	// Notice that the method from class InfoClass is invoked instead the one from the direct parent class.
	// It is because the info of the memory as it is is not neccesary, but the register value...
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("BYTES", std::move (valueRegisters ()));

	return (result);
}
