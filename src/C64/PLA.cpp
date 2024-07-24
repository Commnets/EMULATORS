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

		// Managing the configuration of the memory
		// How to identify the type of cartrige connected?
		// none/ram	: _EXROM = true		_GAME = true
		// 8k		: _EXROM = false	_GAME = true
		// 16k		: _EXROM = false	_GAME = false
		// Ultimax	: _EXROM = true		_GAME = false
		// Other common situations to compare:
		// none & 8K:					_GAME = true
		// The info has been extracted from: https://skoe.de/docs/c64-dissected/pla/c64_pla_dissected_a4ds.pdf

		// To simplify the understanding...
		bool noneOrC8k = _GAME;
		bool c16k = !_EXROM && !_GAME;
		bool c8Or16k = !_EXROM;
		bool ultimax = _EXROM && !_GAME;

		// ---
		// Standard memory zones
		// BASIC active?
		// $a000 - $bfff: BASICROM from CPU Perspective. Read access only. No cartridge or 8k:
		bool basicActive  = _LORAM && _HIRAM && noneOrC8k; // Very basic condition...

		// KERNEL active?
		// $e000 - $fff: KERNELROM from CPU perspective. Read access only. No cartridge or 8k:
		// bool p1  = _HIRAM && noneOrC8k;
		// $e000 - $fff: KERNELROM from CPU perspective. Read access only. Cartridge 16k:
		// bool p2  = _HIRAM && c16k;
		// p1 || p2 = (_HIRAM && noneOrC8k) || (_HIRAM && c16k) = _HIRAM && (noneOrC8k || c16k);
		bool kernelActive = _HIRAM && (noneOrC8k || c16k);

		// CHARROM active? (from CPU perspective)
		// $d000 - $dfff: CHARROM from CPU perspective. Read access only. No cartridge or 8K:
		// bool p3  = _HIRAM && !_CHAREN && noneOrC8k;
		// bool p4  = _LORAM && !_CHAREN && noneOrC8k;
		// $d000 - $dfff: CHARROM From CPU perspective. Read access only. Cartridge 16K:
		// bool p5  = _HIRAM && !_CHAREN && c16k;
		bool charromActive_cpu = 
			(_HIRAM && !_CHAREN && noneOrC8k) || 
			(_LORAM && !_CHAREN && noneOrC8k) || 
			(_HIRAM && !_CHAREN && c16k);

		// CHARROM active? (from VICII perspective)
		// $1000 - $1fff, $9000 - $9fff: CHARROM from VICII perspective. Read access only. No cartridge or 8K:
		// bool p6  = noneOrC8k;
		// $1000 - $1FFF, $9000 - $9FFF: CHARROM from VICII perspective. Read access only. Cartridge 16K;
		// bool p7  = c16k;
		bool charromActive_vicII = noneOrC8k || c16k; // It is in any circunstance not Ultimax!

		// p8 is not implemented...

		// IO active?
		// This space is not shown under any circunstance from the VICII....
		// $d000 - $dfff: IO from CPU perspective. Read & Write access. No cartridge or 8K:
		// bool p9  = _HIRAM && _CHAREN && noneOrC8k;
		// p10 is the same than p9. 1 talking about read access and the other one talking about write one...
		// But the way it has been implemented here just one is enough...
		// $d000 - $dfff IO from CPU perspective. Read & Write access. No cartridge or 8K:
		// bool p11 = _LORAM && _CHAREN && noneOrC8k;
		// p12 is the same than p11. 1 talking about read access and the other one talking about write one...
		// But the way it has been implemented here just one is enough...
		// $d000 - $dfff: IO from CPU perspective. Read & Write access. Cartridge 16K:
		// bool p13 = _HIRAM && _CHAREN && c16k;
		// p14 is the same than p13. 1 talking about read access and the other one talking about write one...
		// But the way it has been implemented here just one is enough...
		// $d000 - $dfff: IO from CPU perspective. Read & Write access. Cartridge 16K:
		// bool p15 = _LORAM && _CHAREN && c16k;
		// p16 is the same than p15. 1 talking about read access and the other one talking about write one...
		// But the way it has been implemented here just one is enough...
		// $d000 - $dfff: IO from CPU perspective. Read & Write access. Ultimax mode:
		// bool p17 = ultimax;
		// p18 is the same than p16. 1 talking about read access and the other one talking about write one...
		// But the way it has been implemented here just one is enough...
		bool ioActive_cpu = 
			(_HIRAM && _CHAREN && noneOrC8k) || 
			(_LORAM && _CHAREN && noneOrC8k) || 
			(_HIRAM && _CHAREN && c16k) || 
			(_LORAM && _CHAREN && c16k) || ultimax;
		// ---

		// ---
		// Cartridge zones
		// ROML active? (this configuration will be transmited to the cartridge)
		// $8000 - $9fff : ROML from CPU perspective. Read access. Cartridge 8K or 16k:
		// bool p19 = _LORAM && _HIRAM && c8Or16k;
		// $8000 - $9fff : ROML from CPU perspective. Read access. Ultimax mode:
		// bool p20 = ultimax;
		bool romlActive = 
			(_LORAM && _HIRAM && c8Or16k) ||
			ultimax;

		// ROMH1 active? (this configuration will be transmited to the cartridge)
		// $a000 - $bfff : ROMH1 from CPU perspective. Read access. Cartridge 16k:
		bool romh1Active_cpu = _HIRAM && c16k;
		// ROMH2 active? (this configuration will be transmited to the cartridge)
		// $e000 - $ffff : ROMH from CPU perspective. Read access. Ultimax mode:
		bool romh2Active_cpu = ultimax;
		// ROMH2 active? (from VICII perspective)
		// $3000 - $3FFF, $7000 - $7FFF, $B000 - $BFFF, $E000 - $EFFF : ROMH from VICII perspective. Read access. Ultimax mode.
		bool romh2Active_vicII = ultimax;
		// ---

		// In Ultimax mode only the RAM up to $1000 (4k) is accesible.
		// All conditions are the same when the address lines are excluded.
		// $1000 - $1fff, $3000 - $3fff : Read & Write access forbiden. Ultimax mode:
		// bool p24 = _EXROM && !_GAME;
		// $2000 - $3fff : Read & Write access forbiden. Ultimax mode:
		// bool p25 = _EXROM && !_GAME;
		// $4000 - $7fff : Read & Write access forbiden. Ultimax mode:
		// bool p26 = _EXROM && !_GAME;
		// $a000 - $bfff : Read & Write access forbiden. Ultimax mode:
		// bool p27 = _EXROM && !_GAME;
		// $c000 - $cfff : Read & Write access forbiden. Ultimax mode:
		// bool p28 = _EXROM && !_GAME;
		bool casramActive = !ultimax;

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
