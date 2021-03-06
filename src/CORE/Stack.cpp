#include <CORE/Stack.hpp>

// ---
void MCHEmul::Stack::initialize ()
{ 
	MCHEmul::PhisicalStorageSubset::initialize ();

	_position = _fromBack ? (int) (size () - 1) : 0; 
	_stackOverflow = false; 
	_empty = true; // No elements so far at the beginning...
}

// ---
void MCHEmul::Stack::push (const MCHEmul::UBytes& v)
{
	if (_stackOverflow)
		return;

	_stackOverflow = false;

	for (std::vector <MCHEmul::UByte>::const_iterator i = v.bytes ().begin (); 
			i != v.bytes ().end () && !_stackOverflow; i++)
	{
		// When the stack is filled from the highest address down to 0...
		if (_fromBack)
		{
			// Filling from highest down to 0, but
			// The stack pointer points to an empty position? 
			if (_pointToEmpty)
			{
				// In that case, 
				// If would be only possible to insert a new byte when that pointer is >= 0 (never less than 1)
				if (!(_stackOverflow = (_position < 0)))
				{
					set (initialAddress () + (size_t) _position, (*i));

					_position--; // After that it could be negative and no other insertion will allowed later!
				}
			}
			else
			{
				// The pointer doesn't move after this first insertion!
				if (_empty)
					set (initialAddress () + (size_t) _position, (*i)); // And not move the pointer...
				else
				{
					if (!(_stackOverflow = (_position < 1)))
					{
						_position--; // After that the position can become 0 not allowing insertions later!

						set (initialAddress () + (size_t) _position, (*i));
					}
				}
			}
		}
		// When the stack is filled from the position 0 to the highest possible...
		else
		{
			if (_pointToEmpty)
			{
				if (!(_stackOverflow = ((size_t) _position >= size ())))
				{
					set (initialAddress () + (size_t) _position, (*i));

					_position++;
				}
			}
			else
			{
				if (_empty)
					set (initialAddress () + (size_t) _position, (*i)); 
				else
				{
					if (!(_stackOverflow = ((size_t) _position >= (size () - 1))))
					{
						_position++;

						set (initialAddress () + (size_t) _position, (*i));
					}
				}
			}
		}

		_empty = false;
	}
}

// ---
MCHEmul::UBytes MCHEmul::Stack::pull (size_t nV)
{
	if (_stackOverflow || _empty)
		return (MCHEmul::UBytes::_E); // nothing can be pull when error || empty!

	std::vector <MCHEmul::UByte> dt = { };

	_stackOverflow = false;

	for (size_t i = 0; i < nV && !_stackOverflow; i++)
	{
		if (_fromBack)
		{
			if (_pointToEmpty)
			{
				if (!(_stackOverflow = ((size_t) _position >= size ())))
				{
					_position++;

					dt.push_back (value (initialAddress () + (size_t) _position));
				}
			}
			else
			{
				if (!(_stackOverflow = ((size_t)_position >= (size () - 1))))
				{
					dt.push_back (value (initialAddress () + (size_t) _position));

					_position++;
				}
			}
		}
		else
		{
			if (_pointToEmpty)
			{
				if (!(_stackOverflow = (_position < 0)))
				{
					_position--;

					dt.push_back (value (initialAddress () + (size_t) _position));
				}
			}
			else
			{
				if (!(_stackOverflow = (_position < 1)))
				{
					dt.push_back (value (initialAddress () + (size_t) _position));

					_position--;
				}
			}
		}
	}

	return (MCHEmul::UBytes (dt));
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Stack& s)
{
	o << (*(static_cast <const MCHEmul::PhisicalStorageSubset*> (&s))) << std::endl;
	o << (s._fromBack ? "Back" : "Front") << "," << (s._pointToEmpty ? "Pointing empty" : "Pointing last") << std::endl;
	o << (s._stackOverflow ? "Overflow" : "No Overflow") << std::endl;
	o << (s._empty ? "Empty" : "With data") << std::endl;
	o << "->" << s.position ();

	return (o);
}
