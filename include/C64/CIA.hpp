/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA Chips.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA__
#define __C64_CIA__

#include <CORE/incs.hpp>
#include <C64/CIARegisters.hpp>

namespace C64
{
	/** The chip is to communicate the C64 with the environment. */
	class CIA1 : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 2;

		CIA1 ()
			: MCHEmul::Chip (_ID, 
				{ { "Name", "CIA1" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1980" } }),
			  _CIA1Registers (nullptr)
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		C64::CIA1Registers* _CIA1Registers;
	};

	class VICII;

	/** The chip is to communicate the C64 with the environment. */
	class CIA2 : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 3;

		CIA2 ()
			: MCHEmul::Chip (_ID, 
				{ { "Name", "CIA1" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1980" } }),
			  _CIA2Registers (nullptr),
			  _VICIIRef (nullptr)
							{ }

		void setVICIIRef (VICII* v)
							{ _VICIIRef = v; }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		C64::CIA2Registers* _CIA2Registers;
		VICII* _VICIIRef;
	};
}

#endif
  
// End of the file
/*@}*/
