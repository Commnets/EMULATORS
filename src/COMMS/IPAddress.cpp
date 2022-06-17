#include <COMMS/IPAddress.hpp>

// ---
MCHEmul::IPAddress::IPAddress (const std::string& ip)
	: _a (0x00), _b (0x00), _c (0x00), _d (0x00), _port (0x0000), _error (false)
{
	unsigned char a = 0;
	unsigned char b = 0;
	unsigned char c = 0;
	unsigned char d = 0;
	unsigned short p = 0;
	size_t pp = 0;

	std::vector <std::string> t = MCHEmul::getElementsFrom (ip, ':');
	if (t.size () == 4)
	{
		a = (unsigned char) std::stoi (MCHEmul::trim (t [0]));
		b = (unsigned char) std::stoi (MCHEmul::trim (t [1]));
		c = (unsigned char) std::stoi (MCHEmul::trim (t [2]));
		if ((pp = t [3].find ('.')) != std::string::npos)
		{
			d = std::stoi (MCHEmul::trim (t [3].substr (0, pp)));
			p = std::stoi (MCHEmul::trim (t [3].substr (pp + 1)));
		}
		else
			d = (unsigned char) std::stoi (MCHEmul::trim (t [3]));
	}
	else
		_error = true;
}
