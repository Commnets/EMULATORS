/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: GraphicalChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forni?s (EMULATORS library) \n
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
	/**
	  * GraphicalChip is very connected with a Screen. \n
	  * The graphical chip uses the ScreenMemory to write. \n
	  * The same object will be taken by th Screen to finaly put the graphics out to the display. \n
	  * The instance of the ScreenMemory object used is created in the method "createScreenMemory ()" that has to be overload. \n
	  * When a graphic is ready (complete) to be sent to the screen, the method "setGraphicsReady (true)" should be invoked. \n
	  * The Screen, when output the raphics to the display invokes back the method "setGraphicsReady (false)".
	  */
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

