/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: GraphicalChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/06/2022 \n
 *	Description: A special type of chip mamanging a screen memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_GRAPHICALCHIP__
#define __MCHEMUL_GRAPHICALCHIP__

#include <CORE/Chip.hpp>
#include <CORE/ScreenMemory.hpp>

namespace MCHEmul
{
	class GraphicalChip : public Chip
	{
		public:
		GraphicalChip (int id, const Attributes& attrs = { })
			: Chip (id, attrs), 
			  _screenMemory (nullptr), 
			  _graphicsReady (false)
							{ }

		~GraphicalChip ()
							{ delete (_screenMemory); }

		const ScreenMemory* screenMemory () const
							{ return (_screenMemory); }
		ScreenMemory* screenMemory ()
							{ return (_screenMemory); }

		bool graphicsReady () const
							{ return (_graphicsReady); }
		void setGraphicsReady (bool gR) 
							{ _graphicsReady = gR; }

		virtual bool initialize () override;

		protected:
		/** Invoked from initialize to create the right screen memory. */
		virtual ScreenMemory* createScreenMemory () = 0;

		protected:
		ScreenMemory* _screenMemory;

		// Implementation
		bool _graphicsReady;
	};
}

#endif
  
// End of the file
/*@}*/

