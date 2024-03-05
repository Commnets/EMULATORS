#include <F6500/IO6510PortRegisters.hpp>

// ---
F6500::IO6510PortRegisters::IO6510PortRegisters 
		(int id, MCHEmul::PhysicalStorage* ps, const MCHEmul::UByte& bU)
	: MCHEmul::ChipRegisters (id, ps, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 2),
	  _mask (~bU), // The bits not used will be 1...
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	// Must be RAM...
	assert (ps -> type () == MCHEmul::PhysicalStorage::Type::_RAM);

	setClassName ("IO6510PortRegisters");

	initializeInternalValues ();
}

// ---
void F6500::IO6510PortRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void F6500::IO6510PortRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	unsigned char pp = p % 2;
	
	MCHEmul::ChipRegisters::setValue (pp, v);

	switch (pp)
	{
		// Data direction of the port
		case 0x00 :
			{
				_dirValue = v | _mask; 
			}
			
			break;

		// Port value
		case 0x01:
			{
				MCHEmul::UByte oPV = _portValue;
				_outputValue = (v | _mask); // It is kept in the ocuput register...
				
				/** truth table. \n
										in			out \n
					_dirValue	_v		_portValue	_portvalue \n
					0			0		0			0 \n
					0			0		1			1 \n
					0			1		0			0 \n
					0			1		1			1 \n
					1			0		0			0 \n
					1			0		1			0 \n
					1			1		0			1 \n
					1			1		1			1 */
				// But what is in the port can be different, 
				// attending to the way the data direction port is configured
				_portValue = (~_dirValue & _portValue) | (_dirValue & _outputValue);
				// If _dirValue.bit = 0 (read), nothing changes...
				// But with 1, the value changes to what is define din the _outputValue (register)
				
				// Notify only potential changes in the value of the port...
				notifyPortChanges (oPV ^ _portValue, _portValue);
			}

			break;
			
		default:
			// It should be here...
			assert (false);
			break;
	}
}

// ---
const MCHEmul::UByte& F6500::IO6510PortRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::ChipRegisters::readValue (p);

	unsigned char pp = p % 2;

	switch (p)
	{
		case 0x00:
			{
				// Nothing else apart of getting the data is done...
			}
			
			break;

		case 0x01:
			{
				result = _portValue;
			}

			break;
			
		default:
			// It shouldn't be here...
			assert (false);
			break;
	}

	return (_lastValueRead = result);
}
