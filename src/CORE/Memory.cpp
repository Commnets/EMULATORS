#include <CORE/Memory.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>

size_t MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT = std::numeric_limits <size_t>::max (); // Meaning no limit...
MCHEmul::Memory::Configuration MCHEmul::Memory::_CONFIGURATION = 
	MCHEmul::Memory::Configuration ();

// ---
MCHEmul::UByte MCHEmul::PhysicalStorage::_DEFAULTVALUE (MCHEmul::UByte::_0);

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
bool MCHEmul::PhysicalStorage::saveFrom (const std::string& fN, size_t nB, size_t p) const
{
	if (p /** Starts in 0. */ >= size () || (p + nB) > size ())
		return (false); // It is not in the limits of the memory...

	return (MCHEmul::UBytes::saveBytesTo 
		(fN, std::vector <MCHEmul::UByte> (_data.begin () + p, _data.begin () + p + nB - 1)));
}

// ---
MCHEmul::InfoStructure MCHEmul::PhysicalStorageSubsetDUMP::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("CLASSNAME",	std::string ("PhysicalStorageSubsetDUMP"));
	result.add ("ID",			_id);
	result.add ("NAME",			_name);
	result.add ("RAM",			_RAM);
	result.add ("ACTIVE",		_active);
	result.add ("READ",			_activeForReading);
	result.add ("FROM",			_from);
	result.add ("TO",			_to);
	result.add ("BYTES",		_bytes);

	return (result);
}

