#include <COMMODORE/VIA/VIAControlLine.hpp>

// ---
MCHEmul::InfoStructure COMMODORE::VIAControlLine::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("VALUE",	_wire.value ());
	result.add ("MODE",		_mode);
	result.add ("POSEDGE",	_wire.peekPositiveEdge ());
	result.add ("NEGEDGE",	_wire.peekNegativeEdge ());

	return (result);
}
