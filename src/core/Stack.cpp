#include <core/Stack.hpp>

// ---
void MCHEmul::Stack::push (const MCHEmul::UBytes& v)
{
	if (_stackOverflow)
		return;

	_stackOverflow = false;

	for (std::vector <MCHEmul::UByte>::const_iterator i = v.values ().begin (); 
			i != v.values ().end () && !_stackOverflow; i++)
		if (!(_stackOverflow = (_position >= size ())))
			set (initialAddress () + _position++, (*i));
}

// ---
MCHEmul::UBytes MCHEmul::Stack::pull (size_t nV)
{
	if (_stackOverflow)
		return (MCHEmul::UBytes::_E); // nothing...

	std::vector <MCHEmul::UByte> dt = { };

	_stackOverflow = false;

	for (size_t i = 0; i < nV; i++)
		if (!(_stackOverflow = (_position <= 0)))
			dt.push_back (value (initialAddress () + --_position));

	return (MCHEmul::UBytes (dt));
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Stack& s)
{
	o << "->" << s.position () << std::endl;
	o << (*(static_cast <const MCHEmul::Memory*> (&s)));

	return (o);
}