// ---
MCHEmul::PhysicalStorageSubset::PhysicalStorageSubset 
		(int id, MCHEmul::PhysicalStorage* pS, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::InfoClass ("PhysicalStorageSubset"),
	  _id (id), _name (""),
	  _physicalStorage (pS), _initialPhisicalPosition (pp), _initialAddress (a), _size (s),
	  _deepDebugFile (nullptr),
	  _active (true), // By default all are active
	  _activeForReading (true), // It can be switched off
	  _bufferMemorySetCommands (true), // By default the storage is active for buffering set commands if the option were active
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
void MCHEmul::PhysicalStorageSubset::set
	(const MCHEmul::Address& a, const MCHEmul::UByte& d, bool f)
{ 
	int dt = 0; 

	if (_physicalStorage -> canBeWriten (f) && isIn (a, dt))
	{
		if (MCHEmul::Memory::configuration ().bufferMemorySetCommands (this))
			MCHEmul::Memory::configuration ().addMemorySetCommand (MCHEmul::SetMemoryCommand (this, dt, d));
		else
			setValue (dt, d); 
	}
}

// ---
const MCHEmul::UByte& MCHEmul::PhysicalStorageSubset::value (const MCHEmul::Address& a) const
{ 
	int dt = 0; 

	return ((_activeForReading && isIn (a, dt))
		? readValue (dt) : MCHEmul::PhysicalStorage::_DEFAULTVALUE); 
}

// ---
const MCHEmul::UByte& MCHEmul::PhysicalStorageSubset::valueDirect (const Address& a) const
{ 
	int dt = 0; 

	return ((a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) < (int) _size) 
		? readValue (dt) : MCHEmul::PhysicalStorage::_DEFAULTVALUE); 
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::PhysicalStorageSubset::bytes (const MCHEmul::Address& a, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;

	int dt;
	if (_active && _activeForReading &&
		nB <= _size && 
		(a >= _initialAddress && 
			(dt = _initialAddress.distanceWith (a)) <= (int) (_size - nB)))
			// dt is always positive or 0, but the operand compared couldn't, 
			// and then this logical expression won't match!
	{
		// At this place, it is guaranteed that the first position is within the memory
		// but not necessarily the rest. So the boundaries muct be checked!
		for (size_t i = 0; i < nB; i++)
		{
			size_t pos = (size_t) dt + i;
			if (pos < _size)
				result.emplace_back (readValue (pos)); 
		}
	}

	return (result);
}

// ---
void MCHEmul::PhysicalStorageSubset::set 
	(const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{
	int dt; 
	if (_active && _physicalStorage -> canBeWriten (f) &&
		v.size () <= _size && 
		(a >= _initialAddress && 
			(dt = _initialAddress.distanceWith (a)) <= (int) (_size - v.size ())))
		// Read the explanations above!
	{
		for (size_t i = 0; i < v.size (); i++)
		{
			size_t pos = (size_t) dt + i;
			if (pos < _size)
			{
				if (MCHEmul::Memory::configuration ().bufferMemorySetCommands (this))
					MCHEmul::Memory::configuration ().addMemorySetCommand (MCHEmul::SetMemoryCommand (this, pos, v [i]));
				else
					setValue (pos, v [i]);
			}
		}
	}
}

// ---
void MCHEmul::PhysicalStorageSubset::put 
	(const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{
	int dt; 
	if (_active && _physicalStorage -> canBeWriten (f) &&
		v.size () <= _size && 
		(a >= _initialAddress && 
			(dt = _initialAddress.distanceWith (a)) <= (int) (_size - v.size ())))
		// Also read the explanations above!
	{
		for (size_t i = 0; i < v.size (); i++)
		{
			size_t pos = (size_t) dt + i;
			if (pos < _size)
				setValue (pos, v [i]);
		}
	}
}

// ---
void MCHEmul::PhysicalStorageSubset::fillWith (const MCHEmul::Address& addr, const MCHEmul::UByte& b, size_t nB)
{ 
	if (addr >= _initialAddress) // Only if the requested address is bigger than the initial position...
		for (size_t i = (addr - _initialAddress); i < _size; i++) 
			setValue (i, b); // The boundaries are controlled in the loop!
}

// ---
MCHEmul::PhysicalStorageSubsetDUMP MCHEmul::PhysicalStorageSubset::dump 
	(const MCHEmul::Address& f, const MCHEmul::Address& t) const
{
	MCHEmul::PhysicalStorageSubsetDUMP result 
		({ _id, _name,
			type () == MCHEmul::PhysicalStorage::Type::_RAM,
			_active,
			_activeForReading,
			f, t, { }
			});

	// The last position has to be after the first!
	if (t >= f) 
	{
		MCHEmul::Address fst = (f >= _initialAddress) ? f : _initialAddress;
		MCHEmul::Address lst = (t <= lastAddress ()) ? t : lastAddress ();
		if (fst <= lst)
		{
			result._from = fst; result._to = lst;
			std::vector <MCHEmul::UByte> dt;
			for (size_t i = 0; i <= (size_t) (lst - fst); i++)
				dt.push_back (value (fst + i)); // If the data didn't belong to the set, the default value would be added...
												// The boundaries are also checked in the loop...
			result._bytes = dt;
		}
	}

	return (result);
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
	put (iA, bys.LSUBytes (bys.size () - sA));

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
MCHEmul::InfoStructure MCHEmul::PhysicalStorageSubset::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("NAME",		_name);
	result.add ("ADDRESS",	_initialAddress);
	result.add ("SIZE",		_size);
	result.add ("ACTIVE",	_active);
	result.add ("READ",		_activeForReading);
	/** The memory subset could be long.
		As much as the CPU allows.
		So the number of bytes to print out can be limited. */
	result.add ("BYTES",	std::move (bytes (initialAddress (), 
		(size () > MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT) 
			? MCHEmul::PhysicalStorageSubset::_GETINFOBYTESLIMIT : size ())));

	return (result);
}

// ---
void MCHEmul::SetMemoryCommand::execute ()
{ 
	if (_subset -> deepDebugActive ())
	{
		_subset -> _deepDebugFile -> writeCompleteLine
			(_subset -> name (), std::numeric_limits <unsigned int>::max (), 
				"Applied Set\t\t\t:Address=$" +
				MCHEmul::removeAll0 ((_subset -> initialAddress () + _position).
					asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "," +
				"Value=$" + _value.asString (MCHEmul::UByte::OutputFormat::_HEXA, 2));
	}

	_subset -> setValue (_position, _value); 
}

// ---
MCHEmul::InfoStructure MCHEmul::MemoryViewDUMP::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("CLASSNAME",	std::string ("MemoryViewDUMP"));
	result.add ("ID",			_id);

	int ct = 0;
	MCHEmul::InfoStructure dt;
	for (const auto& i : _data)
		dt.add (MCHEmul::fixLenStr (std::to_string (ct++), 5, true, MCHEmul::_CEROS), 
			std::move (i.getInfoStructure ()));
	result.add ("DATA", std::move (dt));

	return (result);
}

// ---
MCHEmul::MemoryView::MemoryView (int id, const MCHEmul::PhysicalStorageSubsets& ss)
	: MCHEmul::InfoClass ("MemoryView"),
	  _id (id), _subsets (ss),
	  // They all will be set in the constructor body!
	  _minAddress (), _maxAddress (), _numPositions (0), _memPositions ()
{
	plainMemory ();
}

// ---
bool MCHEmul::MemoryView::addSubset (MCHEmul::PhysicalStorageSubset* s)
{ 
	PhysicalStorageSubsets::const_iterator i = _subsets.find (s -> id ());
	bool result = (i == _subsets.end ());
	if (result)
	{
		_subsets.insert (MCHEmul::PhysicalStorageSubsets::value_type (s -> id (), s));

		plainMemory ();
	}
	
	return (result); 
}

// ---
bool MCHEmul::MemoryView::removeSubSet (int id)
{
	PhysicalStorageSubsets::const_iterator i = _subsets.find (id);
	bool result = (i != _subsets.end ());
	if (result)
	{
		_subsets.erase (i);

		plainMemory ();
	}
	
	return (result); 
}

// ---
const MCHEmul::UByte& MCHEmul::MemoryView::value (const MCHEmul::Address& a) const
{
	UByte& result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= _numPositions)
	{
		MCHEmul::PhysicalStorageSubset* fS = nullptr;
		const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
		for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
			if (pL [i] -> active () && pL [i] -> activeForReading ()) 
				fS = pL [i];

		// If a valid storage has been found under the address requested...
		if (fS != nullptr)
			result = fS -> readValue ((size_t) (a - fS -> initialAddress ())); // ...the value is got!
	}

	return (result);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const MCHEmul::UByte& d, bool f)
{
	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= _numPositions)
	{
		MCHEmul::PhysicalStorageSubset* fS = nullptr;
		const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
		for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
			if (pL [i] -> active () && pL [i] -> canBeWriten (f)) fS = pL [i];

		// If a valid storage is under the position requested...
		if (fS != nullptr)
		{
			if (MCHEmul::Memory::configuration ().bufferMemorySetCommands (fS))
				MCHEmul::Memory::configuration ().addMemorySetCommand 
					(MCHEmul::SetMemoryCommand (fS, a - fS -> initialAddress (), d));
			else
				fS -> setValue ((size_t) (a - fS -> initialAddress ()), d); // ...save the value...
		}
	}
}

// ---
std::vector <MCHEmul::UByte> MCHEmul::MemoryView::bytes (const MCHEmul::Address& a, size_t nB) const
{
	std::vector <MCHEmul::UByte> result;

	// If more bytes are required than max available nothing is returned...
	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= (_numPositions - nB))
	{
		for (size_t i = 0; i < nB; i++)
		{ 
			MCHEmul::PhysicalStorageSubset* fS = nullptr;
			const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
			for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
				if (pL [j] -> active () && pL [j] -> activeForReading ()) 
					fS = pL [j];

			// At this point it is sure that the first position requested is inside the memory identified...
			// ..but not necessary the last one. So it is needed to control that we don't exceed the boundaries!
			// and if this happen a DEFAULT value is kept into the list to be returned!
			size_t pos = (fS != nullptr) 
				? ((size_t) (a - fS -> initialAddress ()) + i) : 0;
			result.emplace_back ((fS != nullptr && pos < fS -> size ())
				? fS -> readValue (pos) : MCHEmul::PhysicalStorage::_DEFAULTVALUE);
		}
	}
			
	return (result);
}

