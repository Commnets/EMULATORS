#include <CORE/Memory.hpp>
#include <fstream>

// ---
MCHEmul::UByte MCHEmul::PhisicalStorage::_DEFAULTVALUE (MCHEmul::UByte::_0);

// ---
std::vector <MCHEmul::UByte> MCHEmul::PhisicalStorage::bytes (size_t pB, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < nB; i++)
		result.push_back (value (pB + i));
	return (result);
}

// ---
MCHEmul::PhisicalStorageSubset::PhisicalStorageSubset 
		(int id, MCHEmul::PhisicalStorage* pS, size_t pp, const MCHEmul::Address& a, size_t s)
	: _id (id), _phisicalStorage (pS), _initialPhisicalPosition (pp), _initialAddress (a), _size (s),
	  _active (true), // By default all are active
	  _activeForReading (true), // It can be switched off
	  _defaultData (_size, MCHEmul::UByte::_0) // The default data is initially = _0;
{ 
	assert (_phisicalStorage != nullptr);
	assert (_size > 0);

	// The size defined for a phisical storage subset can not exceed the boundaries of the full phisical storage behind.
	// If it happens then the whole phisical storage behind is considered!
	if (_phisicalStorage -> size () < (_initialPhisicalPosition + _size))
	{
		_initialPhisicalPosition = 0;
		_size = _phisicalStorage -> size ();

		_defaultData = std::vector <MCHEmul::UByte> (_size, MCHEmul::UByte::_0);
	}
}

// ---
bool MCHEmul::PhisicalStorageSubset::load (const std::string& fN, size_t sA, bool bE)
{
	std::ifstream file (fN, std::ios::binary);
	if (!file)
		return (false);

	file.seekg (0, std::ios::end);
	std::streamoff lF = file.tellg ();
	if (lF > (std::streamoff) std::numeric_limits <size_t>::max () ||
		lF < (std::streamoff) sA || (lF - (std::streamoff) sA) > (std::streamoff) size ())
		return (false); // either bad format or too long for this memory...

	size_t lFA = (size_t) lF;
	char* aDT = new char [sA];
	file.seekg (0, std::ios::beg);
	file.read (aDT, (std::streamsize) sA); // Reads the address where to load the info
	char* fDT = new char [lFA - sA];
	file.read (fDT, (std::streamsize) (lFA - sA)); // Reads the info itself

	file.close ();

	std::vector <MCHEmul::UByte> aV;
	for (size_t i = 0; i < (size_t) (sA / MCHEmul::UByte::size  ()); i += MCHEmul::UByte::size ())
		aV.push_back ((MCHEmul::UByte) (*(aDT + i)));
	std::vector <MCHEmul::UByte> fV;
	for (size_t i = 0; i < (size_t) ((lFA - sA) / MCHEmul::UByte::size ()); i += MCHEmul::UByte::size ())
		fV.push_back ((MCHEmul::UByte) (*(fDT + i)));

	delete [] aDT;
	delete [] fDT;

	int dt;
	MCHEmul::Address adr (aV, bE);
	if (!isIn (adr, dt))
		return (false);
	
	set (adr, fV, true);

	return (true);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::PhisicalStorageSubset::bytes (const MCHEmul::Address& a, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;
	
	int dt;
	if (_active && _activeForReading &&
		nB < _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - nB))) 
		for (size_t i = 0; i < nB; i++)
			result.push_back (readValue (dt + i)); 
	
	return (result);
}

// ---
void MCHEmul::PhisicalStorageSubset::set (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{
	std::vector <MCHEmul::UByte> result;
	
	int dt; 
	if (_active && _phisicalStorage -> canBeWriten (f) &&
		v.size () < _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - v.size ())))
		for (size_t i = 0; i < v.size (); i++)
			setValue (dt + i, v [i]);
}

// ---
bool MCHEmul::PhisicalStorage::loadInto (const std::string& fN, size_t pB)
{
	std::ifstream file (fN, std::ios::binary);
	if (!file)
		return (false);

	file.seekg (0, std::ios::end);
	std::streamoff lF = file.tellg ();
	if (pB > size () || lF > (std::streamoff) (size () - pB))
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
	
	set (pB, fV);

	return (true);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::PhisicalStorageSubset& ps)
{
	if (!ps.active ())
		return (o);

	o << "Memory Subset Data" << std::endl;
	o << "(Id:" << ps.id () << "), Initial Address:" << ps.initialAddress () << ", Size:" << ps.size ();
	o << ", Read " << (ps.activeForReading () ? "allowed" : "not allowed");

	if (ps.size () != 0)
	{
		size_t bS = 0x10;
		for (size_t i = 0; i <= (size_t) (ps.size () / bS); i++)
		{
			if ((i * bS) < ps.size ())
				o << std::endl;
			
			for (size_t j = 0; j < bS && ((i * bS) + j) < ps.size (); j++)
				o << ((j != 0) ? " " : "") << ps.value (ps.initialAddress () + (i * bS) + j);
		}
	}
		
	return (o);
}

