#include <CORE/Memory.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <fstream>

// ---
MCHEmul::UByte MCHEmul::PhysicalStorage::_DEFAULTVALUE (MCHEmul::UByte::_0);

// ---
std::vector <MCHEmul::UByte> MCHEmul::PhysicalStorage::bytes (size_t pB, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < nB; i++)
		result.push_back (value (pB + i));
	return (result);
}

// ---
MCHEmul::PhysicalStorageSubset::PhysicalStorageSubset 
		(int id, MCHEmul::PhysicalStorage* pS, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::InfoClass ("PhysicalStorageSubset"),
	  _id (id), _physicalStorage (pS), _initialPhisicalPosition (pp), _initialAddress (a), _size (s),
	  _active (true), // By default all are active
	  _activeForReading (true), // It can be switched off
	  _defaultData (_size, MCHEmul::UByte::_0) // The default data is initially = _0;
{ 
	assert (_physicalStorage != nullptr);
	assert (_size > 0);

	// The size defined for a phisical storage subset can not exceed the boundaries of the full phisical storage behind.
	// If it happens then the whole phisical storage behind is considered!
	if (_physicalStorage -> size () < (_initialPhisicalPosition + _size))
	{
		_initialPhisicalPosition = 0;
		_size = _physicalStorage -> size ();

		_defaultData = std::vector <MCHEmul::UByte> (_size, MCHEmul::UByte::_0);
	}
}

// ---
bool MCHEmul::PhysicalStorageSubset::load (const std::string& fN, size_t sA, bool bE)
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
std::vector <MCHEmul::UByte> MCHEmul::PhysicalStorageSubset::bytes (const MCHEmul::Address& a, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;
	
	int dt;
	if (_active && _activeForReading &&
		nB <= _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - nB))) 
		for (size_t i = 0; i < nB; i++)
			result.push_back (readValue (dt + i)); 
	
	return (result);
}

// ---
void MCHEmul::PhysicalStorageSubset::set (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{
	std::vector <MCHEmul::UByte> result;
	
	int dt; 
	if (_active && _physicalStorage -> canBeWriten (f) &&
		v.size () < _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - v.size ())))
		for (size_t i = 0; i < v.size (); i++)
			setValue (dt + i, v [i]);
}

// ---
bool MCHEmul::PhysicalStorage::loadInto (const std::string& fN, size_t pB)
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
MCHEmul::InfoStructure MCHEmul::PhysicalStorageSubset::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID",		_id);
	result.add ("ADDRESS",	_initialAddress);
	result.add ("SIZE",		_size);
	result.add ("ACTIVE",	_active);
	result.add ("READ",		_activeForReading);
	result.add ("BYTES",	bytes ());

	return (result);
}

// ---
MCHEmul::MemoryView::MemoryView (int id, MCHEmul::PhysicalStorageSubsets ss)
	: MCHEmul::InfoClass ("MemoryView"),
	  _id (id), _subsets (ss),
	  // They all will be set in the constructor body!
	  _minAddress (), _maxAddress (), _numPositions (0), _memPositions ()
{
	// Try to create a linear vision of the memory...
	// Identifying first which the min and max address in the memory!
	bool fL = true;
	for (const auto& i : _subsets)
	{
		if (fL)
		{
			_minAddress = i.second -> initialAddress ();
			_maxAddress = i.second -> lastAddress ();
		}
		else
		{
			if (i.second -> initialAddress () < _minAddress) 
				_minAddress = i.second -> initialAddress ();
			if (i.second -> lastAddress () > _maxAddress) 
				_maxAddress = i.second -> lastAddress ();
		}

		fL = false;
	}

	// And then identifying how many physical storages are attached to every memory location...
	_numPositions = (_maxAddress - _minAddress) + 1;
	_memPositions = MCHEmul::MemoryView::MemoryPositions (_numPositions, MCHEmul::MemoryView::MemoryPosition ());
	for (const auto& i : _subsets)
	{
		size_t mA = i.second -> initialAddress () - _minAddress;
		for (size_t j = 0; j < i.second -> size (); j++)
		{
			_memPositions [mA + j]._number++;
			_memPositions [mA + j]._storages.push_back (i.second);
		}
	}
}

// ---
bool MCHEmul::MemoryView::isIn (const MCHEmul::Address& a, int& dt) const
{ 
	int dtT = a.distanceWith (_minAddress);
	if (dtT < 0 || (size_t) dtT > _numPositions)
		return (false);

	if (_memPositions [dtT]._number == 1 && 
		(*_memPositions [dtT]._storages.begin ()) -> active ())
	{
		dt = dtT;
		return (true);
	}

	MCHEmul::PhysicalStorageSubset* f = nullptr;
	const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
	for (MCHEmul::PhysicalStorageSubsetsList::const_iterator i = pL.begin (); i != pL.end () && f == nullptr; i++)
		if ((*i) -> active ()) f = (*i); // The first one active...

	if (f != nullptr) dt = dtT;
	return (f != nullptr);
}

