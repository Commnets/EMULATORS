/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: The screen linked to the C64.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SCREEN__
#define __C64_SCREEN__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C64
{
	/** The C64 screen is one of the very basic IO systems. */
	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs)
			: MCHEmul::Screen ("C64", _ID, w, h, 2, hz, attrs),
			  _drawBorder (false), _borderColor (0)
							{ }

		/** Managing the border. */
		void setDrawBorder (bool dB, unsigned int c = 0)
							{ _drawBorder = dB; _borderColor = c; }

		protected:
		virtual void drawAdditional () override;

		protected:
		bool _drawBorder;
		unsigned int _borderColor;
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
