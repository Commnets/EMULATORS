#include <MSX/PortManager.hpp>
#include <MSX/VDP.hpp>

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
	// Depending on the port the chip affected will be one or another...
	switch (id)
	{
		// VDP Ports
		case 0x98:
		case 0x99:
		case 0x9B:
		case 0x9C:
		case 0x9D:
		case 0x9E:
		case 0x9F:
			{
				_vdp -> setPortValue (id - 0x98, v);
			}

			break;

		// TODO

		// It shouldn't be here...
		default:
			{
				_LOG ("Port not implemented: " + std::to_string (id));
			}
	}
}

// ---
MCHEmul::UByte MSX::PortManager::getValue (unsigned short ab, unsigned char id, bool ms) const
{
	MCHEmul::UByte result = 0x00;

	// Depending on the port the chip affected will be one or another...
	switch (id)
	{
		// VDP Ports: From 0x98 to 0x9F
		case 0x98:
		case 0x99:
		case 0x9B:
		case 0x9C:
		case 0x9D:
		case 0x9E:
		case 0x9F:
			{
				result = _vdp -> readPortValue (id - 0x98);
			}

			break;

		// TODO

		// It shouldn't be here...
		default:
			{
				_LOG ("Port not implemented: " + std::to_string (id));
			}
	}

	return (result);
}

// ---
void MSX::PortManager::initialize ()
{
	// TODO
}