// ---
void MCHEmul::MemoryView::set (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{ 
	// If there are more bytes to set than max available nothing is done...
	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= (_numPositions - v.size ()))
	{
		for (size_t i = 0; i < v.size (); i++)
		{
			MCHEmul::PhysicalStorageSubset* fS = nullptr;
			const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
			for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
				if (pL [j] -> active () && pL [j] -> canBeWriten (f)) fS = pL [j];

			// At this point it is guarantteed that the first position will be inside the memory
			// but the rests won't. To avoid a crash in the system it is needed to control
			// the boundaries..
			size_t pos = (fS != nullptr) 
				? ((size_t) (a - fS -> initialAddress ()) + i) : 0;
			if (fS != nullptr && pos < fS -> size ())
			{
				if (MCHEmul::Memory::configuration ().bufferMemorySetCommands (fS))
					MCHEmul::Memory::configuration ().addMemorySetCommand 
						(MCHEmul::SetMemoryCommand (fS, pos, v [i]));
				else
					fS -> setValue (pos, v [i]);
			}
		}
	}
}

// ---
void MCHEmul::MemoryView::put (const MCHEmul::Address& a, const MCHEmul::UByte& d, bool f)
{
	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= _numPositions)
	{
		MCHEmul::PhysicalStorageSubset* fS = nullptr;
		const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
		for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
			if (pL [i] -> active () && pL [i] -> canBeWriten (f)) fS = pL [i];

		// If a valid storage is under the position requested...
		if (fS != nullptr)
			fS -> setValue ((size_t) (a - fS -> initialAddress ()), d); // ...save the value...
	}
}

