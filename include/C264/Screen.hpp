/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: The screen linked to the C264 series computer.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_SCREEN__
#define __C264_SCREEN__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C264
{
	/** The C264 screen is one of the very basic IO systems. */
	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (const std::string& tt, double hz, int w, int h, const MCHEmul::Attributes& attrs);

		// The char code and the ASCII code ar quite similar!
		virtual size_t charCodeFromASCII (unsigned char id) const override
							{ return ((size_t) (id & ~0b01000000)); }

		protected:
		virtual void drawAdditional () override;
	};

	/** Screen valid for NTSC systems. Used in USA. */
	class ScreenNTSC final : public Screen
	{
		public:
		ScreenNTSC (const std::string& tt);
	};

	/** Screen valid for PAL systems. Used in Europe. */
	class ScreenPAL final : public Screen
	{
		public:
		ScreenPAL (const std::string& tt);
	};
}

#endif
  
// End of the file
/*@}*/
