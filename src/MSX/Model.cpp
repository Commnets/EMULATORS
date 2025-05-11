#include <MSX/Model.hpp>
#include <MSX/Memory.hpp>
#include <MSX/Screen.hpp>
#include <MSX/Sound.hpp>
#include <MSX/OSIO.hpp>
#include <MSX/VDP.hpp>
#include <MSX/PPI8255.hpp>
#include <FZ80/INTInterrupt.hpp>

// ---
MSX::MSXModel::~MSXModel ()
{
	delete (_vdp);

	// But deleting VDP the graphical chip is not deleted...
}

// ---
MSX::Memory* MSX::MSXModel::memory (unsigned int cfg, const std::string& lang)
{ 
	// It is created here intead in the .hpp file to avoid recursive include...
	return ((_memory == nullptr) 
		? (_memory = new MSX::Memory (this /** The model. */, cfg, lang)) : _memory);
}

// ---
MCHEmul::Chips MSX::MSXModel::createChips () const
{
	MCHEmul::Chips result;

    // Add the Graphical Chip....
    _vdp = const_cast <MSX::VDP*> (vdp ());
    result.insert (MCHEmul::Chips::value_type (_vdp -> id (), 
		(MCHEmul::Chip*) _vdp -> graphicalChip ()));

	// Add the Sound Chip...
	_psg = const_cast <MSX::PSG*> (psg ());
	result.insert (MCHEmul::Chips::value_type (_psg -> id (),
		(MCHEmul::Chip*) _psg -> soundChip ()));

	// The PPI Chip, that is critical in communications...
	MSX::PPI8255* ppi = new MSX::PPI8255 (nullptr);
	result.insert (MCHEmul::Chips::value_type (ppi -> id (), ppi));

	return (result);
}

// ---
MCHEmul::IODevices MSX::MSXModel::createIODevices (const std::string& lang) const
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// The Screen...
	result.insert (MCHEmul::IODevices::value_type (MSX::Screen::_ID, 
		(MCHEmul::IODevice*) new MSX::Screen 
			(screenFrequency (), 
			 vdp () -> numberColumns (), 
			 vdp () -> numberRows (),
			 const_cast <MSX::VDP*> (vdp ()), // To avoid the const returned...
			 { 
				{"Name", "Screen" }, 
				{ "Type", "Output" }, 
				{ "Frequency", std::to_string (screenFrequency ()) 
			 } }))); // The data depends on the the screen frequency...

	// ...The sound system
	result.insert (MCHEmul::IODevices::value_type (MSX::SoundSystem::_ID,
		(MCHEmul::IODevice*) new MSX::SoundSystem ()));

	// ...and the IO system...
	// with the standard key
	result.insert (MCHEmul::IODevices::value_type 
		(MSX::InputOSSystem::_ID, new MSX::InputOSSystem (keystrockedMap (lang))));

	return (result);
}