// ---
void MCHEmul::MemoryView::put (const MCHEmul::Address& a, const std::vector <MCHEmul::UByte>& v, bool f)
{ 
	// If there are more bytes to set than max available nothing is done...
	int dtT = _minAddress.distanceWith (a);
	if (dtT >= 0 && (size_t) dtT <= (_numPositions - v.size ()))
	{
		for (size_t i = 0; i < v.size (); i++)
		{
			MCHEmul::PhysicalStorageSubset* fS = nullptr;
			const MCHEmul::PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
			for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
				if (pL [j] -> active () && pL [j] -> canBeWriten (f)) fS = pL [j];

			// The next action goes directly against the memory,
			// so to avoid unexpected results (or even a crash of the system),
			// it is needed to verify that we don't go over the boundaries...
			size_t pos = (fS != nullptr) 
				? ((size_t) (a - fS -> initialAddress ()) + i) : 0;
			if (fS != nullptr && pos < fS -> size ())
				fS -> setValue (pos, v [i]); // If it were out the limits, the operations wouldn't take place...
		}
	}
}

// ---
MCHEmul::MemoryViewDUMP MCHEmul::MemoryView::dump (const MCHEmul::Address& f, const MCHEmul::Address& t) const
{
	MCHEmul::MemoryViewDUMP result { _id, { } };

	int dt;
	if (t >= f && // The first position has to belong to the storage...
		(dt = (f - _minAddress)) >= 0)
		for (const auto& i : _memPositions [(size_t) dt]._storages)
			result._data.emplace_back (std::move (i -> dump (f, t)));

	return (result);
}

// ---
MCHEmul::MemoryViewDUMP MCHEmul::MemoryView::dumpStructure (bool a)
{
	MCHEmul::MemoryViewDUMP result { _id, { } };

	PhysicalStorageSubsetsList ssR;
	for (size_t i = 0; i < _memPositions.size (); i++)
	{
		PhysicalStorageSubsetsList ss = _memPositions [i]._storages;
		for (const auto& j : ss)
		{
			if (std::find (ssR.begin (), ssR.end (), j) == ssR.end () &&
				((!a && j -> active ()) || (a))) // If it wasn't visited before and has been requested to be included...
			{
				result._data.emplace_back 
					(MCHEmul::PhysicalStorageSubsetDUMP { 
						j -> id (),
						j -> name (),
						j -> canBeWriten (false), // if can be written with no forcing...it is a RAM
						j -> active (),
						j -> activeForReading (),
						j -> initialAddress (),
						j -> lastAddress (),
						{ }, // With no bytes included in this case...
					 });

				ssR.push_back (j);
			}
		}
	}

	return (result);
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
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	MCHEmul::InfoStructure mems;
	for (const auto& i : _subsets)
		mems.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	result.add ("STORAGES", std::move (mems));

	return (result);
}

