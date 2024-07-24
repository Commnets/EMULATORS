#include <C64/PLA.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Memory.hpp>
#include <C64/IO6510PortRegisters.hpp>

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
	setClassName ("PLA");
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
	{ 
		// To take trace...
		if (deepDebugActive ())
		{
			*_deepDebugFile
				// Where
				<< "PLA\t" 
				// When
				<< std::to_string (cpu -> clockCycles ()) << "\t" // clock cycles at that point
				// What
				<< "Status\t\t"
				// Data
				<< "_LORAM:"	<< (_LORAM ? "on" : "off")
				<< " _HIRAM:"	<< (_HIRAM ? "on" : "off")
				<< " _CHAREN:"	<< (_CHAREN ? "on" : "off")
				<< " _GAME:"	<< (_GAME ? "on" : "off")
				<< " _EXROM:"	<< (_EXROM ? "on" : "off") << "\n";
		}

		dynamic_cast <C64::Memory*> (memoryRef ()) -> configureMemoryStructure 
			(/** BASIC		= */ (_LORAM && _HIRAM && _GAME) &&
								 !(_HIRAM && !_GAME && !_EXROM), // and ROMH1 not connected...
			 /** KERNEL		= */ ((_HIRAM && _GAME) || 
								 (_HIRAM && !_GAME && !_EXROM)) && 
								 !(!_GAME && _EXROM), // and ROMH2 not connected which happens in ULTIMAX cartridges...
			 /** CHARROM	= */ (_HIRAM && !_CHAREN && _GAME) || 
								 (_LORAM && !_CHAREN && _GAME) || 
								 (_HIRAM && !_CHAREN && !_GAME && !_EXROM),
			 /** ROML		= */ (_LORAM && _HIRAM && !_EXROM) ||
								 (!_GAME && _EXROM), // This also active in ULTIMAX situation...
			 /** ROMH1		= */ (_HIRAM && !_GAME && !_EXROM), // At the same location than BASIC ROM...
			 /** ROMH2		= */ (!_GAME && _EXROM) // At the same location than KERNEL ROM. This is ULTIMAX situation!
			);
	}

	return (true);
}

// ---
void C64::PLA::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier*)
{
	if (evnt.id () == C64::IO6510PortRegisters::_C64PORTIOBITSACTUALIZED)
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

// ---
MCHEmul::InfoStructure C64::PLA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory");
	result.add ("_GAME",	_GAME);
	result.add ("_EXROM",	_EXROM);
	result.add ("_LORAM",	_LORAM);
	result.add ("_HIRAM",	_HIRAM);
	result.add ("_CHAREN",	_CHAREN);

	return (result);
}