// ---
const MCHEmul::UByte& MCHEmul::MemoryView::value (const MCHEmul::Address& a) const
{
	int dtT = _minAddress.distanceWith (a);
	if (dtT < 0 || (size_t) dtT > _numPositions)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);

	MCHEmul::PhysicalStorageSubset* fS = nullptr;
	const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
	for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
		if (pL [i] -> active () && pL [i] -> activeForReading ()) fS = pL [i];

	if (fS == nullptr)
		return (MCHEmul::PhysicalStorage::_DEFAULTVALUE);
	
	return (fS -> readValue (a - fS -> initialAddress ()));
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::MemoryView::bytes (const MCHEmul::Address& a, size_t nB) const
{
	int dtT = _minAddress.distanceWith (a);
	if (dtT < 0 || (size_t) dtT > (_numPositions - nB))
		return (std::vector <MCHEmul::UByte> { });

	std::vector <MCHEmul::UByte> result;
	for (size_t i = 0; i < nB;i++)
	{ 
		MCHEmul::PhysicalStorageSubset* fS = nullptr;
		const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
		for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
			if (pL [j] -> active () && pL [j] -> activeForReading ()) fS = pL [j];

		result.push_back ((fS != nullptr) 
			? fS -> readValue (a - fS -> initialAddress () + i) : MCHEmul::PhysicalStorage::_DEFAULTVALUE);
	}

	return (result);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const MCHEmul::UByte& d, bool f)
{
	int dtT = _minAddress.distanceWith (a);
	if (dtT < 0 || (size_t) dtT > _numPositions)
		return;

	MCHEmul::PhysicalStorageSubset* fS = nullptr;
	const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions[dtT]._storages;
	for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
		if (pL[i] -> active () && pL[i] -> canBeWriten (f)) fS = pL[i];

	if (fS != nullptr)
		fS -> setValue (a - fS -> initialAddress (), d);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{ 
	int dtT = _minAddress.distanceWith (a);
	if (dtT < 0 || (size_t) dtT > _numPositions)
		return;

	for (size_t i = 0; i < v.size (); i++)
	{
		MCHEmul::PhysicalStorageSubset* fS = nullptr;
		const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
		for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
			if (pL [j] -> active () && pL [j] -> canBeWriten (f)) fS = pL [j];
	
		if (fS != nullptr)
			fS -> setValue (a - fS -> initialAddress () + i, v [i]);
	}
}

// ---
bool MCHEmul::MemoryView::loadInto (const std::string& fN, const MCHEmul::Address& a)
{
	int dt = 0;
	MCHEmul::PhysicalStorageSubset* ss = nullptr;
	for (MCHEmul::PhysicalStorageSubsets::const_iterator i = _subsets.begin (); 
			i != _subsets.end () && ss == nullptr; i++)
		ss = (*i).second -> isIn (a, dt) ? (*i).second : nullptr;

	return ((ss == nullptr) ? ss -> loadInto (fN, a) : false);
}

// ---
MCHEmul::InfoStructure MCHEmul::MemoryView::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	MCHEmul::InfoStructure mems;
	for (const auto& i : _subsets)
		mems.add (std::to_string (i.first), i.second -> getInfoStructure ());
	result.add ("STORAGES", mems);

	return (result);
}

// ---
bool MCHEmul::Memory::Content::verifyCoherence () const
{
	_error = _physicalStorages.empty () || _subsets.empty () || _views.empty ();

	for (const auto& i :_subsets)
		_error |= _physicalStorages.find (i.second -> physicalStorage () -> id ()) == _physicalStorages.end ();
	
	for (const auto& i : _views)
	{
		for (const auto& j : i.second -> subsets ())
		{
			_error |= _subsets.find (j.second -> id ()) == _subsets.end ();
			_error |= _physicalStorages.find (j.second -> physicalStorage () -> id ()) == _physicalStorages.end ();
		}
	}

	return (_error);
}

// ---
bool MCHEmul::Memory::Content::initialize ()
{ 
	if (_error) 
		return (false); 

	for (const auto& i : _subsets)
		i.second -> initialize ();

	return (true); 
}

// ---
MCHEmul::Memory::Memory (const Content& cnt)
	: MCHEmul::InfoClass ("Memory"),
	  _content (), 
	  _activeView (nullptr),
	  _stack (nullptr), 
	  _cpuView (nullptr), 
	  _error (MCHEmul::_NOERROR)
{
	// It has to be verified...
	cnt.verifyCoherence ();

	if (cnt.error ())
		_error = MCHEmul::_INIT_ERROR;
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
	for (const auto& i : _content._physicalStorages) 
		delete (i.second); 

	for (const auto& i : _content._subsets) 
		delete (i.second); 

	for (const auto& i : _content._views) 
		delete (i.second); 
}
