#include <ZX81/IOPBuilder.hpp>
#include <ZX81/Cartridge.hpp>
#include <ZX81/Datasette.hpp>
#include <ZX81/ZX81.hpp>

// ---
MCHEmul::IOPeripheral* ZX81::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	assert (dynamic_cast <ZX81::SinclairZX81*> (c) != nullptr);

	MCHEmul::IOPeripheral* result = nullptr;

	if (id == ZX81::Cartridge::_ID)
		result = new ZX81::Cartridge;
	// https://en.wikipedia.org/wiki/ZX81
	else if (id == ZX81::Datasette::_ID)
		/** ZX81 datasette keeps only 1 type of pulse 360us length (150us up / 150 us down)
			The bits 1 are 9 pulses of this type.
			The bits 0 are 4 pulses of this type.
			In both cases there are 1300us of silence in between.
			As the speed of the CPU is 3,25Mhz and any relation with the datasette is done in the fast mode,
			there must be an access to the datasette every 487 cpu cycles = (3,25MHz * 150us).
			The speed is the same in PAL and in NTSC. */
		result = new ZX81::Datasette (487);
	else if (id == ZX81::DatasetteP::_ID)
		result = new ZX81::DatasetteP (150);
	else if (id == ZX81::DatasetteInjection::_ID)
		result = new ZX81::DatasetteInjection (static_cast <ZX81::SinclairZX81*> (c) -> type ());
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
