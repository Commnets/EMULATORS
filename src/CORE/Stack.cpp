#include <CORE/Stack.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
void MCHEmul::Stack::reset ()
{ 
	_position = _fromBack ? (int) (size () - 1) : 0; 
	_overflow = false; 
	_notUsed = true; // No elements so far at the beginning...
}

// ---
void MCHEmul::Stack::initialize ()
{ 
	MCHEmul::PhysicalStorageSubset::initialize ();

	reset ();
}

// ---
void MCHEmul::Stack::push (const MCHEmul::UBytes& v)
{
	if (_overflow)
		return;

	_overflow = false;

	for (std::vector <MCHEmul::UByte>::const_iterator i = v.bytes ().begin (); 
			i != v.bytes ().end () && !_overflow; i++)
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
				if (!(_overflow = (_position < 0)))
				{
					set (initialAddress () + (size_t) _position, (*i));

					_position--; // After that it could be negative and no other insertion will allowed later!
				}
			}
			else
			{
				// The pointer doesn't move after this first insertion!
				if (_notUsed)
					set (initialAddress () + (size_t) _position, (*i)); // And not move the pointer...
				else
				{
					if (!(_overflow = (_position < 1)))
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
				if (!(_overflow = ((size_t) _position >= size ())))
				{
					set (initialAddress () + (size_t) _position, (*i));

					_position++;
				}
			}
			else
			{
				if (_notUsed)
					set (initialAddress () + (size_t) _position, (*i)); 
				else
				{
					if (!(_overflow = ((size_t) _position >= (size () - 1))))
					{
						_position++;

						set (initialAddress () + (size_t) _position, (*i));
					}
				}
			}
		}

		_notUsed = false;
	}
}

// ---
MCHEmul::UBytes MCHEmul::Stack::pull (size_t nV)
{
	if (_overflow || _notUsed) // nothing can be pull when error || empty!
		return (MCHEmul::UBytes (std::vector <MCHEmul::UByte> (nV, MCHEmul::UByte::_0))); 

	std::vector <MCHEmul::UByte> dt = { };

	_overflow = false;

	for (size_t i = 0; i < nV && !_overflow; i++)
	{
		if (_fromBack)
		{
			if (_pointToEmpty)
			{
				if (!(_overflow = ((size_t) _position >= size ())))
				{
					_position++;

					dt.push_back (value (initialAddress () + (size_t) _position));
				}
			}
			else
			{
				if (!(_overflow = ((size_t)_position >= (size () - 1))))
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
				if (!(_overflow = (_position < 0)))
				{
					_position--;

					dt.push_back (value (initialAddress () + (size_t) _position));
				}
			}
			else
			{
				if (!(_overflow = (_position < 1)))
				{
					dt.push_back (value (initialAddress () + (size_t) _position));

					_position--;
				}
			}
		}
	}

	// If the final situation of the stack is overflow, then a list of 0 data is returned, 
	// but the status of the stack is set to bad!
	return (_overflow 
		? MCHEmul::UBytes (std::vector <MCHEmul::UByte> (nV, MCHEmul::UByte::_0)) : MCHEmul::UBytes (dt));
}

// ---
MCHEmul::InfoStructure MCHEmul::Stack::getInfoStructure () const
{
	// Take into account that InfoClass method is invoked instead the parent one,
	// as the info from parent class is included in the informatiomn added as a subset...
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("PhysicalStorageSubset",	std::move (MCHEmul::PhysicalStorageSubset::getInfoStructure ()));
	result.add ("BACK",						_fromBack );
	result.add ("LAST",						_pointToEmpty );
	result.add ("OVERFLOW",					_overflow );
	result.add ("EMPTY",					_notUsed );
	result.add ("POSITION",					_position);
	result.add ("ADDRESS",					_initialAddress + _position);

	return (result);
}
