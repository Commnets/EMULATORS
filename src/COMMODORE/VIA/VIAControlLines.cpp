#include <COMMODORE/VIA/VIAControlLines.hpp>
#include <COMMODORE/VIA/VIAPort.hpp>

COMMODORE::VIAControlLine::VIAControlLine (int id)
	: MCHEmul::InfoClass ("VIAControlLine"),
	  _wire (id),
	  _mode (0), // Not defined yet...
	  _interruptEnabled (false),
	  _P (nullptr), _CL (nullptr),
	  _interruptRequested (false)
{ 
	initialize (); 
}

// ---
void COMMODORE::VIAControlLine::initialize ()
{ 
	_wire.setValue (false); 
		
	_interruptRequested = false; 
}

// ---
MCHEmul::InfoStructure COMMODORE::VIAControlLine::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("VALUE",	_wire.value ());
	result.add ("MODE",		_mode);
	result.add ("POSEDGE",	_wire.peekPositiveEdge ());
	result.add ("NEGEDGE",	_wire.peekNegativeEdge ());

	return (result);
}

// ---
bool COMMODORE::VIAControlLineType1::simulate (MCHEmul::CPU* cpu)
{
	switch (_mode)
	{
		case 0x00:
			{
				if (peekNegativeEdge ())
				{ 
					assert (_P != nullptr);

					_interruptRequested = true;

					_P -> setLatchIR (true);
				}
			}

			break;

		case 0x01:
			{
				if (peekPositiveEdge ())
				{
					assert (_P != nullptr);

					_interruptRequested = true;

					_P -> setLatchIR (true);
				}
			}

			break;

		default:
			{ 
				// It shouldn't be here...
				_LOG ("VIA Control Type 1 mode not supported:" + 
					std::to_string ((int) _mode));
				assert (false); // Just when compiling under debug mode...
			}

			break;
	}

	return (true);
}

// ---
void COMMODORE::VIAControlLineType2::whenReadWritePort (bool r)
{
	// @see simulate method

	switch (_mode)
	{
		case 0x00:
		case 0x02:
			{
				interruptRequested (); // Doing so, the flag is cleared (it is OBool)
			}

			break;

		case 0x01:
		case 0x03:
			break;

		case 0x04:
			{
				if (!r)
					setValue (false); 
			}

			break;

		case 0x05:
			{
				if (!r)
				{
					setValue (false);

					_justMovedToFalse = true;
				}
			}

			break;

		case 0x06:
		case 0x07:
			break;

		default:
			{ 
				// It shouldn't be here...
				_LOG ("VIA Control Type 2 mode not supported:" + 
					std::to_string ((int) _mode));
				assert (false); // Just when compiling under debug mode...
			}

			break;
	}
}

// ---
void COMMODORE::VIAControlLineType2::initialize ()
{
	_justMovedToFalse = false;

	COMMODORE::VIAControlLine::initialize ();
}

// ---
bool COMMODORE::VIAControlLineType2::simulate (MCHEmul::CPU* cpu)
{
	switch (_mode)
	{
		case 0x00:
		case 0x01:
			{
				if (peekNegativeEdge ())
					_interruptRequested = true;
			}

			break;

		case 0x02:
		case 0x03:
			{
				if (peekPositiveEdge ())
					_interruptRequested = true;
			}

			break;

		case 0x04:
			{
				assert (_CL != nullptr);

				if (_CL -> peekTransition ())
					setValue (true);
			}

			break;

		case 0x05:
			{
				if (!_justMovedToFalse)
					setValue (true);
			}

			break;

		case 0x06:
			{
				setValue (false);
			}

			break;

		case 0x07:
			{
				setValue (true);
			}

			break;

		default:
			{ 
				// It shouldn't be here...
				_LOG ("VIA Control Type 2 mode not supported:" + 
					std::to_string ((int) _mode));
				assert (false); // Just when compiling under debug mode...
			}

			break;
	}

	return (true);
}
