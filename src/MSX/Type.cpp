#include <MSX/Type.hpp>
#include <MSX/Memory.hpp>
#include <MSX/Screen.hpp>
#include <MSX/Sound.hpp>
#include <MSX/OSIO.hpp>
#include <TEXASINST/incs.hpp>

// ---
MSX::MSXModel::~MSXModel ()
{
	delete (_vdp);

	_vdp = nullptr;
}

// ---
MSX::Memory* MSX::MSXModel::memory (unsigned int cfg, const std::string& lang)
{ 
	return ((_memory == nullptr) 
		? (_memory = new MSX::Memory (this, cfg, lang)) : _memory); 
}

// ---
MCHEmul::Chips MSX::MSXModel::createChips () const
{
	MCHEmul::Chips result;

	// Add the graphical chip...
	const VDP* vcd = vdp (); // It doesn't own ot...
	result.insert (MCHEmul::Chips::value_type (vcd -> id (), (MCHEmul::Chip*) vcd -> chip ()));

	return (result);
}

// ---
MCHEmul::IODevices MSX::MSXModel::createIODevices () const
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// The Screen...
	result.insert (MCHEmul::IODevices::value_type (MSX::Screen::_ID, 
		(MCHEmul::IODevice*) new MSX::Screen 
			(screenFrequency (), 
			 vdp () -> chip () -> numberColumns (), 
			 vdp () -> chip () -> numberRows (),
			 const_cast <MSX::VDP*> (vdp ()), // To avoid the cast...
			 { 
				 {"Name", "Screen" }, 
				{ "Type", "Output" }, 
				{ "Frequency", std::to_string (screenFrequency ()) 
			 } }))); // The data depends on the the screen frequency...
	// The Sound System...
	result.insert (MCHEmul::IODevices::value_type (MSX::SoundSystem::_ID,
		(MCHEmul::IODevice*) new MSX::SoundSystem ()));
	// ...and the IO system...
	result.insert (MCHEmul::IODevices::value_type (MSX::InputOSSystem::_ID, new MSX::InputOSSystem));

	return (result);
}

// ---
MCHEmul::Attributes MSX::MSXStdModel::attributes () const
{
	MCHEmul::Attributes attrs 
		({ 
			{ "Name", "MSX Standard" },
			{ "Year", "1980" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::MSXStdModel::createVDP ()
{
	return (new MSX::VDPOverTexasInstruments99XXFamily 
		(new TEXASINSTRUMENTS::TMS9929A_PAL (clockSpeed ())));
}

// ---
MCHEmul::Chips MSX::MSXStdModel::createChips () const
{
	MCHEmul::Chips result = std::move (MSX::MSXModel::createChips ());

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices MSX::MSXStdModel::createIODevices () const
{
	MCHEmul::IODevices result = std::move (MSX::MSXModel::createIODevices ());

	// TODO

	return (result);
}

//---
MCHEmul::Memory::Content MSX::MSXStdModel::memoryContent () const
{
	// TODO

	return (MCHEmul::Memory::Content ());
}

// ---
bool MSX::MSXStdModel::loadROMOverForLanguage (MCHEmul::PhysicalStorage* fs,
	const std::string& lang)
{
	// TODO

	return (false);
}

// ---
MCHEmul::Attributes MSX::SVI728::attributes () const
{
	MCHEmul::Attributes attrs 
		({ 
			{ "Name", "SVI Spectravideo 728" },
			{ "Manufacturer", "Spectravideo" },
			{ "Year", "1985" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::SVI728::createVDP ()
{
	return (new MSX::VDPOverTexasInstruments99XXFamily 
		((visualSystem () == MSX::MSXModel::VisualSystem::_PAL) 
			? (TEXASINSTRUMENTS::TMS99XX*) new TEXASINSTRUMENTS::TMS9929A_PAL (clockSpeed ())
			: (TEXASINSTRUMENTS::TMS99XX*) new TEXASINSTRUMENTS::TMS9928A_NTSC (clockSpeed ())));
}

// ---
MCHEmul::Chips MSX::SVI728::createChips () const
{
	MCHEmul::Chips result = std::move (MSX::MSXModel::createChips ());

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices MSX::SVI728::createIODevices () const
{
	MCHEmul::IODevices result = std::move (MSX::MSXModel::createIODevices ());

	return (result);
}

// ---
MCHEmul::Memory::Content MSX::SVI728::memoryContent () const
{
	// TODO

	return (MCHEmul::Memory::Content ());
}

// ---
bool MSX::SVI728::loadROMOverForLanguage (MCHEmul::PhysicalStorage* ss,
	const std::string& lang)
{
	bool result = true;

	// Select the version of the ROM to load....
	std::string ROMFILE = "./bios/svi728_basic-bios_ENG.rom";
	if (lang == "ENG")		ROMFILE = "./bios/svi728_basic-bios_ENG.rom";
	else if (lang == "ESP")	ROMFILE = "./bios/svi728_basic-bios_ESP.rom";
	else result = false;

	result &= ss -> loadInto (ROMFILE);

	return (result);
}

// ---
MCHEmul::Attributes MSX::SVI738::attributes () const
{
	MCHEmul::Attributes attrs 
		({ 
			{ "Name", "SVI Spectravideo 738" },
			{ "Manufacturer", "Spectravideo" },
			{ "Year", "1985" } });

	return (attrs);
}

// ---
MSX::VDP* MSX::SVI738::createVDP ()
{
	return (new MSX::VDPOverTexasInstruments99XXFamily 
		((visualSystem () == MSX::MSXModel::VisualSystem::_PAL) 
			? (TEXASINSTRUMENTS::TMS99XX*) new TEXASINSTRUMENTS::TMS9929A_PAL (clockSpeed ())
			: (TEXASINSTRUMENTS::TMS99XX*) new TEXASINSTRUMENTS::TMS9928A_NTSC (clockSpeed ())));
}

// ---
MCHEmul::Chips MSX::SVI738::createChips () const
{
	MCHEmul::Chips result = std::move (MSX::MSXModel::createChips ());

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices MSX::SVI738::createIODevices () const
{
	MCHEmul::IODevices result = std::move (MSX::MSXModel::createIODevices ());

	return (result);
}

// ---
MCHEmul::Memory::Content MSX::SVI738::memoryContent () const
{
	// TODO

	return (MCHEmul::Memory::Content ());
}

// ---
bool MSX::SVI738::loadROMOverForLanguage (MCHEmul::PhysicalStorage* ss,
	const std::string& lang)
{
	bool result = true;

	// Select the version of the ROM to load....
	std::string ROMFILE = "./svi738_basic-bios_ENG.rom";
	if (lang == "ENG")		ROMFILE = "./bios/svi738_basic-bios_ENG.rom";
	else if (lang == "DEU")	ROMFILE = "./bios/svi738_basic-bios_DEU.rom";
	else if (lang == "SEW") ROMFILE = "./bios/svi738_basic-bios_SEW.rom";
	else result = false;

	result &= ss -> loadInto (ROMFILE);

	return (result);
}
