#include <global.hpp>

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Attributes& attrs)
{ 
	bool f = true;
	for (auto i : attrs)
	{
		if (!f)
			o << std::endl;
		o << i.first << ":" << i.second;

		f = false;
	}

	return (o);
}
