#include <CORE/Memory.hpp>
#include <CORE/Stack.hpp>
#include <fstream>

// ---
MCHEmul::Memory::Memory (const MCHEmul::Address& iA, size_t s, bool r, const MCHEmul::Memories& blks, MCHEmul::Stack* stk)
	: _initialAddress (iA), _size (s), _values (nullptr), _rom (r), _blocks (blks), _stack (stk),
	  _active (true),
	  _defaultValues (nullptr)
{ 
	// The memory has to have either "some size" or having block behind!
	assert (_size >= 0 || (_size == 0 && !_blocks.empty ()));

	if (_size > 0)
	{
		_values = new MCHEmul::UByte [_size];
		_defaultValues = new MCHEmul::UByte [_size];
		for (size_t i = 0; i < _size; i++)
			_values [i] = _defaultValues [i] = MCHEmul::UByte::_0;
	}
}

// ---
MCHEmul::Memory::~Memory ()
{ 
	bool dS = false;
	for (auto i : _blocks)
	{
		dS |= (i.second == _stack);
		delete (i.second);
	}

	if (!dS)
		delete (_stack);

	delete [] (_values); 
	delete [] (_defaultValues);
}

// ---
size_t MCHEmul::Memory::size () const
{ 
	size_t result = _size;
	for (auto i : _blocks)
		result += i.second -> size ();
	
	return (result); 
}

// ---
void MCHEmul::Memory::fixDefaultValues ()
{
	for (size_t i = 0; i < _size; i++)
		_defaultValues [i] = _values [i];
	for (auto i : _blocks)
		i.second -> fixDefaultValues ();
}

// ---
bool MCHEmul::Memory::isIn (const MCHEmul::Address& a) const
{ 
	if (!_active)
		return (false);

	bool result = (_size > 0 && a >= _initialAddress && _initialAddress.distanceWith (a) < (int) _size);
	if (!result)
		for (MCHEmul::Memories::const_iterator i = _blocks.begin (); i != _blocks.end () && !result; i++)
			result = (*i).second -> isIn (a);

	return (result); 
}

// ---
const MCHEmul::UByte MCHEmul::Memory::value (const MCHEmul::Address& a) const
{
	if (!_active)
		return (MCHEmul::UByte::_0);

	MCHEmul::UByte result = MCHEmul::UByte::_0;

	size_t p = 0;
	if (_size > 0 && a >= _initialAddress && (p = _initialAddress.distanceWith (a)) < (int) _size)
		result = readValue (p);
	else
	{
		bool t = false;
		for (MCHEmul::Memories::const_iterator i = _blocks.begin (); i != _blocks.end () && !t; i++)
		{
			if ((*i).second -> isIn (a))
			{
				result = (*i).second -> value (a);
				t = true;
			}
		}
	}

	return (result);
}

// ---
void MCHEmul::Memory::set (const MCHEmul::Address& a, const MCHEmul::UByte v, bool f)
{
	if (!_active)
		return;

	if (_rom && !f)
		return;

	size_t p = 0;
	if (_size > 0 && a >= _initialAddress && (p = _initialAddress.distanceWith (a)) < (int) _size)
		setValue (p, v);
	else
	{
		bool t = false;
		for (MCHEmul::Memories::iterator i = _blocks.begin (); i != _blocks.end () && !t; i++)
		{
			if ((*i).second -> isIn (a))
			{
				(*i).second -> set (a, v);
				t = true;
			}
		}
	}
}

// ---
const MCHEmul::UBytes MCHEmul::Memory::values (const MCHEmul::Address& a, size_t nB) const
{
	if (!_active)
		return (MCHEmul::UBytes::_E);

	std::vector <MCHEmul::UByte> dt = { };

	size_t p = 0;
	if (_size > 0 && a >= _initialAddress && ((p = _initialAddress.distanceWith (a)) + nB - 1) < (int) _size)
	{
		for (size_t i = 0; i < nB; i++)
			dt.push_back (readValue (p + i));
	}
	else
	{
		bool t = false;
		for (MCHEmul::Memories::const_iterator i = _blocks.begin (); i != _blocks.end () && !t; i++)
		{
			if ((*i).second -> isIn (a))
			{
				dt = (*i).second -> values (a, nB).values ();
				t = true;
			}
		}
	}

	return (MCHEmul::UBytes (dt));
}

