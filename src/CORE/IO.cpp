#include <CORE/IO.hpp>
#include <CORE/global.hpp>
#include <CORE/FmterBuilder.hpp>
#include <CORE/Formatter.hpp>
#include <SDL.h>

std::shared_ptr <MCHEmul::IODeviceSystem> MCHEmul::IODeviceSystem::_theSystem = 
	std::shared_ptr <MCHEmul::IODeviceSystem> (new MCHEmul::IODeviceSystem);

// ---
MCHEmul::IODevice::IODevice (MCHEmul::IODevice::Type t, int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::MotherboardElement (id, "IODevice", attrs),
	  _type (t), 
	  _chips (), // None by default...
	  _peripherals (), // None by default...
	  _error (MCHEmul::_NOERROR)
{
	MCHEmul::IODeviceSystem::system () -> add (this);
}

// ---
MCHEmul::IODevice::~IODevice ()
{
	MCHEmul::IODeviceSystem::system () -> remove (_id);

	disconnectAllPeripherals ();
}

// ---		
bool MCHEmul::IODevice::connectPeripheral (MCHEmul::IOPeripheral* p)
{
	if (p == nullptr)
		return (true); // Possible, but nothing is done...

	// Only when peripheral with the same id connected...
	bool result = true;
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (p -> id ());
	if (i == _peripherals.end ())
	{
		_peripherals.insert (MCHEmul::IOPeripherals::value_type (p -> id (), p));

		p -> _device = this; // link it...

		// The peripheral is initialized and has to be ok!
		if (!p -> initialize ())
		{
			_error = MCHEmul::_INIT_ERROR;

			result = false;

			// Unplugged...
			disconnectPeripheral (p -> id ());
		}
	}
	// ...otherwise an error is generated...
	else
	{
		_error = MCHEmul::_PERIPHERAL_ERROR;

		result = false;
	}

	return (result);
}

// ---
bool MCHEmul::IODevice::disconnectPeripheral (int id)
{
	bool result = true;
	MCHEmul::IOPeripherals::const_iterator i = _peripherals.find (id);
	if (i != _peripherals.end ())
	{
		if (!(*i).second -> finalize ())
		{
			_error = MCHEmul::_PERIPHERAL_ERROR;

			result = false;
		}

		(*i).second -> setDeepDebugFile (nullptr); // No deep debug file assigned, just in case...

		(*i).second -> _device = nullptr; // No device to be linked to...

		_peripherals.erase (i); // unlink it...
	}

	return (result);
}

// ---
bool MCHEmul::IODevice::disconnectAllPeripherals ()
{
	std::vector <int> prhs;
	for (const auto& i : _peripherals)
		prhs.emplace_back (i.first);

	bool result = true;
	for (int i : prhs)
		result &= disconnectPeripheral (i);
	return (result);
}

// ---
bool MCHEmul::IODevice::initialize ()
{
	if (!MCHEmul::MotherboardElement::initialize ())
		return (false);

	bool result = true;
	for (const auto& i : _peripherals)
		result &= i.second -> initialize ();

	if (!result)
		_error = MCHEmul::_INIT_ERROR;

	return (result);
}

// ---
bool MCHEmul::IODevice::simulate (MCHEmul::CPU* cpu)
{
	bool result = true;
	for (const auto& i : _peripherals)
		result &= i.second -> simulate (cpu);

	if (!result)
		_error = MCHEmul::_DEVICE_ERROR;

	return (result);
}

// ---
MCHEmul::InfoStructure MCHEmul::IODevice::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::MotherboardElement::getInfoStructure ());

	result.add ("ID",		_id);
	result.add ("ATTRS",	_attributes);

	MCHEmul::InfoStructure pers;
	for (const auto& i : _peripherals)
		pers.add (std::to_string (i.first), std::move (i.second -> getInfoStructure ()));
	result.add ("IOPERIPHERALS", std::move (pers));

	return (result);
}

// ---
void MCHEmul::IODevice::setDeepDebugFile (MCHEmul::DebugFile* dF)
{ 
	_deepDebugFile = dF;
	for (const auto& i : _peripherals) // It is transmited to the peripherals connected!...
		i.second -> setDeepDebugFile (dF);
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
