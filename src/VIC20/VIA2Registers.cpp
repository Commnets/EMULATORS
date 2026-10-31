#include <VIC20/VIA2Registers.hpp>

// ---
VIC20::VIA2Registers::VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA2_SUBSET, ps, pp, a, s)
{ 
	setClassName ("VIA2Registers");

	initializeInternalValues ();
}

// ---
void VIC20::VIA2Registers::refreshKeyboardInputs () const
{
	// Start from independent sources, not a previous keyboard result or an input latch.
	MCHEmul::UByte externalPA = MCHEmul::UByte::_FF;
	MCHEmul::UByte externalPB = _joystickStatus | 0x7f;

	// Every changing iteration clears at least one of the sixteen level bits.
	// These are settling computations, not additional emulated clock cycles.
	for (unsigned int iteration = 0; iteration < 16; ++iteration)
	{
		MCHEmul::UByte pinsA = _PA -> resolvedValue (externalPA);
		MCHEmul::UByte pinsB = _PB -> resolvedValue (externalPB);
		MCHEmul::UByte nextPA = externalPA, nextPB = externalPB;

		for (size_t column = 0; column < 8; ++column)
			if (!pinsB.bit (column))
				nextPA &= _keyboardStatusMatrix [column];
		for (size_t row = 0; row < 8; ++row)
			if (!pinsA.bit (row))
				nextPB &= _rev_keyboardStatusMatrix [row];

		if (nextPA == externalPA && nextPB == externalPB)
			break;

		externalPA = nextPA;
		externalPB = nextPB;
	}

	// Publish only after both sides have been resolved.
	_PA -> setExternalPins (externalPA);
	_PB -> setExternalPins (externalPB);
}

// ---
const MCHEmul::UByte& VIC20::VIA2Registers::readValue (size_t p) const
{
	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return (_lastValueRead);

	size_t pp = p % 0x10;
	if (pp == 0x00 || pp == 0x01 || pp == 0x0f)
		refreshKeyboardInputs ();

	return (COMMODORE::VIARegisters::readValue (pp));
}

// ---
const MCHEmul::UByte& VIC20::VIA2Registers::peekValue (size_t p) const
{
	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return (_lastValueRead);

	size_t pp = p % 0x10;
	if (pp == 0x00 || pp == 0x01 || pp == 0x0f)
	{
		refreshKeyboardInputs ();

		return (_lastValueRead = (pp == 0x00)
			? _PB -> value (false)
			: _PA -> value (false));
	}

	return (COMMODORE::VIARegisters::peekValue (pp));
}

// ---
void VIC20::VIA2Registers::initializeInternalValues ()
{
	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; // No keys pressed...

	COMMODORE::VIARegisters::initializeInternalValues ();

	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return;

	// VIC-20 VIA2 keyboard defaults:
	// PB selects keyboard columns, PA reads keyboard rows.
	setValue (0x02, MCHEmul::UByte::_FF);	// DDRB: PB0..PB7 output
	setValue (0x03, MCHEmul::UByte::_0);	// DDRA: PA0..PA7 input
	setValue (0x00, MCHEmul::UByte::_FF);   // ORB: columns high
	setValue (0x01, MCHEmul::UByte::_0);    // ORA

	refreshKeyboardInputs ();
}
