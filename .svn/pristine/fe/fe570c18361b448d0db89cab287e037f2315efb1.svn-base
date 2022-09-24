#include <COMMS/IPAddress.hpp>

// ---
MCHEmul::IPAddress::IPAddress (const std::string& ip)
	: _a (0x00), _b (0x00), _c (0x00), _d (0x00), _port (0x0000), _error (false)
{
	std::vector <std::string> t = MCHEmul::getElementsFrom (ip, '.');
	if (t.size () == 4)
	{
		_a = (unsigned char) std::stoi (MCHEmul::trim (t [0]));
		_b = (unsigned char) std::stoi (MCHEmul::trim (t [1]));
		_c = (unsigned char) std::stoi (MCHEmul::trim (t [2]));

		size_t pp = 0;
		if ((pp = t [3].find (':')) != std::string::npos)
		{
			_d = std::stoi (MCHEmul::trim (t [3].substr (0, pp)));
			_port = std::stoi (MCHEmul::trim (t [3].substr (pp + 1)));
		}
		else
			_d = (unsigned char) std::stoi (MCHEmul::trim (t [3]));
	}
	else
		_error = true;
}
