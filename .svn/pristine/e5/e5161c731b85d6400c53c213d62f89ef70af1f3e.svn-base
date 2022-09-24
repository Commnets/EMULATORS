/** \ingroup CIA */
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

#include <C64/Memory.hpp>

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
				  { "Year", "1980" } })
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU*) override;
	};

	/** The chip is to communicate the C64 with the environment. */
	class CIA2 : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 3;

		CIA2 ()
			: MCHEmul::Chip (_ID, 
				{ { "Name", "CIA1" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1980" } })
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU*) override;
	};
}

#endif
  
// End of the file
/*@}*/
