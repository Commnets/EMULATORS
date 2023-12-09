/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: The screen linked to the VIC20.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_SCREEN__
#define __VIC20_SCREEN__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace VIC20
{
	/** The VIC20 screen is one of the very basic IO systems. */
	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 200;
		
		Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs)
			: MCHEmul::Screen ("VIC20", _ID, w, h, 2, hz, attrs),
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
