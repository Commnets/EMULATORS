#include <MSX/Memory.hpp>

MSX::SubSlotRegisters* MSX::SubSlotRegisters::_instance = nullptr;

// ---
MSX::LastPagePhysicalStorageSubset::LastPagePhysicalStorageSubset 
		(int id,
		 MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
		 const Configuration& cfg)
	: MCHEmul::Stack (id, ps, pp, iA, s, cfg),
	  _slotNumber (0), // Assigned later...
	  _lastValueRead (MCHEmul::UByte::_0)
{
	// The only useful variable is _slotNumber, the other ones are not used...
	unsigned char sslot, page;
	MSX::Memory::getSlotSubSlotAndPageForMemoryElement (id, _slotNumber, sslot, page);

	// The actions in this set are not buffered...
	// ...just to allow the changes in the configuration immediately...
	setBufferMemorySetCommands (false);
}

// ---
void MSX::LastPagePhysicalStorageSubset::setValue (size_t nB, const MCHEmul::UByte& v)
{
	MCHEmul::Stack::setValue (nB, v); // First, store the value normally...
	if (nB == (size () - 1)) // The last register in the page? (usually == 0xffff)
		MSX::SubSlotRegisters::instance () -> setSubSlotRegister (_slotNumber, v);
	// In other case, it does nothing because it is an empty storage!
}

// ---
const MCHEmul::UByte& MSX::LastPagePhysicalStorageSubset::readValue (size_t nB) const
{
	return (_lastValueRead = 
		(nB == (size () - 1)) 
			? MSX::SubSlotRegisters::instance () -> subSlotRegister (_slotNumber) 
			: MCHEmul::Stack::readValue (nB));
}

// ---
MSX::EmptyPhysicalStorageSubset::EmptyPhysicalStorageSubset (int id, const MCHEmul::UByte& fV, 
		MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
		const MCHEmul::Stack::Configuration& cfg)
	: MCHEmul::Stack (id, ps, pp, iA, s, cfg),
	  _fixValue (fV)
{
		// Nothing else...
}

// ---
MSX::EmptyPhysicalStorageLastPageSubset::EmptyPhysicalStorageLastPageSubset 
		(int id, const MCHEmul::UByte& fV,
		 MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
		 const Configuration& cfg)
	: LastPagePhysicalStorageSubset (id, ps, pp, iA, s, cfg),
	  _fixValue (fV)
{
	// Nothing else...
}

// ---
const MCHEmul::UByte& MSX::EmptyPhysicalStorageLastPageSubset::readValue (size_t nB) const
{ 
	return (_lastValueRead = 
		(nB == (size () - 1)) 
			? MSX::SubSlotRegisters::instance () -> subSlotRegister (_slotNumber) 
			: _fixValue);
}