// ---
MSX::InputOSSystem::KeystrockesMap MSX::MSXModel::createKeystrockesMap (const std::string& lang) const
{
	// TODO Consider different languages...

	return (MSX::InputOSSystem::KeystrockesMap (
		{
			// Row 0
			{ SDL_SCANCODE_7,				{ std::make_pair (0, 7) } },
			{ SDL_SCANCODE_6,				{ std::make_pair (0, 6) } },
			{ SDL_SCANCODE_5,				{ std::make_pair (0, 5) } },
			{ SDL_SCANCODE_4,				{ std::make_pair (0, 4) } },
			{ SDL_SCANCODE_3,				{ std::make_pair (0, 3) } },
			{ SDL_SCANCODE_2,				{ std::make_pair (0, 2) } },
			{ SDL_SCANCODE_1,				{ std::make_pair (0, 1) } },
			{ SDL_SCANCODE_0,				{ std::make_pair (0, 0) } },
			// Row 1
			{ SDL_SCANCODE_SEMICOLON,		{ std::make_pair (1, 7) } },
			{ SDL_SCANCODE_RIGHTBRACKET,	{ std::make_pair (1, 6) } },
			{ SDL_SCANCODE_LEFTBRACKET,		{ std::make_pair (1, 5) } },
			{ SDL_SCANCODE_BACKSLASH,		{ std::make_pair (1, 4) } },
			{ SDL_SCANCODE_EQUALS,			{ std::make_pair (1, 3) } },
			{ SDL_SCANCODE_MINUS,			{ std::make_pair (1, 2) } },
			{ SDL_SCANCODE_9,				{ std::make_pair (1, 1) } },
			{ SDL_SCANCODE_8,				{ std::make_pair (1, 0) } },
			// Row 2
			{ SDL_SCANCODE_B,				{ std::make_pair (2, 7) } },
			{ SDL_SCANCODE_A,				{ std::make_pair (2, 6) } },
			{ SDL_SCANCODE_LEFTBRACKET,		{ std::make_pair (2, 5) } }, // Pending DEAD
			{ SDL_SCANCODE_SLASH,			{ std::make_pair (2, 4) } },
			{ SDL_SCANCODE_PERIOD,			{ std::make_pair (2, 3) } },
			{ SDL_SCANCODE_COMMA,			{ std::make_pair (2, 2) } },
			{ SDL_SCANCODE_GRAVE,			{ std::make_pair (2, 1) } },
			{ SDL_SCANCODE_APOSTROPHE,		{ std::make_pair (2, 0) } },
			// Row 3
			{ SDL_SCANCODE_J,				{ std::make_pair (3, 7) } },
			{ SDL_SCANCODE_I,				{ std::make_pair (3, 6) } },
			{ SDL_SCANCODE_H,				{ std::make_pair (3, 5) } },
			{ SDL_SCANCODE_G,				{ std::make_pair (3, 4) } },
			{ SDL_SCANCODE_F,				{ std::make_pair (3, 3) } },
			{ SDL_SCANCODE_E,				{ std::make_pair (3, 2) } },
			{ SDL_SCANCODE_D,				{ std::make_pair (3, 1) } },
			{ SDL_SCANCODE_C,				{ std::make_pair (3, 0) } },
			// Row 4
			{ SDL_SCANCODE_R,				{ std::make_pair (4, 7) } },
			{ SDL_SCANCODE_Q,				{ std::make_pair (4, 6) } },
			{ SDL_SCANCODE_P,				{ std::make_pair (4, 5) } },
			{ SDL_SCANCODE_O,				{ std::make_pair (4, 4) } },
			{ SDL_SCANCODE_N,				{ std::make_pair (4, 3) } },
			{ SDL_SCANCODE_M,				{ std::make_pair (4, 2) } },
			{ SDL_SCANCODE_L,				{ std::make_pair (4, 1) } },
			{ SDL_SCANCODE_K,				{ std::make_pair (4, 0) } },
			// Row 5
			{ SDL_SCANCODE_Z,				{ std::make_pair (5, 7) } },
			{ SDL_SCANCODE_Y,				{ std::make_pair (5, 6) } },
			{ SDL_SCANCODE_X,				{ std::make_pair (5, 5) } },
			{ SDL_SCANCODE_W,				{ std::make_pair (5, 4) } },
			{ SDL_SCANCODE_V,				{ std::make_pair (5, 3) } },
			{ SDL_SCANCODE_U,				{ std::make_pair (5, 2) } },
			{ SDL_SCANCODE_T,				{ std::make_pair (5, 1) } },
			{ SDL_SCANCODE_S,				{ std::make_pair (5, 0) } },
			// Row 6
			{ SDL_SCANCODE_F3,				{ std::make_pair (6, 7) } },
			{ SDL_SCANCODE_F2,				{ std::make_pair (6, 6) } },
			{ SDL_SCANCODE_F1,				{ std::make_pair (6, 5) } },
			{ SDL_SCANCODE_LALT,			{ std::make_pair (6, 4) } }, // = CODE
			{ SDL_SCANCODE_V,				{ std::make_pair (6, 3) } }, // Pending CAPS
			{ SDL_SCANCODE_LCTRL,			{ std::make_pair (6, 2) } }, // = GRAPH
			{ SDL_SCANCODE_RCTRL,			{ std::make_pair (6, 1) } }, // = CTRL
			{ SDL_SCANCODE_LSHIFT,			{ std::make_pair (6, 0) } }, // = SHIFT
			{ SDL_SCANCODE_RSHIFT,			{ std::make_pair (6, 0) } }, // = SHIFT
			// Row 7
			{ SDL_SCANCODE_RETURN,			{ std::make_pair (7, 7) } },
			{ SDL_SCANCODE_HOME,			{ std::make_pair (7, 6) } }, // Pending SELECT
			{ SDL_SCANCODE_BACKSPACE,		{ std::make_pair (7, 5) } }, // Backspace
			{ SDL_SCANCODE_END,				{ std::make_pair (7, 4) } }, // = STOP
			{ SDL_SCANCODE_TAB,				{ std::make_pair (7, 3) } },
			{ SDL_SCANCODE_ESCAPE,			{ std::make_pair (7, 2) } },
			{ SDL_SCANCODE_F5,				{ std::make_pair (7, 1) } },
			{ SDL_SCANCODE_F4,				{ std::make_pair (7, 0) } },
			// Row 8
			{ SDL_SCANCODE_RIGHT,			{ std::make_pair (8, 7) } },
			{ SDL_SCANCODE_DOWN,			{ std::make_pair (8, 6) } },
			{ SDL_SCANCODE_UP,				{ std::make_pair (8, 5) } },
			{ SDL_SCANCODE_LEFT,			{ std::make_pair (8, 4) } },
			{ SDL_SCANCODE_DELETE,			{ std::make_pair (8, 3) } },
			{ SDL_SCANCODE_INSERT,			{ std::make_pair (8, 2) } },
			{ SDL_SCANCODE_HOME,			{ std::make_pair (8, 1) } },
			{ SDL_SCANCODE_SPACE,			{ std::make_pair (8, 0) } },
			// Row 9
			{ SDL_SCANCODE_KP_4,			{ std::make_pair (8, 7) } },
			{ SDL_SCANCODE_KP_3,			{ std::make_pair (8, 6) } },
			{ SDL_SCANCODE_KP_2,			{ std::make_pair (8, 5) } },
			{ SDL_SCANCODE_KP_1,			{ std::make_pair (8, 4) } },
			{ SDL_SCANCODE_KP_0,			{ std::make_pair (8, 3) } },
			{ SDL_SCANCODE_KP_DIVIDE,		{ std::make_pair (8, 2) } },
			{ SDL_SCANCODE_KP_PLUS,			{ std::make_pair (8, 1) } },
			{ SDL_SCANCODE_KP_MULTIPLY,		{ std::make_pair (8, 0) } },
			// Row 10
			{ SDL_SCANCODE_KP_PERIOD,		{ std::make_pair (8, 7) } },
			{ SDL_SCANCODE_KP_COMMA,		{ std::make_pair (8, 6) } },
			{ SDL_SCANCODE_KP_MINUS,		{ std::make_pair (8, 5) } },
			{ SDL_SCANCODE_KP_9,			{ std::make_pair (8, 4) } },
			{ SDL_SCANCODE_KP_8,			{ std::make_pair (8, 3) } },
			{ SDL_SCANCODE_KP_7,			{ std::make_pair (8, 2) } },
			{ SDL_SCANCODE_KP_6,			{ std::make_pair (8, 1) } },
			{ SDL_SCANCODE_KP_5,			{ std::make_pair (8, 0) } }
		}));
}

