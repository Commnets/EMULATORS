/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The screen linked to the MSX.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_SCREEN__
#define __MSX_SCREEN__

#include <CORE/incs.hpp>

namespace MSX
{
	class VDP;

	/** The MSX screen is one of the very basic IO systems. */
	class Screen final : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (double hz, int w, int h, VDP* vdp, 
			const MCHEmul::Attributes& attrs);

		private:
		virtual void drawAdditional () override;

		// The char code and the ASCII code are in the same order!
		virtual size_t charCodeFromASCII (unsigned char id) const override
							{ return (std::isalnum ((int) id) ? (id - 0x20) : 0); }

		private:
		VDP* _vdp;
	};
}

#endif
  
// End of the file
/*@}*/
