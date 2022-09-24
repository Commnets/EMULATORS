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

#include <CPU/incs.hpp>

namespace C64
{
	class VICII;

	class Screen : public MCHEmul::Screen
	{
		public:
		static const int _ID = 1;
		Screen (double hz, const MCHEmul::Attributes& attrs = { })
			: MCHEmul::Screen ("C64", _ID, 403, 284, 2, 16, hz, attrs),
			  _vicII (nullptr)
							{ }

		virtual bool initialize () override;

		protected:
		virtual void initializeColorPalette () override;

		protected:
		VICII* _vicII;
	};

	class ScreenNTSC final : public Screen
	{
		public:
		ScreenNTSC ()
			: Screen (59.940f)
							{ }
	};

	class ScreenPAL final : public Screen
	{
		public:
		ScreenPAL ()
			: Screen (50.125f)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
