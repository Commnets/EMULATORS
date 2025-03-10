#include <CORE/Stack.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
void MCHEmul::Stack::reset ()
{ 
	_position = _configuration._fromBack 
		? ((_configuration._maxSize == -1) 
			? (int) (size () - 1) 
			: (_configuration._maxSize - 1))
		: 0;

	_overflow = false; 

	_notUsed = true; // No elements considered so far at the beginning...
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

	for (std::vector <MCHEmul::UByte>::const_iterator i = v.bytes ().begin (); 
			i != v.bytes ().end () && !_overflow; i++)
	{
		// When _position == -1, 
		// means that an overflow was detected previously, and then nothing can be done!
		// The overflow is only detected if configured, otherwise the pointer starts back
		if (!_overflow)
		{
			if (_configuration._pointToEmpty)
			{
				set (initialAddress () + (size_t) _position, (*i));
				if (_configuration._fromBack) decrementStackPosition ();
				else incrementStackPosition (); // overflow could come here...
			}
			else
			{
				if (_configuration._fromBack) decrementStackPosition ();
				else incrementStackPosition (); // overflow could come here...
				if (!_overflow)
					set (initialAddress () + (size_t) _position, (*i));
			}
		}

		_notUsed = false; // already used once at least...
	}
}

// ---
MCHEmul::UBytes MCHEmul::Stack::pull (size_t nV)
{
	if (_overflow || _notUsed) // nothing can be pull when error || empty!
		return (MCHEmul::UBytes (std::vector <MCHEmul::UByte> (nV, MCHEmul::UByte::_0))); 

	std::vector <MCHEmul::UByte> dt = { };

	for (size_t i = 0; i < nV && !_overflow; i++)
	{
		// When _position == -1, 
		// means that an overflow was detected previously and then nothing can be done!
		// The overflow is only detected if configured, otherwise the pointer starts back
		if (!_overflow)
		{
			if (_configuration._pointToEmpty)
			{
				if (_configuration._fromBack) incrementStackPosition ();
				else decrementStackPosition (); // overflow could come here...
				if (!_overflow)
					dt.push_back (value (initialAddress () + (size_t) _position));
			}
			else
			{
				dt.push_back (value (initialAddress () + (size_t) _position));
				if (_configuration._fromBack) incrementStackPosition ();
				else decrementStackPosition (); // overflow could come here...
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

	result.add ("PhysicalStorageSubset256", 
		std::move (MCHEmul::PhysicalStorageSubset::getInfoStructure ())); // Elements limited to last 256 bytes...
	result.add ("BACK",						_configuration._fromBack );
	result.add ("LAST",						_configuration._pointToEmpty );
	result.add ("OVERFLOW",					_overflow );
	result.add ("EMPTY",					_notUsed );
	result.add ("POSITION",					_position);
	result.add ("ADDRESS",					currentAddress ());

	return (result);
}
