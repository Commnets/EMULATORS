/** \ingroup VICII */
/*@{*/

/**	
 *	@file	
 *	File: VICII.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: The VICII Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_VICII__
#define __C64_VICII__

#include <C64/Memory.hpp>

namespace C64
{
	/** The chip that takes care of anything around the graphics in Commodore 64. */
	class VICII : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 1;

		VICII (const MCHEmul::Attributes& attrs)
			: MCHEmul::Chip (_ID, attrs),
			  _VICMemory (nullptr)
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		C64::VICMemory* _VICMemory;
	};

	/** The version para NTSC systems. */
	class VICII_NTSC : public VICII
	{
		public:
		VICII_NTSC ();
	};

	/** The version para PAL systems. */
	class VICII_PAL : public VICII
	{
		public:
		VICII_PAL ();
	};
}

#endif
  
// End of the file
/*@}*/