// ---
void MCHEmul::MemoryView::plainMemory ()
{
	_minAddress = _maxAddress = MCHEmul::Address ();
	_numPositions = 0;
	_memPositions = { };
	
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
MCHEmul::InfoStructure MCHEmul::MemoryDUMP::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("CLASSNAME", std::string ("MemoryDUMP"));
	result.add ("ID",_id);

	int ct = 0;
	MCHEmul::InfoStructure dt;
	for (const auto& i : _data)
		dt.add (MCHEmul::fixLenStr (std::to_string (ct++), 5, true, MCHEmul::_CEROS), // To have it ordered...
			std::move (i.getInfoStructure ()));
	result.add ("DATA", std::move (dt));

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
MCHEmul::Memory::Memory (int id, const MCHEmul::Memory::Content& cnt, const MCHEmul::Attributes& attrs, size_t ts)
	: MCHEmul::MotherboardElement (id, "Memory", attrs),
	  _content (),
	  _additionalSubsets (),
	  _tracker (ts),
	  _deepDebugFile (nullptr),
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

// ---
bool MCHEmul::Memory::addAdditionalSubsets (int id, const MCHEmul::PhysicalStorageSubsets& ss, MCHEmul::MemoryView* v)
{
	bool result = false;

	MCHEmul::Memory::AdditionalSubsets::const_iterator i = _additionalSubsets.find (id);
	if (i == _additionalSubsets.end ())
	{
		_additionalSubsets.insert (MCHEmul::Memory::AdditionalSubsets::value_type (id, ss));

		result = ((v == nullptr) ? _activeView : v) -> addSubSets (ss);
	}

	return (result);
}

// ---
bool MCHEmul::Memory::removeAdditionalSubsets (int id, MCHEmul::MemoryView* v)
{
	bool result = false;

	MCHEmul::Memory::AdditionalSubsets::const_iterator i = _additionalSubsets.find (id);
	if (i != _additionalSubsets.end ())
	{
		std::vector <int> vId;
		for (const auto& j : (*i).second) vId.emplace_back (j.second -> id ());
		result = ((v == nullptr) ? _activeView : v) -> removeSubsets (vId);

		_additionalSubsets.erase (i);
	}

	return (result);
}

// ---
MCHEmul::MemoryDUMP MCHEmul::Memory::dump (const MCHEmul::Address& f, const MCHEmul::Address& t) const
{
	MCHEmul::MemoryDUMP result { _id, { } };

	for (const auto& i : _content._views)
		result._data.emplace_back (std::move (i.second -> dump (f, t)));

	return (result);
}

// ---
MCHEmul::MemoryDUMP MCHEmul::Memory::dumpStructure (bool a)
{
	MCHEmul::MemoryDUMP result { _id, { } };

	for (const auto& i : _content._views)
		result._data.emplace_back (std::move (i.second -> dumpStructure (a)));

	return (result);
}

// ---
void MCHEmul::Memory::setDeepDebugFile (MCHEmul::DebugFile* dF, const std::vector <int>& mId)
{
	if ((_deepDebugFile = dF) == nullptr)
	{
		for (const auto& i : _content._subsets)
			i.second -> setDeepDebugFile (nullptr);
	}
	else
	{
		if (!mId.empty ())
		{
			if (mId [0] == -1) // All?
			{
				for (const auto& i : _content._subsets)
					i.second -> setDeepDebugFile (_deepDebugFile);
			}
			else // No, just the selected ones....
			{
				for (const auto& i : mId)
					if (_content.existsSubset (i))
						_content.subset (i) -> setDeepDebugFile (_deepDebugFile);
			}
		}
	}
}
