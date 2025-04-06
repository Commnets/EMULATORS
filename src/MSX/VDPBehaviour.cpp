#include <MSX/VDPBehaviour.hpp>

// ---
MSX::VDPBehaviour::VDPBehaviour (size_t s, const MCHEmul::Attributes& attrs)
	: MCHEmul::InfoClass ("VDPBehaviour"),
	  _videoMemory (s, MCHEmul::UByte::_0),
	  _attributes (attrs)
{ 
	// The rest of the attributes are initialized in the next method...

	initializeInternalValues ();
}

// ---
void MSX::VDPBehaviour::initialize ()
{
	initializeInternalValues ();
}

// ---
MCHEmul::UByte MSX::VDPBehaviour::readPortValue (unsigned char prtId) const
{
	// TODO

	return (MCHEmul::UByte::_0);
}

// ---
void MSX::VDPBehaviour::setPortValue (unsigned char prtId, const MCHEmul::UByte& v)
{
	// TODO
}

// ---
MCHEmul::InfoStructure MSX::VDPBehaviour::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("Attributes",	std::move (_attributes));
	// TODO
	result.add ("MemSize",		std::to_string (_videoMemory.size ()));
	result.add ("BYTES",		_videoMemory);

	return (result);
}

// ---
void MSX::VDPBehaviour::initializeInternalValues ()
{
	_register0			= MCHEmul::UByte::_0;
	_register1			= MCHEmul::UByte::_0;
	_nameAddress		= MCHEmul::Address (2, 0);
	_colorAddress		= MCHEmul::Address (2, 0);
	_patternAddress		= MCHEmul::Address (2, 0);
	_spriteAddress		= MCHEmul::Address (2, 0);
	_spriteAttrsAddress = MCHEmul::Address (2, 0);
	_register7			= MCHEmul::UByte::_0;

	// Iniatialize the video memory...
	for (size_t i = 0; i < _videoMemory.size (); 
		_videoMemory [i++] = MCHEmul::UByte::_0);
}

// ---
MCHEmul::UBytes MSX::VDPBehaviour::screenMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}

// ---
MCHEmul::UBytes MSX::VDPBehaviour::colorMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}

// ---
MSX::VDPBehaviourMSX2::VDPBehaviourMSX2 ()
	: VDPBehaviour (0x20000,
		{ { "Name", "V9938/V9939" },
		  { "Manufacturer", "Yamaha" },
		  { "Year", "1985" } })
{
	initializeInternalValues ();
}

// ---
MCHEmul::UByte MSX::VDPBehaviourMSX2::readPortValue (unsigned char prtId) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	switch (prtId)
	{
		// TODO

		default:
			{
				result = MSX::VDPBehaviour::readPortValue (prtId);
			}

			break;
	}

	return (result);
}

// ---
void MSX::VDPBehaviourMSX2::setPortValue (unsigned char prtId, const MCHEmul::UByte& v)
{
	switch (prtId)
	{
		// TODO

		default:
			{
				MSX::VDPBehaviour::setPortValue (prtId, v);
			}

			break;
	}
}

// ---
MCHEmul::InfoStructure MSX::VDPBehaviourMSX2::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (VDPBehaviour::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void MSX::VDPBehaviourMSX2::initializeInternalValues ()
{
	// TODO
}

// ---
MCHEmul::UBytes MSX::VDPBehaviourMSX2::screenMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}

// ---
MCHEmul::UBytes MSX::VDPBehaviourMSX2::colorMemorySnapShot () const
{
	// TODO

	return (MCHEmul::UBytes ());
}
