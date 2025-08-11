#include <ZXSpectrum/IOPBuilder.hpp>
#include <ZXSpectrum/Cartridge.hpp>
#include <ZXSpectrum/Datasette.hpp>
#include <ZXSpectrum/ZXSpectrum.hpp>

// ---
MCHEmul::IOPeripheral* ZXSPECTRUM::IOPeripheralBuilder::createPeripheral 
	(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const
{
	assert (dynamic_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) != nullptr);

	MCHEmul::IOPeripheral* result = nullptr;

	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (3 /** 1/25 * 4 = 1 every 160 miliseconds. */, c);
	else if (id == ZXSPECTRUM::Cartridge::_ID)
		result = new ZXSPECTRUM::Cartridge;
	// https://en.wikipedia.org/wiki/ZX81
	else if (id == ZXSPECTRUM::Datasette::_ID)
		/** ZX81 datasette keeps only 1 type of pulse 360us length (150us up / 150 us down)
			The bits 1 are 9 pulses of this type.
			The bits 0 are 4 pulses of this type.
			In both cases there are 1300us of silence in between.
			As the speed of the CPU is 3,25Mhz and any relation with the datasette is done in the fast mode,
			there must be an access to the datasette every 487 cpu cycles = (3,25MHz * 150us).
			The speed is the same in PAL and in NTSC. */
		result = new ZXSPECTRUM::Datasette (487);
	else if (id == ZXSPECTRUM::DatasetteInjection::_ID)
		result = new ZXSPECTRUM::DatasetteInjection (static_cast <ZXSPECTRUM::SinclairZXSpectrum*> (c) -> type ());
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