// ---
MCHEmul::Memory::Content MSX::MSXModel::memoryContent () const
{ 
	// There might be up to 4 slots and 4 subslots of 64k each in the memory of the MSX.
	// In that slots different things might connected: A cartridge, a disk, RAM...
	// And only one thing at the same time in each slot and subslot, if any...
	// By default the basic set will consider a empty zone very slot/subslot in every different banks, 
	// with the exception of the banks 0 & 1, slot 0, subslot 0 that is filled with the ROM
	// and the bank 3, slot 0, subslot 0 that is filled with RAM.

	// The physical elements for all the 16 slots possible...
	// ROM is only, initially, at the slot 0, bank 0 & 1.
	MCHEmul::PhysicalStorage* ROM_SLOT0 = new MCHEmul::PhysicalStorage (MSX::Memory::_ROM_SET,
			MCHEmul::PhysicalStorage::Type::_ROM, 0x8000);  // 32k
	std::vector <MCHEmul::PhysicalStorage*> RAM_SLOTS;
	for (int i = 0; i < 16; i++) // 4 slots, 4 subslots each...
		RAM_SLOTS.push_back (new MCHEmul::PhysicalStorage (MSX::Memory::_RAM_SET + i,
			MCHEmul::PhysicalStorage::Type::_RAM, 0x10000)); // 64k each...

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages;
	storages.insert (MCHEmul::PhysicalStorages::value_type (MSX::Memory::_ROM_SET, ROM_SLOT0));
	for (int i = 0; i < 16; i++)
		storages.insert (MCHEmul::PhysicalStorages::value_type 
			(MSX::Memory::_RAM_SET + i, RAM_SLOTS [(size_t) i]));

	// Now the subsets...
	// // The basic subsets in the slot0 and subs slot 0...
	// The BIOS subset is ROM, that is iun the banks 0 & 1 of the slot 0, subslot 0...
	MCHEmul::PhysicalStorageSubset* ROMBIOS =
		new MCHEmul::PhysicalStorageSubset (MSX::Memory::_ROMBIOS_SUBSET, ROM_SLOT0,
			0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x8000); // 32k
	ROMBIOS -> setName ("ROM BIOS, Slot 0, Subslot 0, Bank 0 & 1");
	// An empty subset of 16k in the bank 2 of the slot 0
	MCHEmul::EmptyPhysicalStorageSubset* ERAMB2_SLOT0SUBSLOT0 =
		new MCHEmul::EmptyPhysicalStorageSubset (MSX::Memory::_ERAM16KSLOT0SUBSLOT0_SUBSET, MCHEmul::UByte::_0, RAM_SLOTS [0],
			0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000); // 16k 
	ERAMB2_SLOT0SUBSLOT0 -> setName ("RAM Empty 16k, Slot 0, Subslot 0, Bank 2");
	// The basic subset of 16k in the bank 3 of the slot 0
	MCHEmul::PhysicalStorageSubset* RAMB3_SLOT0SUBSLOT0 = 
		new MCHEmul::Stack (MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET, RAM_SLOTS [0],
			0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000,
				MCHEmul::Stack::Configuration (true, false /** Pointing to the last written. */, 
					false /** No overflow detection. */, -1)); // 16k
	RAMB3_SLOT0SUBSLOT0 -> setName ("RAM 16k, Slot 0, Subslot 0, Bank 3");

	// The rest of the memory is created with empty subsets...
	// It is needed to create 12 subslots (remaining) + 
	// 3 (remaining slots) * 4 (subslots each) * 4 (banks each) = 60 empty subsets of 16k each...
	// The rest of the 16 blocks in any slot and subslot are empty...
	std::vector <MCHEmul::PhysicalStorageSubset*> ERAMB_SLOTSSUBSLOTS;
	for (int i = 4; i < (16 << 2); i++) // 4 slots, 4 subslots each...
	{
		int mId = 
			MSX::Memory::_SLOTSUBSLOTBASE_SUBSET + ((i / 16) * 100) /** From 0 to 3 * 100. */ + 
			(((i % 16) / 4) * 10) + /** From 0 to 3 * 10. */
			(i % 4); /** From 0 to 3. */
		MCHEmul::Address add (2, 0x0000 + ((i % 4) << 14 /** Multiply by 16. */));
		MCHEmul::Stack* empty = 
			(add.value () == 0xc000) 
				? (MCHEmul::Stack*) new MSX::EmptyPhysicalStorageLastBankSubset
					(mId, MCHEmul::UByte::_0, RAM_SLOTS [(size_t) (i / 16)], /** From 0 to 3. */
					 add.value (), add, 0x4000,
					 MCHEmul::Stack::Configuration (true, false /** Pointing to the last written. */, 
						false /** No overflow detection. */, -1)) 
				  // 16k memory but with registers controlling things...
				: (MCHEmul::Stack*) new MSX::EmptyPhysicalStorageSubset
					(mId, MCHEmul::UByte::_0, RAM_SLOTS [(size_t) (i / 16)], /** From 0 to 3. */
					 add.value (), add, 0x4000,
					 MCHEmul::Stack::Configuration (true, false /** Pointing to the last written. */, 
						false /** No overflow detection. */, -1)); 
				  // 16k of no memory!
		ERAMB_SLOTSSUBSLOTS.emplace_back (empty);
		empty -> setName ("RAM Empty 16k, Slot " + std::to_string (i >> 4) + ", Subslot " + std::to_string ((i % 16) / 4) + 
			", Bank " + std::to_string (i % 4));
	}

	// The view from the CPU...
	MCHEmul::PhysicalStorageSubsets allsubsets;
	allsubsets.insert (MCHEmul::PhysicalStorageSubsets::value_type 
		(MSX::Memory::_ROMBIOS_SUBSET, ROMBIOS));
	allsubsets.insert (MCHEmul::PhysicalStorageSubsets::value_type 
		(MSX::Memory::_ERAM16KSLOT0SUBSLOT0_SUBSET, ERAMB2_SLOT0SUBSLOT0));
	allsubsets.insert (MCHEmul::PhysicalStorageSubsets::value_type 
		(MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET, RAMB3_SLOT0SUBSLOT0));
	for (int i = 4; i < (16 << 2); i++)
		allsubsets.insert (MCHEmul::PhysicalStorageSubsets::value_type 
			(MSX::Memory::_SLOTSUBSLOTBASE_SUBSET + ((i / 16) * 100) + (((i % 16) / 4) * 10) + (i % 4), 
				ERAMB_SLOTSSUBSLOTS [(size_t) (i - 4)]));
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView (MSX::Memory::_CPU_VIEW, allsubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{ { MSX::Memory::_CPU_VIEW, cpuView} });

	return (result);
}

