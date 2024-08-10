#include <ZXSpectrum/PortManager.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/ULARegisters.hpp>

const std::string ZXSPECTRUM::PortManager::_NAME = "ZX81 PortManager";

// ---
ZXSPECTRUM::PortManager::PortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all ZXSpectrum port iteractions" },
		  { "Detail", "FE (to read the keyboard status), FE (to enable NMI generation), FD (to disable NMI generation)" }
		}),
	  _ULA (nullptr),
	  _ULARegisters (nullptr)
{
	setClassName ("PortManager");
}

// ---
void ZXSPECTRUM::PortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{
	// TODO
}

// ---
MCHEmul::UByte ZXSPECTRUM::PortManager::getValue (unsigned short ab, unsigned char id, bool ms) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// TODO

	return (result);
}

// ---
void ZXSPECTRUM::PortManager::linkToULA (ZXSPECTRUM::ULA* ula)
{
	assert (ula != nullptr);

	_ULA = ula;
	_ULARegisters = _ULA -> registers ();
}

// ---
void ZXSPECTRUM::PortManager::initialize ()
{
	// TODO

	_ULARegisters -> setCasetteSignal (false);
}
