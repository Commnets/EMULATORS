/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: VDP.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: A wrapper over any graphical chip used in the MSX system.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_VDP__
#define __MSX_VDP__

#include <CORE/incs.hpp>
#include <TEXASINST/incs.hpp>

namespace MSX
{
	/** Just the warpper over any graphical chip compatible with the MSX system. \nç
		Defines a set of common functions over this type of chips. */
	class VDP
	{
		public:
		VDP (MCHEmul::GraphicalChip* gC);

		int id () const
							{ return (_chip -> id ()); }

		const MCHEmul::GraphicalChip* chip () const
							{ return (_chip); }
		MCHEmul::GraphicalChip* chip ()
							{ return (_chip); }

		virtual void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2) const = 0;

		/** To get a picture of the info in the memory. \n
			The info returned will depend on the chip and on the mode the chip is in. */
		virtual MCHEmul::UBytes screenMemorySnapShot () const = 0;
		virtual MCHEmul::UBytes colorMemorySnapShot () const = 0;

		protected:
		MCHEmul::GraphicalChip* _chip;
	};

	/** Over the TEXAS Instruments family. */
	class VDPOverTexasInstruments99XXFamily final : public VDP
	{
		public:
		VDPOverTexasInstruments99XXFamily (TEXASINSTRUMENTS::TMS99XX* tI);

		virtual void screenPositions (unsigned short& x1, unsigned short& y1, 
			unsigned short& x2, unsigned short& y2) const override
							{ return (_texasChip -> screenPositions (x1, y1, x2, y2)); }

		virtual MCHEmul::UBytes screenMemorySnapShot () const override
							{ return (MCHEmul::UBytes ()); }
		virtual MCHEmul::UBytes colorMemorySnapShot () const override
							{ return (MCHEmul::UBytes ()); }

		private:
		TEXASINSTRUMENTS::TMS99XX* _texasChip;
	};
}

#endif
  
// End of the file
/*@}*/
