/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SFChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 25/07/2022 \n
 *	Description: Chip to regulate special behaviours.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SFCHIP__
#define __C64_SFCHIP__

#include <CORE/incs.hpp>
#include <C64/SFChipRegisters.hpp>

namespace C64
{
	/** Very critical chip. \n
		The direction of the IO Channels are configurated here at the position 0x00. \n
		The configuration of the memory is controlled with this chip, accesing the possition 0x01 of the page 0. */
	class SpecialFunctionsChip final : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 101;

		SpecialFunctionsChip ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier) override;

		private:
		static const MCHEmul::Address _POS0;
		static const MCHEmul::Address _POS1;

		C64::SpecialFunctionsChipRegisters* _SFRegisters;
		MCHEmul::PhysicalStorageSubset* _BasicROM;
		MCHEmul::PhysicalStorageSubset* _BasicRAM;
		MCHEmul::PhysicalStorageSubset* _KernelROM;
		MCHEmul::PhysicalStorageSubset* _KernelRAM;
		MCHEmul::PhysicalStorageSubset* _CharROM;
		MCHEmul::PhysicalStorageSubset* _VICIIRegisters;
		MCHEmul::PhysicalStorageSubset* _SIDRegisters;
		MCHEmul::PhysicalStorageSubset* _ColorRAM;
		MCHEmul::PhysicalStorageSubset* _CIA1Registers;
		MCHEmul::PhysicalStorageSubset* _CIA2registers;
		MCHEmul::PhysicalStorageSubset* _IO1Registers;
		MCHEmul::PhysicalStorageSubset* _IO2registers;

		// Implementation
		MCHEmul::UByte _lastValue0, _lastValue1;
	};
}

#endif
  
// End of the file
/*@}*/
