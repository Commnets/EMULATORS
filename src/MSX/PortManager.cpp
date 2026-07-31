#include <MSX/PortManager.hpp>
#include <MSX/VDP.hpp>

const std::string MSX::GeneralPortManager::_NAME = "MSX General PortManager";
const std::string MSX::VDPPortManager::_NAME = "MSX VDP PortManager";
const std::string MSX::PPIPortManager::_NAME = "MSX PPI PortManager";
const std::string MSX::PSGPortManager::_NAME = "MSX PSG PortManager";

// ---
MSX::GeneralPortManager::GeneralPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "General Port Manager" },
		  { "Description", "Class to manage MSX iteractions with posts not mapped against any device" }
		}),
	  _portWriteNotLinked (),
	  _portReadNotLinked ()
{
	setClassName ("GeneralPortManager");
}

// ---
MCHEmul::UByte MSX::GeneralPortManager::value (unsigned short ab, unsigned char id) const
{ 
	if (std::find (_portReadNotLinked.begin (),
		_portReadNotLinked.end (), id) == _portReadNotLinked.end ())
	{
		_portReadNotLinked.push_back (id);

		_LOG ("PortManager::value: Unattached output port " +
			std::to_string ((int) id));
	}
	
	return (MCHEmul::UByte::_0); 
}

// ---
void MSX::GeneralPortManager::setValue (unsigned short ab, unsigned char id, const MCHEmul::UByte& v)
{ 
	if (std::find (_portWriteNotLinked.begin (),
		_portWriteNotLinked.end (), id) == _portWriteNotLinked.end ())
	{
		_portWriteNotLinked.push_back (id);
		_LOG ("PortManager::setValue: Unattached input port " +
			std::to_string ((int) id));
	}
}

// ---
MSX::VDPPortManager::VDPPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "VDP Port Manager" },
		  { "Description", "Class to manage MSX iteractions with ports mapped to VDP chip" }
		}),
	  _vdp (nullptr)
{
	setClassName ("VDPPortManager");
}

// ---
MSX::PPIPortManager::PPIPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "PPI Port Manager" },
		  { "Description", "Class to manage MSX iteractions with ports mapped to PPI chip" }
		}),
	  _ppi (nullptr)
{
	setClassName ("PPIPortManager");
}

// ---
MSX::PSGPortManager::PSGPortManager ()
	: FZ80::Z80Port (_ID, _NAME,
		{ { "Name", "PSG Port Manager" },
		  { "Description", "Class to manage MSX iteractions with ports mapped to PSG chip" }
		}),
	  _psg (nullptr)
{
	setClassName ("PSGPortManager");
}
