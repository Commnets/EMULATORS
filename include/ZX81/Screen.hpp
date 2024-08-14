/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: The screen linked to the ZX81.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_SCREEN__
#define __ZX81_SCREEN__

#include <SINCLAIR/incs.hpp>

namespace ZX81
{
	/** The ZX81 screen is one of the very basic IO systems. */
	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs);

		protected:
		virtual void drawAdditional () override;

		// The char code and the ASCII code are quite similar!
		virtual size_t charCodeFromASCII (unsigned char id) const override
							{ return (std::isalnum ((int) id) // Only alphanumeric are represented...
								? (std::isdigit ((int) id) ? (id - 20) : (id - 27))
								: 0); }
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