// ---
bool MCHEmul::MemoryView::isIn (const MCHEmul::Address& a, int& dt) const
{ 
	bool result = false;
	for (MCHEmul::PhisicalStorageSubsets::const_iterator i = _subsets.begin ();
			i != _subsets.end () && !result; i++)
		result = (*i).second -> isIn (a, dt); // Only one is enought...

	return (result);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const MCHEmul::UByte& d, bool f)
{
	int dt = 0;
	bool w = false;
	for (MCHEmul::PhisicalStorageSubsets::const_iterator i = _subsets.begin (); 
			i != _subsets.end () && !w; i++)
		if (w = ((*i).second -> canBeWriten (f) && (*i).second -> isIn (a, dt))) // The first one when it is possible...
			(*i).second -> setValue (dt, d); // Access directly to the low level method to speed up access...
											 // and to avoid "isIn" to be executed twice
}

// ---
const MCHEmul::UByte& MCHEmul::MemoryView::value (const MCHEmul::Address& a) const
{
	int dt = 0;
	for (auto i : _subsets)
		if (i.second -> activeForReading () && i.second -> isIn (a, dt))
			return (i.second -> readValue (dt)); // Access directly to the low level method to speed up the access...
												 // and to avoid "isIn" to be executed twice!

	return (MCHEmul::PhisicalStorage::_DEFAULTVALUE);
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::MemoryView::bytes (const MCHEmul::Address& a, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;
	
	int dt = 0;
	bool r = false;
	for (MCHEmul::PhisicalStorageSubsets::const_iterator i = _subsets.begin ();
			i != _subsets.end () && !r; i++)
	{
		if (r = ((*i).second -> active () && (*i).second -> activeForReading () && 
			(a >= (*i).second -> initialAddress () && 
			(dt = (*i).second -> initialAddress ().distanceWith (a)) <= (int) ((*i).second -> size () - nB))))
			for (size_t j = 0; j < nB; j++)
				result.push_back ((*i).second -> readValue (dt + j));
	}

	return (result);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{ 
	int dt = 0;
	bool w = false;
	for (MCHEmul::PhisicalStorageSubsets::const_iterator i = _subsets.begin ();
			i != _subsets.end () && !w; i++)
	{
		if (w = ((*i).second -> active () && (*i).second -> canBeWriten (f) && 
			(a >= (*i).second -> initialAddress () && 
				(dt = (*i).second -> initialAddress ().distanceWith (a)) <= (int) ((*i).second -> size () - v.size ()))))
			for (size_t j = 0; j < v.size (); j++)
				(*i).second -> setValue (dt + j, v [j]);
	}
}

// ---
bool MCHEmul::MemoryView::loadInto (const std::string& fN, const MCHEmul::Address& a)
{
	int dt = 0;
	MCHEmul::PhisicalStorageSubset* ss = nullptr;
	for (MCHEmul::PhisicalStorageSubsets::const_iterator i = _subsets.begin (); 
			i != _subsets.end () && ss == nullptr; i++)
		ss = (*i).second -> isIn (a, dt) ? (*i).second : nullptr;

	return ((ss == nullptr) ? ss -> loadInto (fN, a) : false);
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::MemoryView& mv)
{
	o << "---" << std::endl;
	o << "Memory View Data";

	for (auto i : mv.subsets ())
		o << std::endl << i.second; 
		
	return (o);
}

// ---
bool MCHEmul::Memory::Content::verifyCoherence () const
{
	_error = _phisicalStorages.empty () || _subsets.empty () || _views.empty ();

	for (auto i :_subsets)
		_error |= _phisicalStorages.find (i.second -> phisicalStorage () -> id ()) == _phisicalStorages.end ();
	
	for (auto i : _views)
	{
		for (auto j : i.second -> subsets ())
		{
			_error |= _subsets.find (j.second -> id ()) == _subsets.end ();
			_error |= _phisicalStorages.find (j.second -> phisicalStorage () -> id ()) == _phisicalStorages.end ();
		}
	}

	return (_error);
}

// ---
bool MCHEmul::Memory::Content::initialize ()
{ 
	if (_error) 
		return (false); 

	for (auto i : _subsets)
		i.second -> initialize ();

	return (true); 
}

// ---
MCHEmul::Memory::Memory (const Content& cnt)
	: _content (), 
	  _activeView (nullptr),
	  _stack (nullptr), 
	  _cpuView (nullptr), 
	  _lastError (MCHEmul::_NOERROR)
{
	// It has to be verified...
	cnt.verifyCoherence ();

	if (cnt.error ())
		_lastError = MCHEmul::_INIT_ERROR;
	else
	{
		_content = cnt;

		// Just to have one by default...
		_activeView = _content.firstView ();
	}
}

// ---
MCHEmul::Memory::~Memory ()
{ 
	for (auto i : _content._phisicalStorages) 
		delete (i.second); 

	for (auto i : _content._subsets) 
		delete (i.second); 

	for (auto i : _content._views) 
		delete (i.second); 
}

// ---
std::ostream& MCHEmul::operator << (std::ostream& o, const MCHEmul::Memory& m)
{
	o << "---" << std::endl;
	o << "Memory Data" << std::endl;
	if (m.lastError () != MCHEmul::_NOERROR) o << "Error";
	else *m.activeView (); // Only the active view is prited out is there is no errors...
		
	return (o);
}
