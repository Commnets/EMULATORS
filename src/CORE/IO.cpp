#include <CORE/IO.hpp>
#include <CORE/global.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <SDL.h>

std::shared_ptr <MCHEmul::IODeviceSystem> MCHEmul::IODeviceSystem::_theSystem = 
	std::shared_ptr <MCHEmul::IODeviceSystem> (new MCHEmul::IODeviceSystem);

// ---
MCHEmul::IODevice::IODevice (MCHEmul::IODevice::Type t, int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::InfoClass ("IODevice"),
	  _type (t), 
	  _id (id), 
	  _chips (), 
	  _attributes (),
	  _peripherals (),
	  _error (MCHEmul::_NOERROR)
{
	MCHEmul::IODeviceSystem::system () -> add (this);
}

// ---
MCHEmul::IODevice::~IODevice ()
{
	MCHEmul::IODeviceSystem::system () -> remove (_id);
}

// ---		
void MCHEmul::IODevice::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	if (p == nullptr)
		return;

	// Only when peripheral with the same id connected...
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (p -> id ());
	if (i == _peripherals.end ())
	{
		_peripherals.insert (MCHEmul::IOPeripherals::value_type (p -> id (), p));

		p -> _device = this; // link it...
	}
}

// ---
void MCHEmul::IODevice::disconnectPeripheral (int id)
{
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (id);
	if (i != _peripherals.end ())
	{
		(*i).second -> _device = nullptr;

		_peripherals.erase (i); // unlink it...
	}
}

// ---
bool MCHEmul::IODevice::initialize ()
{
	bool result = true;
	for (const auto& i : _peripherals)
		result &= i.second -> initialize ();

	if (!result)
		_error = MCHEmul::_INIT_ERROR;

	return (result);
}

// ---
bool MCHEmul::IODevice::simulate ()
{
	bool result = true;
	for (const auto& i : _peripherals)
		result &= i.second -> simulate ();

	if (!result)
		_error = MCHEmul::_DEVICE_ERROR;

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::IODevice::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("ID", _id);
	result.add ("ATTRS", _attributes);

	MCHEmul::InfoStructure pers;
	for (const auto& i : _peripherals)
		pers.add (std::to_string (i.first), i.second -> getInfoStructure ());
	result.add ("IOPERIPHERALS", pers);

	return (result);
}

// ---
MCHEmul::IODeviceSystem::IODeviceSystem ()
{
	if (_theSystem != nullptr)
		exit (1); // it shouldn't...but just in case...

	if (SDL_Init (SDL_INIT_EVERYTHING))
		exit (1); // the system has to be initialized...
}

// ---
MCHEmul::IODeviceSystem::~IODeviceSystem ()
{
	SDL_Quit ();

	_theSystem = nullptr;
}
