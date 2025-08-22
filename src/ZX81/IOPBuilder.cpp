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

	// The ZX81/ZX80 are very slow mnachines!
	if (id == MCHEmul::Typewriter::_ID)
		result = new MCHEmul::Typewriter (8 /** 1/25 * 20 = 1 every 320 miliseconds. */, c);
	else if (id == ZX81::Cartridge::_ID)
		result = new ZX81::Cartridge;
	// https://en.wikipedia.org/wiki/ZX81
	else if (id == SINCLAIR::Datasette::_ID)
		/** In the Datasette for the ZX81, every logical bit (1 or 0) is codified with a train of pulses. \n
			For both a different number of pulses are needed. \n
			To codify a 1, 9 pulses are needed, whilst to codify a 0 only 4 pulses are needed. \n
			The frequecy of both trains pulses is the same = 150 micro seconds per pulse (up + down). \n
			Considering the frequency of the ZX81, which is 3.25MHz, the number of T-States per pulse is 488 T-States. \n
			Following Nyquist-Shannon's theorem, the minimum sampling frequency to reproduce 
			the signal must be double the maximum frequency minimum. So half the number of TStates = 244. \n
			Notice that the frequecy of nthe signal is higher than in a ZXSpectrum. \n
			The programs will load quicker but with more errors than in the Spectrum. \n
			In a ZX80 the way the signals are sent to the EAR/MIC is not the same, but using the video signal. 
			However the final effect is similar and we could then consider "compatible". */
		result = new SINCLAIR::Datasette (244);
	else if (id == ZX81::DatasetteInjection::_ID)
		result = new ZX81::DatasetteInjection (static_cast <ZX81::SinclairZX81*> (c) -> type ());
	else
		result = SINCLAIR::IOPeripheralBuilder::createPeripheral (id, c, prms);

	// Take care, it could be null...
	return (result);
}
