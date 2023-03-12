#include <C64/PLA.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>
#include <C64/IO6510Registers.hpp>

// ---
C64::PLA::PLA ()
	: MCHEmul::Chip (_ID,
		{ { "Name", "PLA" },
		  { "Code", "906114-01" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _LORAM (true), _HIRAM (true), _CHAREN (true),
	  _EXROM (true), _GAME (true),
	  _statusAffected (false)
{
	// Nothing else to do...
}

bool C64::PLA::initialize ()
{
	_LORAM = _HIRAM = _CHAREN = true;
	_EXROM = _GAME = true;

	_statusAffected = false;

	return (true);
}

// ---
bool C64::PLA::simulate (MCHEmul::CPU* cpu)
{
	if (statusAffected ())
		dynamic_cast <C64::Memory*> (memoryRef ()) -> configureMemoryStructure 
			(/** BASIC		= */ (_LORAM && _HIRAM && _GAME) &&
								 !(_HIRAM && !_GAME && !_EXROM), // and ROMH1 not connected...
			 /** KERNEL		= */ ((_HIRAM && _GAME) || 
								 (_HIRAM && !_GAME && !_EXROM)) && 
								 !(!_GAME && _EXROM), // and ROMH2 not connected
			 /** CHARROM	= */ (_HIRAM && !_CHAREN && _GAME) || 
								 (_LORAM && !_CHAREN && _GAME) || 
								 (_HIRAM && !_CHAREN && !_GAME && !_EXROM),
			 /** ROML		= */ (_LORAM && _HIRAM && !_EXROM) ||
								 (!_GAME && _EXROM),
			 /** ROMH1		= */ (_HIRAM && !_GAME && !_EXROM), // At the same location than BASIC ROM
			 /** ROMH2		= */ (!_GAME && _EXROM) // At the same location than KERNEL ROM
			);

	return (true);
}

// ---
void C64::PLA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier*)
{
	if (evnt.id () == C64::IO6510Registers::_C64PORTIOBITSACTUALIZED)
	{
		_statusAffected = true;

		_LORAM	= (evnt.value () & 0x01) != 0;
		_HIRAM	= (evnt.value () & 0x02) != 0;
		_CHAREN	= (evnt.value () & 0x04) != 0;
	}
	else
	if (evnt.id () == C64::ExpansionIOPort::_C64EXPANSIONPORTMEMLINESACTUALIZED)
	{
		_statusAffected = true;

		_GAME	= !((evnt.value () & 0x01) != 0);
		_EXROM	= !((evnt.value () & 0x02) != 0);
	}
}
