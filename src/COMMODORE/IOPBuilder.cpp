#include <COMMODORE/IOPBuilder.hpp>
#include <COMMODORE/UserIOPeripherals.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>
#include <COMMODORE/1530Datasette.hpp>
#include <C64/Cartridge.hpp>

// ---
MCHEmul::IOPeripheral* COMMODORE::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	MCHEmul::IOPeripheral* result = nullptr;

	if (id == COMMODORE::NoUserIOPeripheral::_ID)
		result = new COMMODORE::NoUserIOPeripheral;
	else if (id == MCHEmul::NoDatasettePeripheral::_ID)
		result = new MCHEmul::NoDatasettePeripheral;
	else if (id == COMMODORE::NoExpansionPeripheral::_ID)
		result = new COMMODORE::NoExpansionPeripheral;
	else if (id == COMMODORE::Datasette1530::_ID)
		/** https://www.c64-wiki.com/wiki/Datassette_Encoding
			CN2 datasette keeps three types on short pulses (in NTSC):
			long pulses = 1488Hz (672us length),
			medium pulses = 1953Hz (512us length),
			short pulses = 2840Hz (352us length = 176 + 176).
			Beging this one the shortest one too:
			0,000176 s/device cycle == (* 1022727 CPU cycles/s) 180 CPU cycles / device cyles.
			Making the numbers with PAL definition, the constant would be the same!.
			Simular calculus can be done for every commodore computer type. */
		result = new COMMODORE::Datasette1530
			(180, new COMMODORE::Datasette1530::TAPFileFormatImplementation (180));

	// Take care, it could be null...
	return (result);
}