// ---
MCHEmul::Attributes MSX::MSXStdModel::attributes () const
{
	MCHEmul::Attributes attrs 
		({
			{ "Manufacturer", "MSX" },
			{ "Generation", "MSX1" },
			{ "Visual System", "PAL" }, // Always PAL...
			{ "Clock Speed", std::to_string (clockSpeed ()) + " Hz" },
			{ "Name", "MSX Standard" },
			{ "Year", "1980" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::MSXStdModel::createVDP () const
{
	// The standard model is always PAL and based on a Texas Instruments basic chip...
	// Notice that VDP doesn't own the graphical chip behind...
	return (new MSX::VDP_TMS99xxFamily
		(new TEXASINSTRUMENTS::TMS9929A 
			(nullptr /** Created internally. */, 3 /** 3 times quicked than CPU = 10,74MHz */, FZ80::INTInterrupt::_ID)));
}

// ---
MSX::PSG* MSX::MSXStdModel::createPSG () const
{
	// The standard model is based on the General Instruments AY-3-8910 chip...
	return (new MSX::PSG_AY38910
		(new GENERALINSTRUMENTS::AY38910 (nullptr)));
}

// ---
bool MSX::MSXStdModel::loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs,
	const std::string& lang)
{
	bool result = true;

	// The standard version is like the SVI728 in English...
	result &= fs -> loadInto ("./bios/svi728_basic-bios_ENG.rom");

	return (result);
}

// ---
void MSX::MSXStdModel::configureMemory (MSX::Memory* m, unsigned int cfg)
{
	assert (m != nullptr);

	// Disactive all elements, but maintain the standard ones...
	m -> desactivateAllMemoryElements (true);
	// ...and only the basic elements are connected...
	m -> connectMemoryElements ({ 
		MSX::Memory::_ROMBIOS_SUBSET, 
		MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET });
	// ...and also fix the stack subset...
	m -> setStackSubset (MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET);
}

// ---
MCHEmul::Attributes MSX::SVI728::attributes () const
{
	MCHEmul::Attributes attrs 
		({ 
			{ "Manufacturer", "SpectraVideo" },
			{ "Generation", "MSX1" },
			{ "Visual System", (visualSystem () == MSX::MSXModel::VisualSystem::_PAL) ? "PAL" : "NTSC" },
			{ "Clock Speed", std::to_string (clockSpeed ()) + " Hz" },
			{ "Name", "MSX SpectraVideo 728" },
			{ "Year", "1980" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::SVI728::createVDP () const
{
	return ((visualSystem () == MSX::MSXModel::VisualSystem::_PAL) 
			? (MSX::VDP*) new MSX::VDP_TMS99xxFamily
				(new TEXASINSTRUMENTS::TMS9929A 
					(nullptr /** Created internally. */, 3 /** 2 times quicker than CPU = 10,74MHz */, 
						FZ80::INTInterrupt::_ID /** INT. */))
			: (MSX::VDP*) new MSX::VDP_TMS99xxFamily 
				(new TEXASINSTRUMENTS::TMS9918A 
					(nullptr /** Created internally. */, 3 /** 2 times quicker than CPU = 10,74MHz */, 
						FZ80::INTInterrupt::_ID /** INT. */)));
}

// ---
MSX::PSG* MSX::SVI728::createPSG () const
{
	// The standard model is based on the General Instruments AY-3-8910 chip...
	return (new MSX::PSG_AY38910
		(new GENERALINSTRUMENTS::AY38910 (nullptr)));
}

// ---
MCHEmul::Chips MSX::SVI728::createChips () const
{
	MCHEmul::Chips result = std::move (MSX::MSXModel::createChips ());

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices MSX::SVI728::createIODevices (const std::string& lang) const
{
	MCHEmul::IODevices result = std::move (MSX::MSXModel::createIODevices (lang));

	// TODO

	return (result);
}

// ---
MSX::InputOSSystem::KeystrockesMap MSX::SVI728::createKeystrockesMap (const std::string& lang) const
{
	MSX::InputOSSystem::KeystrockesMap result = std::move (MSX::MSX1Model::createKeystrockesMap (lang));

	// TODO

	return (result);
}

// ---
bool MSX::SVI728::loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs,
	const std::string& lang)
{
	bool result = true;

	// Select the version of the ROM to load....
	std::string ROMFILE = "./bios/svi728_basic-bios_ENG.rom";
	if (lang == "ENG")		ROMFILE = "./bios/svi728_basic-bios_ENG.rom";
	else if (lang == "ESP")	ROMFILE = "./bios/svi728_basic-bios_ESP.rom";
	else result = false;

	result &= fs -> loadInto (ROMFILE);

	return (result);
}

// ---
void MSX::SVI728::configureMemory (MSX::Memory* m, unsigned int cfg)
{
	assert (m != nullptr);

	// Disactive all elements, but maintain the standard ones...
	m -> desactivateAllMemoryElements (true);
	// ...and only the basic elements are connected...
	m -> connectMemoryElements ({ 
		MSX::Memory::_ROMBIOS_SUBSET, 
		MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET });
	// ...and also fix the stack subset...
	m -> setStackSubset (MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET);
}

// ---
MCHEmul::Attributes MSX::SVI738::attributes () const
{
	MCHEmul::Attributes attrs 
		({ 
			{ "Manufacturer", "SpectraVideo" },
			{ "Generation", "MSX2" },
			{ "Visual System", (visualSystem () == MSX::MSXModel::VisualSystem::_PAL) ? "PAL" : "NTSC" },
			{ "Clock Speed", std::to_string (clockSpeed ()) + " Hz" },
			{ "Name", "MSX SpectraVideo 738" },
			{ "Year", "1982" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::SVI738::createVDP () const
{
	// TODO

	return (nullptr);
}

// ---
MSX::PSG* MSX::SVI738::createPSG () const
{
	// TODO

	return (nullptr);
}

// ---
MCHEmul::Chips MSX::SVI738::createChips () const
{
	MCHEmul::Chips result = std::move (MSX::MSXModel::createChips ());

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices MSX::SVI738::createIODevices (const std::string& lang) const
{
	MCHEmul::IODevices result = std::move (MSX::MSXModel::createIODevices (lang));

	// TODO

	return (result);
}

// ---
MSX::InputOSSystem::KeystrockesMap MSX::SVI738::createKeystrockesMap (const std::string& lang) const
{
	MSX::InputOSSystem::KeystrockesMap result = std::move (MSX::MSX1Model::createKeystrockesMap (lang));

	// TODO

	return (result);
}

// ---
MCHEmul::Memory::Content MSX::SVI738::memoryContent () const
{
	MCHEmul::Memory::Content result = std::move (MSX::MSXModel::memoryContent ());

	// TODO

	return (result);
}

// ---
bool MSX::SVI738::loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs,
	const std::string& lang)
{
	bool result = true;

	// Select the version of the ROM to load....
	std::string ROMFILE = "./svi738_basic-bios_ENG.rom";
	if (lang == "ENG")		ROMFILE = "./bios/svi738_basic-bios_ENG.rom";
	else if (lang == "DEU")	ROMFILE = "./bios/svi738_basic-bios_DEU.rom";
	else if (lang == "SEW") ROMFILE = "./bios/svi738_basic-bios_SEW.rom";
	else result = false;

	result &= fs -> loadInto (ROMFILE);

	return (result);
}

// ---
void MSX::SVI738::configureMemory (MSX::Memory* m, unsigned int cfg)
{
	assert (m != nullptr);

	// Disactive all elements, but maintain the standard ones...
	m -> desactivateAllMemoryElements (true);
	// ...and only the basic elements are connected...
	m -> connectMemoryElements ({ 
		MSX::Memory::_ROMBIOS_SUBSET, 
		MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET });
	// ...and also fix the stack subset...
	m -> setStackSubset (MSX::Memory::_RAM16KSLOT0SUBSLOT0_SUBSET);
}
