#include <MSX/PortManager.hpp>

const std::string MSX::PortManager::_NAME = "ZX81 PortManager";

// ---
MSX::PortManager::PortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage all MSX port iteractions" }
		})
{
	setClassName ("PortManager");
}

// ---
void MSX::PortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{
	// TODO
}

// ---
MCHEmul::UByte MSX::PortManager::getValue (unsigned short ab, unsigned char id, bool ms) const
{
	MCHEmul::UByte result = 0x00;

	// TODO

	return (result);
}

// ---
void MSX::PortManager::initialize ()
{
	// TODO
}
