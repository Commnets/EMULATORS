/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The screen linked to the ZXSpectrum.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_SCREEN__
#define __ZXSPECTRUM_SCREEN__

#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
{
	/** The ZXSpectrum screen is one of the very basic IO systems. */
	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs);

		protected:
		virtual void drawAdditional () override;

		// The char code and the ASCII code are in the same order!
		virtual size_t charCodeFromASCII (unsigned char id) const override
							{ return (std::isalnum ((int) id) ? (id - 0x20) : 0); }
	};

	/** Screen valid for NTSC systems. Used in USA. */
	class ScreenNTSC final : public Screen
	{
		public:
		ScreenNTSC ();
	};

	/** Screen valid for PAL systems. Used in Europe. */
	class ScreenPAL final : public Screen
	{
		public:
		ScreenPAL ();
	};
}

#endif
  
// End of the file
/*@}*/
