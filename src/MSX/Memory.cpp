#include <MSX/Memory.hpp>

MSX::SubSlotRegisters* MSX::SubSlotRegisters::_instance = nullptr;

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
MSX::EmptyPhysicalStorageLastBankSubset::EmptyPhysicalStorageLastBankSubset (int id, const MCHEmul::UByte& fV, 
		MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
		const MCHEmul::Stack::Configuration& cfg)
	: MCHEmul::Stack (id, ps, pp, iA, s, cfg),
	  _fixValue (fV),
	  _slotNumber (0) // Assigned later...
{
	unsigned char sslot, bank;
	MSX::Memory::getSlotSubSlotAndBankForMemoryElement (id, _slotNumber, sslot, bank);
}

// ---
void MSX::EmptyPhysicalStorageLastBankSubset::setValue (size_t nB, const MCHEmul::UByte& v)
{
	if (nB == 0x3fff) // The last register in the bank? (== 0xffff)
		MSX::SubSlotRegisters::instance () -> setSubSlotRegister (_slotNumber, v);

	// In other case, does nothing...
}

// ---
const MCHEmul::UByte& MSX::EmptyPhysicalStorageLastBankSubset::readValue (size_t nB) const
{
	return (_lastValueRead = 
		((nB == 0x3fff) // The last register in the bank? (== 0xffff)
			? MSX::SubSlotRegisters::instance () -> 
				subSlotRegister (_slotNumber).complement ()
		    // The value returned is the complement to be able to distinguish whether it is or not this type or other!
			: MCHEmul::Stack::readValue (nB)));
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
	MSX::SubSlotRegisters::instance (); // Just to create it...

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
	if (_ROM == nullptr || _BASICRAM == nullptr || _EMPTYBASICRAM == nullptr)
		{ _error = MCHEmul::_INIT_ERROR; return; }	
	// Create the internal variable _memoryElements, but it must follows the structure defined!
	if (!createBanks ())
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
MCHEmul::PhysicalStorageSubset* MSX::Memory::activeMemoryElementInSlotSubSlotAndBank 
	(unsigned char slot, unsigned char sslot, unsigned char bank)
{
	MCHEmul::PhysicalStorageSubset* result = nullptr;

	for (size_t i = 0; i < 4; i++)
	for (size_t j = 0; j < 4; j++)
	{
		bool oneE = (_memoryElements [i][j][bank].size () == 1); // Many elements?
		for (const auto& k : _memoryElements [i][j][bank]) // A maximum of 2...
		{
			bool eps = (dynamic_cast <MSX::EmptyPhysicalStorageSubset*> (k) != nullptr); // Is it different than Empty?
			bool a = 
				// If there is only one element...
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
void MSX::Memory::activeteSlotsPerBank (unsigned char sb0, unsigned char sb1, unsigned char sb2, unsigned sb3)
{
	unsigned char bStack = (unsigned char) 
		(stack () -> initialAddress ().value () / 0x4000); // In which bank is the stack now?

	// Active the different slots in the different banks....
	// The subslot to consider is the one defined in the subslot registers of each slot...
	// By default these subslots will be the 0 in each of the slots (no extension admitted)....
	MCHEmul::PhysicalStorageSubset* nSS [4] = { };
	MCHEmul::Stack* nStack = nullptr;
	nSS [0] = activeMemoryElementInSlotSubSlotAndBank (_slotSubSlotActive [0]._slot = sb0, // Slot to be active in the bank 0...
		_slotSubSlotActive [0]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (sb0).value () & 0x03), 0);
	nSS [1] = activeMemoryElementInSlotSubSlotAndBank (_slotSubSlotActive [1]._slot = sb1, // Slot to be active in the bank 1...
		_slotSubSlotActive [1]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (sb1).value () & 0xc0) >> 2, 1);
	nSS [2] = activeMemoryElementInSlotSubSlotAndBank (_slotSubSlotActive [2]._slot = sb2, // Slot to be active in the bank 2...
		_slotSubSlotActive [2]._subSlot = (MSX::SubSlotRegisters::instance () -> subSlotRegister (sb2).value () & 0x30) >> 4, 2);
	nSS [3] = activeMemoryElementInSlotSubSlotAndBank (_slotSubSlotActive [3]._slot = sb3, // Slot to be active in the bank 3...
		_slotSubSlotActive [3]._subSlot= (MSX::SubSlotRegisters::instance () -> subSlotRegister (sb3).value () & 0x03) >> 6, 3);

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

	// slot, subslot active pero bank = 0...
	for (size_t i = 0; i < 4; _slotSubSlotActive [i++] = { 0, 0 })

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
bool MSX::Memory::createBanks ()
{
	// Organize the elements created by bank
	// See the note in the header file of the Memory class...
	// To organize them, the id of the subset is used...
	// @see Memory class header...
	for (const auto& i : view (_CPU_VIEW) -> subsets ())
	{
		unsigned char slot, sslot, bank;
		getSlotSubSlotAndBankForMemoryElement (i.first, slot, sslot, bank);

		// Very block is 16k wide, but the element to insert can be in many banks, depending on the size...
		// For instance the ROM of the BIOS is 32k, so it will be in 2 banks...
		for (size_t j = 0; j < (i.second -> size () / 0x4000); j++)
			_memoryElements [(size_t) slot][(size_t) sslot][(size_t) bank + j].emplace_back (i.second);
	}

	// There must be only a maximum of two elements per slot, subslot and bank...
	// ...and one of them and only one of them has to be a EmptyPhysicalStorageStack (or inheriting from it)
	// The other has to be a RAM or ROM reflecting on top another element like a disk or a cartridge....
	// When adding a new element to the memory this method has to be executed again!
	bool result = true;
	for (size_t i = 0; i < 4 && result; i++) // Slot
	for (size_t j = 0; j < 4 && result; j++) // Subslot
	for (size_t k = 0; k < 4 && result; k++) // Bank
	{
		unsigned cnt = 0;
		for (const auto& l : _memoryElements [i][j][k])
			if (dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (l) != nullptr) cnt++;
		result = (_memoryElements [i][j][k].size () == 1) ||
				 (_memoryElements [i][j][k].size () == 2 && cnt == 1);
	}

	return (result);
}