// ---
MSX::Memory::Memory (MSX::MSXModel* m, unsigned int cfg, 
		const std::string& lang)
	: MCHEmul::Memory (0, m -> memoryContent (), { }),
	  _model (m),
	  _configuration (cfg),
	  _ROM (nullptr), _BASICRAM (nullptr), _EMPTYBASICRAM (nullptr),
	  _memoryElements (),
	  _STACK_SUBSET (0), // Initially the version not expanded...
	  _slotSubSlotActive { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } // The default values...
{
	// Just to create it...
	// ...defining the slots expanded or not expanded...
	MSX::SubSlotRegisters::instance (m -> expandedSlots ()); 

	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	if (physicalStorage (_ROM_SET) == nullptr) 
		{ _error = MCHEmul::_INIT_ERROR; return; }
	if (!_model -> loadROMOverForLanguage (physicalStorage (_ROM_SET), lang)) 
		{ _error = MCHEmul::_INIT_ERROR; return; }
	subset (_ROMBIOS_SUBSET) -> fixDefaultValues (); // Fix the values in the ROM...

	// The basic elements...
	_ROM = subset (_ROMBIOS_SUBSET);
	_BASICRAM = subset (_RAM16KSLOT0SUBSLOT0_SUBSET);
	_EMPTYBASICRAM = subset (_ERAM16KSLOT0SUBSLOT0_SUBSET);
	// None of them can be nullptr...
	if (_ROM == nullptr) // The other two can be nullptr, but not the ROM...
		{ _error = MCHEmul::_INIT_ERROR; return; }	
	// Create the internal variable _memoryElements, but it must follows the structure defined!
	if (!createMemoryElementsEntity ())
		{ _error = MCHEmul::_INIT_ERROR; return; }	

	// Finally, sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
MSX::Memory::~Memory ()
{
	MSX::SubSlotRegisters::destroyInstance ();
}

// ---
MCHEmul::PhysicalStorageSubset* MSX::Memory::activeMemoryElementInSlotSubSlotAndPage 
	(unsigned char slot, unsigned char sslot, unsigned char page)
{
	MCHEmul::PhysicalStorageSubset* result = nullptr;

	for (size_t i = 0; i < 4; i++)
	for (size_t j = 0; j < 4; j++)
	{
		bool oneE = (_memoryElements [i][j][page].size () == 1); // Many elements?
		for (const auto& k : _memoryElements [i][j][page]) // A maximum of 2...
		{
			// Is it different than Empty?
			bool eps = (dynamic_cast <MSX::EmptyPhysicalStorageSubset*> (k) != nullptr); 
			bool a = 
				// If there is only one element...
				// and the requested slot/subslot are the current one,
				(oneE && (i == slot && j == sslot)) || 
				// ...or there are 2, the element is the basic one and the counter is not at the right slot/subslot...
				(!oneE && eps && (i != slot || j != sslot) ||
				// ...or there are 2, the element is the not the basic one and the counter is on the right slot/subslot...
				(!oneE && !eps && (i == slot && j == sslot)));
				// Ative...Otherwise No active...

			k -> setActive (a);
			k -> setActiveForReading (a);

			if (a && (i == slot && j == sslot))
				result = k;
		}
	}

	return (result);
}

// ---
void MSX::Memory::activateSlotsPerPage (unsigned char spage0, unsigned char spage1, unsigned char spage2, unsigned spage3)
{
	unsigned char bStack = (unsigned char) 
		(stack () -> initialAddress ().value () / 0x4000); // In which page is the stack now?

	// Active the different slots in the different pages....
	// The subslot to consider is the one defined in the subslot registers of each slot...
	// By default these subslots will be the 0 in each of the slots (no extension admitted)....
	MCHEmul::PhysicalStorageSubset* nSS [4] = { };
	MCHEmul::Stack* nStack = nullptr;
	nSS [0] = activeMemoryElementInSlotSubSlotAndPage (_slotSubSlotActive [0]._slot = spage0, // Slot to be active in the page 0...
		_slotSubSlotActive [0]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (spage0).value () & 0x03), 0);
	nSS [1] = activeMemoryElementInSlotSubSlotAndPage (_slotSubSlotActive [1]._slot = spage1, // Slot to be active in the page 1...
		_slotSubSlotActive [1]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (spage1).value () & 0x0c) >> 2, 1);
	nSS [2] = activeMemoryElementInSlotSubSlotAndPage (_slotSubSlotActive [2]._slot = spage2, // Slot to be active in the page 2...
		_slotSubSlotActive [2]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (spage2).value () & 0x30) >> 4, 2);
	nSS [3] = activeMemoryElementInSlotSubSlotAndPage (_slotSubSlotActive [3]._slot = spage3, // Slot to be active in the page 3...
		_slotSubSlotActive [3]._subSlot= (MSX::SubSlotRegisters::instance () -> subSlotRegister (spage3).value () & 0xc0) >> 6, 3);

	// The memory activated in the place where the stack was defined, must be also of the type stack! 
	nStack = dynamic_cast <MCHEmul::Stack*> (nSS [bStack]);
	// ..and if the stack exists...
	if (nStack != nullptr)
	{
		// Get the position where the stack was (absolute)...
		unsigned int osPos = stack () -> currentAddress ().value (); 
		// Reset the stack...
		resetStack ();
		// ...and this should be the id of the new one...
		_STACK_SUBSET = nStack -> id (); 
		// ...and where a new location based on where the stack is...
		// This command also loads the new stack!
		stack () -> setPosition ((int) osPos, false /** Absolute. */);
	}
	else
	{
		_LOG ("Stack not possible in this configurarion");

		// In debug mode, the system stops... 
		assert (false);
	}
}

// ---
bool MSX::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// slot, subslot active per page = 0...
	for (size_t i = 0; i < 4; _slotSubSlotActive [i++] = { 0, 0 })

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
bool MSX::Memory::createMemoryElementsEntity ()
{
	// Organize the elements created in the memory per slot, subslot and page...
	// See the note in the header file of the Memory class...
	// IMPORTANT: To organize them, the id of the subset is used...
	// @see Memory class header...
	for (const auto& i : view (_CPU_VIEW) -> subsets ())
	{
		unsigned char slot, sslot, page;
		getSlotSubSlotAndPageForMemoryElement (i.first, slot, sslot, page);
		// Every page is 16k wide, 
		// but one memory element can be in many pages simultaneosly (depending on its size)...
		// For instance the BIOS ROM is 32k wide, so it will be in 2 pages...
		for (size_t j = 0; j < (i.second -> size () / 0x4000); j++)
			_memoryElements [(size_t) slot][(size_t) sslot][(size_t) page + j].emplace_back (i.second);
	}

	// There must be only a maximum of two elements per slot, subslot and page...
	// ...and one of them and only one of them has to be a EmptyPhysicalStorageStack (or inheriting from it)
	// The other has to be either RAM or ROM reflecting on top another element like a disk or a cartridge....
	// When adding a new element to the memory this method has to be executed again!
	bool result = true;
	for (size_t i = 0; i < 4 && result; i++) // Slot
	for (size_t j = 0; j < 4 && result; j++) // Subslot
	for (size_t k = 0; k < 4 && result; k++) // Page
	{
		unsigned cnt = 0;
		for (const auto& l : _memoryElements [i][j][k])
			if (dynamic_cast <MSX::EmptyPhysicalStorageSubset*> (l) != nullptr ||
				dynamic_cast <MSX::EmptyPhysicalStorageLastPageSubset*> (l) != nullptr) cnt++;
		result = (_memoryElements [i][j][k].size () == 1) ||
				 (_memoryElements [i][j][k].size () == 2 && cnt == 1);
	}

	return (result);
}
