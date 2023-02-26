#include <CORE/Memory.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

// ---
MCHEmul::UByte MCHEmul::PhysicalStorage::_DEFAULTVALUE (MCHEmul::UByte::_0);

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
	bool e = false;
	std::vector <MCHEmul::UByte> by = MCHEmul::UBytes::loadBytesFrom (fN, bE);
	if (e)
		return (false);

	MCHEmul::UBytes bys = MCHEmul::UBytes (by);
	// The first "sA" bytes are the address (big or little endian format)
	MCHEmul::Address iA (bys.MSUBytes (sA), bE); 
	if (iA > _initialAddress || (iA + by.size () - sA) > lastAddress ())
		return (false); // The data doesn't fit the limits of the memory, so not possible to load!

	// Sets the memory...
	set (iA, bys.LSUBytes (bys.size () - sA));

	return (true);
}

// ---
bool MCHEmul::PhysicalStorageSubset::save (const std::string& fN, size_t sA, bool bE)
{
	// Get the bytes of the address...
	std::vector <MCHEmul::UByte> by = _initialAddress.bytes ();
	if (by.size () < sA)
		for (size_t i = 0; i < (sA - by.size ()); by.insert (by.begin (), MCHEmul::UByte::_0));
	by = MCHEmul::UBytes (by, bE).bytes (); // The way to reverse them if any...

	// Get the data bytes
	std::vector <MCHEmul::UByte> dby = bytes ();
	by.insert (by.end (), dby.begin (), dby.end ());

	// Save them...
	return (MCHEmul::UBytes::saveBytesTo (fN, by));
}

// ---
bool MCHEmul::PhysicalStorage::loadInto (const std::string& fN, size_t pB)
{
	if (pB /** Starts in 0. */ >= size ())
		return (false);

	bool e = false;
	std::vector <MCHEmul::UByte> by = MCHEmul::UBytes::loadBytesFrom (fN, e);
	if (e || by.size () > (size () - pB))
		return (false); // It can not be loaded...

	set (pB, by);

	return (true);
}

// ---
bool MCHEmul::PhysicalStorage::saveFrom (const std::string& fN, size_t nB, size_t p)
{
	if (p /** Starts in 0. */ >= size () || (p + nB) > size ())
		return (false); // It is not in the limits of the memory...

	return (MCHEmul::UBytes::saveBytesTo 
		(fN, std::vector <MCHEmul::UByte> (_data.begin () + p, _data.begin () + p + nB - 1)));
}

// ---
MCHEmul::InfoStructure MCHEmul::PhysicalStorageSubset::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("ID",		_id);
	result.add ("ADDRESS",	_initialAddress);
	result.add ("SIZE",		_size);
	result.add ("ACTIVE",	_active);
	result.add ("READ",		_activeForReading);
	result.add ("BYTES",	bytes ());

	return (result);
}

// ---
MCHEmul::MemoryView::MemoryView (int id, const MCHEmul::PhysicalStorageSubsets& ss)
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
bool MCHEmul::MemoryView::saveFrom (const std::string& fN, size_t nB, const MCHEmul::Address& a)
{
	int dt = 0;
	MCHEmul::PhysicalStorageSubset* ss = nullptr;
	for (MCHEmul::PhysicalStorageSubsets::const_iterator i = _subsets.begin (); 
		i != _subsets.end () && ss == nullptr; i++)
		ss = (*i).second -> isIn (a, dt) ? (*i).second : nullptr;

	return ((ss == nullptr) ? ss -> saveFrom (fN, nB, a) : false);
}

// ---
MCHEmul::InfoStructure MCHEmul::MemoryView::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

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
