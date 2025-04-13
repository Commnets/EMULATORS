#include <MSX/PortManager.hpp>
#include <MSX/VDP.hpp>

const std::string MSX::GeneralPortManager::_NAME = "MSX General PortManager";
const std::string MSX::VDPPortManager::_NAME = "MSX VDP PortManager";

// ---
MSX::GeneralPortManager::GeneralPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage MSX iteractions with posts not mapped against any device" }
		})
{
	setClassName ("GeneralPortManager");
}

// ---
MCHEmul::UByte MSX::GeneralPortManager::value (unsigned short ab, unsigned char id) const
{ 
	_LOG ("Port " + std::to_string (id) + " not implemented"); 
	
	return (MCHEmul::UByte::_0); 
}

// ---
void MSX::GeneralPortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{ 
	_LOG ("Port " + std::to_string (id) + " not implemented"); 
}

// ---
MSX::VDPPortManager::VDPPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "VDP Port Manager" },
		  { "Description", "Class to manage MSX iteractions with ports mapped to VDP chip" }
		})
{
	setClassName ("VDPPortManager");
}

// ---
void MSX::VDPPortManager::initialize ()
{
	FZ80::Z80Port::initialize ();

	// TODO
}