// ---
void MCHEmul::Memory::set (const MCHEmul::Address& a, const MCHEmul::UBytes& v, bool f)
{
	if (!_active)
		return;

	if (_rom && !f)
		return;

	size_t p = 0;
	if (_size > 0 && a >= _initialAddress && ((p = _initialAddress.distanceWith (a)) + v.size () - 1) < (int) _size)
	{
		for (size_t i = 0; i < v.size (); i++)
			setValue (p + i, v [i]);
	}
	else
	{
		bool t = false;
		for (MCHEmul::Memories::iterator i = _blocks.begin (); i != _blocks.end () && !t; i++)
		{
			if ((*i).second -> isIn (a))
			{
				(*i).second -> set (a, v);
				t = true;
			}
		}
	}
}

// ---
bool MCHEmul::Memory::initialize ()
{
	for (size_t i = 0; i < _size; i++)
		_values [i] = _defaultValues [i];
	
	bool result = true;
	for (auto i : _blocks)
		result &= i.second -> initialize  ();

	return (result);
}

// ---
bool MCHEmul::Memory::load (const std::string& fN)
{
	if (!_active)
		return (false);

	std::ifstream file (fN, std::ios::binary);
	if (!file)
		return (false);

	file.seekg (0, std::ios::end);
	std::streamoff lF = file.tellg ();
	size_t lA = _initialAddress.size () * MCHEmul::UByte::sizeBits ();
	if (lF > (std::streamoff) std::numeric_limits <size_t>::max () ||
		lF < (std::streamoff) lA || (lF - (std::streamoff) lA) > (std::streamoff) size ())
		return (false); // either bad format or too long for this memory...

	size_t lFA = (size_t) lF;
	char* aDT = new char [lA];
	file.seekg (0, std::ios::beg);
	file.read (aDT, (std::streamsize) lA); // Reads the address where to load the info
	char* fDT = new char [lFA - lA];
	file.read (fDT, (std::streamsize) (lFA - lA)); // Reads the info itself

	file.close ();

	std::vector <MCHEmul::UByte> aV;
	for (size_t i = 0; i < (size_t) (lA / MCHEmul::UByte::size  ()); i += MCHEmul::UByte::size ())
		aV.push_back ((MCHEmul::UByte) (*(aDT + i)));
	std::vector <MCHEmul::UByte> fV;
	for (size_t i = 0; i < (size_t) ((lFA - lA) / MCHEmul::UByte::size ()); i += MCHEmul::UByte::size ())
		fV.push_back ((MCHEmul::UByte) (*(fDT + i)));

	delete [] aDT;
	delete [] fDT;

	MCHEmul::Address adr (aV);
	if (!isIn (adr))
		return (false);
	
	set (adr, fV, true);

	return (true);
}

// ---
bool MCHEmul::Memory::loadInto (const std::string& fN, const MCHEmul::Address& a)
{
	if (!_active)
		return (false);

	if (!isIn (a))
		return (false);

	std::ifstream file (fN, std::ios::binary);
	if (!file)
		return (false);

	file.seekg (0, std::ios::end);
	std::streamoff lF = file.tellg ();
	if (lF > (std::streamoff) std::numeric_limits <size_t>::max () ||
		lF > (std::streamoff) size ())
		return (false); // too long for this memory...

	size_t lFA = (size_t) lF;
	char* fDT = new char [lFA];
	file.seekg (0, std::ios::beg);
	file.read (fDT, (std::streamsize) lFA); // Reads the info itself

	file.close ();

	std::vector <MCHEmul::UByte> fV;
	for (size_t i = 0; i < (size_t) (lFA / MCHEmul::UByte::size ()); i += MCHEmul::UByte::size ())
		fV.push_back ((MCHEmul::UByte) (*(fDT + i)));

	delete [] fDT;
	
	set (a, MCHEmul::UBytes (fV), true);

	return (true);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Memory& m)
{
	if (!m._active)
		return (o);

	o << "---" << std::endl;
	o << "Memory Block Data" << std::endl;
	o << m.initialAddress () << "," << m.size ();

	for (auto i : m._blocks)
	{
		if (dynamic_cast <MCHEmul::Stack*> (i.second) != nullptr)
			o << std::endl << *((MCHEmul::Stack*) i.second); // Specific for the stack...
		else
			o << std::endl << *i.second;
	}

	if (m._size != 0)
	{
		size_t bS = 0x10;
		for (size_t i = 0; i <= (size_t) (m._size / bS); i++)
		{
			if ((i * bS) < m._size)
				o << std::endl;
			
			for (size_t j = 0; j < bS && ((i * bS) + j) < m._size ; j++)
				o << ((j != 0) ? " " : "") << m._values [(i * bS) + j];
		}
	}
		
	return (o);
}
