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

namespace C64
{
	class SpecialFunctionsChip final : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 1;

		SpecialFunctionsChip ()
			: MCHEmul::Chip (_ID, 
				{ { "Name", "SFChip" },
				  { "Manufacturer", "Ignacio Cea" },
				  { "Year", "2022" } }),
			  _BasicRAM (nullptr), _BasicROM (nullptr), _KernelROM (nullptr), _KernelRAM (nullptr),
			  _CharROM (nullptr), _VICIIRegisters (nullptr), _SIDRegisters (nullptr), _ColorRAM (nullptr),
			  _CIA1Registers (nullptr), _CIA2registers (nullptr), _IO1Registers (nullptr), _IO2registers (nullptr)
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		MCHEmul::PhisicalStorageSubset* _BasicROM;
		MCHEmul::PhisicalStorageSubset* _BasicRAM;
		MCHEmul::PhisicalStorageSubset* _KernelROM;
		MCHEmul::PhisicalStorageSubset* _KernelRAM;
		MCHEmul::PhisicalStorageSubset* _CharROM;
		MCHEmul::PhisicalStorageSubset* _VICIIRegisters;
		MCHEmul::PhisicalStorageSubset* _SIDRegisters;
		MCHEmul::PhisicalStorageSubset* _ColorRAM;
		MCHEmul::PhisicalStorageSubset* _CIA1Registers;
		MCHEmul::PhisicalStorageSubset* _CIA2registers;
		MCHEmul::PhisicalStorageSubset* _IO1Registers;
		MCHEmul::PhisicalStorageSubset* _IO2registers;
	};
}

#endif
  
// End of the file
/*@}*/
