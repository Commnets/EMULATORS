#include <ZXSpectrum/ContendedMemory.hpp>
#include <ZXSpectrum/CZ80.hpp>

// ---
ZXSPECTRUM::ContendedMemory::ContendedMemory
	(int id, MCHEmul::PhysicalStorage* ps, size_t pp,
	 const MCHEmul::Address& iA, size_t s,
	 const MCHEmul::Stack::Configuration& cfg)
	: MCHEmul::Stack (id, ps, pp, iA, s, cfg),
	  _cpu (nullptr)
{
	// Nothing else...
}

// ---
void ZXSPECTRUM::ContendedMemory::whenReading
	(const MCHEmul::Address& a, size_t nB) const
{
	if (_cpu != nullptr)
		_cpu -> recordMemoryAccess
			(a, nB, ZXSPECTRUM::CZ80::MemoryAccess::Type::_READ);
}

// ---
void ZXSPECTRUM::ContendedMemory::whenWriting
	(const MCHEmul::Address& a, size_t nB)
{
	if (_cpu != nullptr)
		_cpu -> recordMemoryAccess
			(a, nB, ZXSPECTRUM::CZ80::MemoryAccess::Type::_WRITE);
}
